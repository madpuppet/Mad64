#include "common.h"
#include "vic.h"
#include "c64ram.h"

static u32 s_palette[16] = {
    0x000000, 0xffffff, 0x880000, 0xaaffee,
    0xcc44cc, 0x00cc55, 0x0000AA, 0xeeee77,
    0xdd8855, 0x664400, 0xff7777, 0x333333,
    0x777777, 0xaaff66, 0x0088ff, 0xbbbbbb
};

Vic::Vic()
{
    m_scPal.screenHeight = 312;
    m_scPal.screenWidth = 504;
    m_scPal.cyclesPerLine = 63;
    m_scPal.badLineCycles = 40;

    // 6 + 5 + 40 + 5 + 7 == 63
    m_scPal.leftBorderStartCycle = 6;
    m_scPal.backgroundStartCycle = 11;
    m_scPal.rightBorderStartCycle = 51;
    m_scPal.hblankStartCycles = 56;

    // 30 + 16 + 200 + 16 + 30
    m_scPal.topBorderStartLine = 16;
    m_scPal.backgroundStartLine = 51;
    m_scPal.bottomBorderStartLine = 251;
    m_scPal.bottomBorderVBlankLine = 286;

    m_scNtsc.screenHeight = 312;
    m_scNtsc.screenWidth = 504;
    m_scNtsc.cyclesPerLine = 65;
    m_scNtsc.badLineCycles = 40;

    // 7 + 5 + 40 + 5 + 8
    m_scNtsc.leftBorderStartCycle = 7;
    m_scNtsc.backgroundStartCycle = 12;
    m_scNtsc.rightBorderStartCycle = 52;
    m_scNtsc.hblankStartCycles = 57;

    m_scNtsc.topBorderStartLine = 30;
    m_scNtsc.backgroundStartLine = 46;
    m_scNtsc.bottomBorderStartLine = 246;
    m_scNtsc.bottomBorderVBlankLine = 262;

    m_scCurrent = &m_scPal;

    m_texture = SDL_CreateTexture(gApp->GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_scCurrent->screenWidth, m_scCurrent->screenHeight);
    m_textureMem = (u8*)SDL_malloc(m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);

    memset(m_textureMem, 0, m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);

    SDL_Rect area = { 0, 0, m_scCurrent->screenWidth, m_scCurrent->screenHeight };
    SDL_UpdateTexture(m_texture, &area, m_textureMem, m_scCurrent->screenWidth * 4);

    Reset();
}

void Vic::Render(int x, int y, int zoom)
{
    FlushTexture();
    SDL_Rect dest = { x, y, m_scCurrent->screenWidth * zoom, m_scCurrent->screenHeight * zoom };
    SDL_RenderCopy(gApp->GetRenderer(), m_texture, nullptr, &dest);
}

void Vic::FlushTexture()
{
    // check overlap
    if (m_textureDirtyExtra.h > 0)
    {
        int y1 = m_textureDirty.y;
        int y2 = m_textureDirty.y + m_textureDirty.h;
        int ey1 = m_textureDirtyExtra.y;
        int ey2 = m_textureDirtyExtra.y+m_textureDirtyExtra.h;

        // if overlap, combine them
        if (ey1 <= y2 && ey2 >= y1)
        {
            int newY1 = min(ey1, y1);
            int newY2 = max(ey2, y2);
            m_textureDirty.y = newY1;
            m_textureDirty.h = newY2 - newY1;
            m_textureDirtyExtra.h = 0;
        }
    }

    if (m_textureDirty.h > 0)
    {
        void* pixels;
        int pitch;
        SDL_LockTexture(m_texture, &m_textureDirty, &pixels, &pitch);
        memcpy(pixels, m_textureMem + m_textureDirty.y * pitch, pitch * m_textureDirty.h);
        SDL_UnlockTexture(m_texture);
        m_textureDirty.y = m_rasterLine;
        m_textureDirty.h = 0;
    }

    if (m_textureDirtyExtra.h > 0)
    {
        void* pixels;
        int pitch;
        SDL_LockTexture(m_texture, &m_textureDirtyExtra, &pixels, &pitch);
        memcpy(pixels, m_textureMem + m_textureDirtyExtra.y * pitch, pitch * m_textureDirtyExtra.h);
        SDL_UnlockTexture(m_texture);
        m_textureDirtyExtra.h = 0;
    }
}

void Vic::Reset()
{
    memset(m_textureMem, 0, m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);
    m_textureDirty = { 0,0,m_scCurrent->screenWidth, m_scCurrent->screenHeight };
    m_textureDirtyExtra = { 0,0,m_scCurrent->screenWidth, 0 };

    m_rasterLine = 0;
    m_rasterLineCycle = 0;
    m_charRow = 0;
    m_backgroundCycle = 0;
    m_bVBorder = true;
    m_bHBorder = true;
    m_bHBlank = true;
    m_bVBlank = true;
    m_bBGVert = false;
    m_bBGHoriz = false;

    memcpy(&m_regs, gC64_vicIIRegisters, sizeof(m_regs));

    //test pattern
    u32* ptr = (u32*)m_textureMem;
    for (int y = 0; y < m_scCurrent->screenHeight; y++)
    {
        for (int x = 0; x < m_scCurrent->screenWidth; x++)
        {
            int c = (x + y) & 1;
            *ptr++ = (c ? 0xff400000 : 0xff000040);
        }
    }

    ResetSpriteFrame();
}

void Vic::CacheLine()
{
    m_bBMM = m_regs.control1 & BMM;
    m_bECM = m_regs.control1 & ECM;
    m_bMCM = m_regs.control1 & MCM;

    // grab 40 bytes of character data
    u16 videoAddr = (((u16)(m_regs.memoryPointers & VideoMatrix)) << 6) + m_charRow * 40;
    u16 colorAddr = m_charRow * 40;
    for (int i = 0; i < 40; i++)
    {
        m_cachedChars[i] = ReadVicByte(videoAddr+i) | (ReadColorByte(colorAddr+i) << 8);
    }
}

void Vic::RasterizeSprite(int i, u32 pixels[8])
{
    auto& sprCache = m_spriteCache[i];
    if (sprCache.multicolor)
    {
        u32 col[3];
        col[0] = s_palette[*(&m_regs.spriteColor0 + i)];
        col[1] = s_palette[m_regs.spriteMulticolor0];
        col[2] = s_palette[m_regs.spriteMulticolor1];
        if (sprCache.sizeX)
        {
            for (int i = 0; i < 8; i += 2)
            {
                u32 idx = (sprCache.shiftedData >> 30) & 3;
                if (idx > 0)
                {
                    pixels[i] = col[idx - 1];
                    pixels[i + 1] = col[idx - 1];
                }
                sprCache.shiftedData <<= 2;
            }
        }
        else
        {
            for (int i = 0; i < 4; i += 2)
            {
                u32 idx = (sprCache.shiftedData >> 30) & 3;
                if (idx > 0)
                {
                    pixels[i] = col[idx - 1];
                    pixels[i + 1] = col[idx - 1];
                    pixels[i + 2] = col[idx - 1];
                    pixels[i + 3] = col[idx - 1];
                }
                sprCache.shiftedData <<= 2;
            }
        }
    }
    else
    {
        u32 col = s_palette[*(&m_regs.spriteColor0 + i)];
        if (sprCache.sizeX)
        {
            for (int i = 0; i < 8; i += 2)
            {
                if (sprCache.shiftedData & 0x80000000)
                {
                    pixels[i] = col;
                    pixels[i + 1] = col;
                }
                sprCache.shiftedData <<= 1;
            }
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                if (sprCache.shiftedData & 0x80000000)
                {
                    pixels[i] = col;
                }
                sprCache.shiftedData <<= 1;
            }
        }
    }
    sprCache.cycleCount++;
}

void Vic::Step()
{
    u32* videoOut = (u32*)(m_textureMem + (m_rasterLineCycle + m_rasterLine * m_scCurrent->cyclesPerLine) * 8 * 4);

    // init our pixels out to bg color
    u32 pixels[8];

    // prime with background color
    u32 col = s_palette[m_regs.backgroundColor0 & 15];
    for (int i = 0; i < 8; i++)
        pixels[i] = col;

    u16 charMapAddr = (u16)(m_regs.memoryPointers & CharacterBank) << 10;

    // sprite data for this cycle
    for (int i = 0; i < 8; i++)
    {
        int bit = 1 << i;
        auto& sprCache = m_spriteCache[i];

        // at the start of each rasterline we'll start sprites/grab data/etc
        if (m_rasterLineCycle == 0)
        {
            // inc raster line if we are already started
            if (sprCache.startraster)
            {
                if (sprCache.sizeY && !sprCache.extraLineLatch)
                {
                    sprCache.extraLineLatch = true;
                }
                else
                {
                    sprCache.rasterLine++;
                    sprCache.extraLineLatch = false;
                }
                if (sprCache.rasterLine == 21)
                {
                    sprCache.startraster = false;
                }
            }

            // lock in our sprite raster lines once we the sprite Y matches the rasterline
            if (!sprCache.startraster && (m_regs.spriteEnable & bit))
            {
                int spriteY = *(&m_regs.sprite0Y + 2 * i);
                if (m_rasterLine == spriteY+1)
                {
                    // ready to start rendering sprite
                    sprCache.y = *(&m_regs.sprite0Y + 2);
                    sprCache.startraster = true;
                    sprCache.rasterLine = 0;
                }
            }

            // if we have started rasterizing, then grab some sprite info at the start of each line
            if (sprCache.startraster)
            {
                sprCache.x = *(&m_regs.sprite0X + 2 * i) + ((m_regs.spriteXMSB & bit) ? 0x100 : 0);
                sprCache.sizeY = (m_regs.spriteYEnlarge & bit) ? true : false;
                sprCache.sizeX = (m_regs.spriteXEnlarge & bit) ? true : false;
                sprCache.pri = (m_regs.spritePriority & bit) ? true : false;
                sprCache.multicolor = (m_regs.spriteMulticolor & bit) ? true : false;
                sprCache.startcycle = false;
                sprCache.firstCycle = sprCache.x / 8 + m_scCurrent->leftBorderStartCycle;
            }
        }

        if (sprCache.startraster && !sprCache.startcycle && m_rasterLineCycle == sprCache.firstCycle)
        {
            sprCache.startcycle = true;
            sprCache.cycle = 0;
            sprCache.cycleCount = sprCache.sizeX ? 6 : 3;

            // grab sprite data
            u16 videoAddr = (((u16)(m_regs.memoryPointers & VideoMatrix)) << 6);
            u16 spriteFramePtr = (videoAddr + 0x3f8);
            u8 spriteFrame = ReadVicByte(spriteFramePtr);
            u16 spriteAddr = spriteFrame * 64 + sprCache.rasterLine * 3;
            sprCache.data = ((u32)ReadVicByte(spriteAddr) << 16) + ((u32)ReadVicByte(spriteAddr + 1) << 8) + (u32)ReadVicByte(spriteAddr + 2);
            sprCache.shiftedData = sprCache.data << (8 - (sprCache.x & 7));
        }
    }

    // rasterize sprites that are priority behind foreground
    for (int i = 0; i < 8; i++)
    {
        auto& sprCache = m_spriteCache[i];
        if (sprCache.startcycle && sprCache.cycle < sprCache.cycleCount && sprCache.pri)
        {
            RasterizeSprite(i, pixels);
        }
    }


    // rasterize foreground - being either text or bitmap
    if (m_bBGVert && m_bBGHoriz)
    {
        u8 bgPal = m_regs.backgroundColor0 & 0xf;
        u8 dataOut[8];
        for (int i = 0; i < 8; i++)
            dataOut[i] = bgPal;

        if (m_regs.control1 & BMM)
        {
            // bitmap mode
        }
        else
        {
            // text mode
            // fetch char byte
            u16 ch = m_cachedChars[m_charCol];
            u16 charAddr = charMapAddr + ((ch & 0xff) * 8) + m_charLine;
            u8 data = ReadVicByte(charAddr);
            u32 foregroundCol = s_palette[(ch >> 8) & 0xf];

            // blit each pixel
            for (int i = 0; i < 8; i++)
            {
                if (data & (1 << (7 - i)))
                    pixels[i] = foregroundCol;
            }
        }
    }

    // rasterize sprites that are priority in front of foreground
    for (int i = 0; i < 8; i++)
    {
        auto& sprCache = m_spriteCache[i];
        if (sprCache.startcycle && sprCache.cycle < sprCache.cycleCount && !sprCache.pri)
        {
            RasterizeSprite(i, pixels);
        }
    }

    // if we're in border, we overwrite the output with the border color
    if (m_bVBorder || m_bHBorder)
    {
        u32 col = s_palette[m_regs.borderColor & 15];
        for (int i = 0; i < 8; i++)
            pixels[i] = col;
    }

    if (!m_bHBlank && !m_bVBlank)
    {
        // finally write pixels to the video texture
        for (int i = 0; i < 8; i++)
            videoOut[i] = pixels[i];
    }

    // next cycle...
    m_rasterLineCycle++;
    m_charCol++;

    if (!m_textureDirty.h)
        m_textureDirty.h = 1;

    if (m_rasterLineCycle == m_scCurrent->leftBorderStartCycle)
        m_bHBlank = false;
    else if (m_rasterLineCycle == m_scCurrent->hblankStartCycles)
        m_bHBlank = true;
    else if (m_rasterLineCycle == m_scCurrent->backgroundStartCycle)
    {
        m_bHBorder = !m_bHBorder;
        m_bBGHoriz = true;
        m_charCol = 0;
    }
    else if (m_rasterLineCycle == m_scCurrent->rightBorderStartCycle)
    {
        m_bBGHoriz = false;
        m_bHBorder = !m_bHBorder;
    }
    else if (m_rasterLineCycle == m_scCurrent->cyclesPerLine)
    {
        m_charLine++;
        m_rasterLine++;
        m_rasterLineCycle = 0;
        m_textureDirty.h = min(m_textureDirty.h + 1, m_scCurrent->screenHeight - m_textureDirty.y);

        if (m_rasterLine == m_scCurrent->topBorderStartLine)
        {
            m_bVBlank = false;
        }
        else if (m_rasterLine == m_scCurrent->bottomBorderVBlankLine)
        {
            m_bVBlank = true;
        }

        if (m_rasterLine == m_scCurrent->backgroundStartLine)
        {
            m_bVBorder = !m_bVBorder;
            m_bBGVert = true;
            m_charRow = 0;
            m_charLine = 0;
            CacheLine();
        }
        else if (m_rasterLine == m_scCurrent->bottomBorderStartLine)
        {
            m_bVBorder = !m_bVBorder;
            m_bBGVert = false;
        }
        else if (m_bBGVert && (m_charLine == 8))
        {
            m_charRow++;
            CacheLine();
            m_charLine = 0;
        }

        if (m_rasterLine == m_scCurrent->screenHeight)
        {
            m_textureDirtyExtra = m_textureDirty;
            m_textureDirty.y = 0;
            m_textureDirty.h = 0;
            m_rasterLine = 0;
        }
    }

    // update vic registers
    m_regs.control1 = (m_regs.control1 & 0x7f) | ((m_rasterLine >> 1) & 0x80);
    m_regs.rasterCounter = m_rasterLine & 0xff;
}

