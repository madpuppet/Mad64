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
    memset(m_colorMem, 14, 1024);
    memset(m_textureMem, 0, m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);
    m_textureDirty = { 0,0,m_scCurrent->screenWidth, m_scCurrent->screenHeight };
    m_textureDirtyExtra = { 0,0,m_scCurrent->screenWidth, 0 };

    m_rasterLine = 0;
    m_rasterLineCycle = 0;
    m_charRow = 0;
    m_backgroundCycle = 0;
    m_bVBlank = true;
    m_bHBlank = true;
    m_bVBorder = false;
    m_bHBorder = false;
    m_bVBackground = false;
    m_bHBackground = false;

    memcpy(&m_regs, gC64_vicIIRegisters, sizeof(m_regs));
    m_regs.control1 = 0x1b;

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

    m_interruptRasterline = 0;
    m_interruptLatch = 0;
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
        m_cachedChars[i] = ReadVicByte(videoAddr+i) | (m_colorMem[colorAddr+i] << 8);
    }
}

void Vic::ResetSpriteFrame()
{
    for (int i = 0; i < 8; i++)
    {
        m_rc.spriteCache[i].startraster = false;
        m_rc.spriteCache[i].startcycle = false;
    }
}

void Vic::RasterizeSprites()
{
    // sprite data for this cycle
    for (int i = 0; i < 8; i++)
    {
        int bit = 1 << i;
        auto& sprCache = m_rc.spriteCache[i];

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
                if (m_rasterLine == spriteY + 1)
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
                sprCache.multicolor = (m_regs.spriteMulticolor & bit) ? true : false;
                sprCache.pri = (m_regs.spritePriority & bit) ? 1 : 0;
                sprCache.startcycle = false;
                sprCache.firstCycle = sprCache.x / 8 + m_scCurrent->leftBorderStartCycle;

                // grab sprite data
                u16 videoAddr = (((u16)(m_regs.memoryPointers & VideoMatrix)) << 6);
                u16 spriteFramePtr = (videoAddr + 0x3f8 + i);
                u8 spriteFrame = ReadVicByte(spriteFramePtr);
                u16 spriteAddr = spriteFrame * 64 + sprCache.rasterLine * 3;
                sprCache.data = ((u32)ReadVicByte(spriteAddr) << 16) + ((u32)ReadVicByte(spriteAddr + 1) << 8) + (u32)ReadVicByte(spriteAddr + 2);

                // decode data to cache pixels
                memset(sprCache.pixels, 0, 56);      // clear the first 8 pixels to background.  the others get written over
                if (sprCache.sizeX)
                {
                    if (sprCache.multicolor)
                    {
                        for (int b = 0; b < 24; b += 2)
                        {
                            int mask = (3 << (22 - b));
                            int bits = sprCache.data & mask;
                            int val = bits >> (22 - b);
                            int outidx = b * 2 + (sprCache.x & 7);
                            sprCache.pixels[outidx] = val;
                            sprCache.pixels[outidx + 1] = val;
                            sprCache.pixels[outidx + 2] = val;
                            sprCache.pixels[outidx + 3] = val;
                        }
                    }
                    else
                    {
                        for (int b = 0; b < 24; b += 1)
                        {
                            int mask = (1 << (23 - b));
                            int bits = sprCache.data & mask;
                            int val = bits >> (23 - b);
                            int outidx = b * 2 + (sprCache.x & 7);
                            sprCache.pixels[outidx] = val;
                            sprCache.pixels[outidx + 1] = val;
                        }
                    }
                }
                else
                {
                    if (sprCache.multicolor)
                    {
                        for (int b = 0; b < 24; b += 2)
                        {
                            int mask = (3 << (22 - b));
                            int bits = sprCache.data & mask;
                            int val = bits >> (22 - b);
                            int outidx = b + (sprCache.x & 7);
                            sprCache.pixels[outidx] = val;
                            sprCache.pixels[outidx + 1] = val;
                        }
                    }
                    else
                    {
                        for (int b = 0; b < 24; b += 1)
                        {
                            int mask = (1 << (23 - b));
                            int bits = sprCache.data & mask;
                            int val = bits >> (23 - b);
                            int outidx = b + (sprCache.x & 7);
                            sprCache.pixels[outidx] = val;
                        }
                    }
                }
            }
        }

        if (sprCache.startraster && !sprCache.startcycle && m_rasterLineCycle == sprCache.firstCycle)
        {
            sprCache.startcycle = true;
            sprCache.cycle = 0;
            sprCache.cycleCount = sprCache.sizeX ? 7 : 4;
        }

        sprCache.collisionMask = 0;
        if (sprCache.startcycle && sprCache.cycle < sprCache.cycleCount)
        {
            RasterizeSprite(i);
        }
    }

    // check sprite collisions and cycle sprite
    for (int i = 0; i < 8; i++)
    {
        auto& sprCache = m_rc.spriteCache[i];
        if (sprCache.startcycle)
        {
            for (int s = 0; s < 8; s++)
            {
                if ((s != i) && ((sprCache.collisionMask & m_rc.spriteCache[s].collisionMask) != 0))
                {
                    m_regs.spriteSpriteCollision |= 1 << s;
                    break;
                }
            }

            if (sprCache.cycle < sprCache.cycleCount)
                sprCache.cycle++;
        }
    }
}

void Vic::RasterizeSprite(int i)
{
    auto& sprCache = m_rc.spriteCache[i];
    u8 col[4];
    col[0] = m_regs.backgroundColor0;
    if (sprCache.multicolor)
    {
        col[3] = m_regs.spriteMulticolor1;
        col[2] = *(&m_regs.spriteColor0 + i);
        col[1] = m_regs.spriteMulticolor0;
    }
    else
    {
        col[3] = col[2] = col[1] = (*(&m_regs.spriteColor0 + i)) & 0xf;
    }

    int spriteBit = 1 << i;
    sprCache.collisionMask = 0;
    for (int b = 0; b < 8; b++)
    {
        int pix = sprCache.pixels[sprCache.cycle * 8 + b];
        if (pix)
        {
            m_rc.spritePixels[b] = col[pix&3];
            m_rc.spritePixelsPri[b] = sprCache.pri;
            m_rc.spritePixelsDat[b] = spriteBit;
            sprCache.collisionMask |= 1 << b;
        }
    }
}

void Vic::RasterizeScreen_NormalTextMode()
{
    // text mode
    // fetch char byte
    u16 ch = m_cachedChars[m_charCol];
    u16 charMapAddr = (u16)(m_regs.memoryPointers & CharacterBank) << 10;
    u16 charAddr = charMapAddr + ((ch & 0xff) * 8) + m_charLine;
    u8 data = ReadVicByte(charAddr);
    u8 foregroundCol = (ch >> 8) & 0xf;

    // blit each pixel
    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << (7 - i)))
        {
            m_rc.screenPixels[i] = foregroundCol;
            m_rc.screenPixelsDat[i] = 1;
        }
    }
}

void Vic::RasterizeScreen_MulticolorTextMode()
{
    // text mode
    // fetch char byte
    u16 ch = m_cachedChars[m_charCol];
    u16 charMapAddr = (u16)(m_regs.memoryPointers & CharacterBank) << 10;
    u16 charAddr = charMapAddr + ((ch & 0xff) * 8) + m_charLine;
    u8 data = ReadVicByte(charAddr);
    u8 foregroundCol = (ch >> 8) & 0xf;

    // multicolor mode only applies if text colour bit 3 is set
    if (foregroundCol & 0x8)
    {
        u8 col[4] = { (u8)(m_regs.backgroundColor0 & 0xf), (u8)(m_regs.backgroundColor1 & 0xf), (u8)(m_regs.backgroundColor2 & 0xf), (u8)(foregroundCol & 0x7) };

        // blit each pixel
        for (int i = 0; i < 8; i += 2)
        {
            int c = (data >> (6 - i)) & 3;
            m_rc.screenPixels[i] = col[c] & 0xf;
            m_rc.screenPixels[i + 1] = col[c] & 0xf;
            if (c > 1)
            {
                m_rc.screenPixelsDat[i] = 1;
                m_rc.screenPixelsDat[i + 1] = 1;
            }
        }
    }
    else
    {
        u8 fcol = foregroundCol & 0x7;
        for (int i = 0; i < 8; i++)
        {
            if (data & (1 << (7 - i)))
            {
                m_rc.screenPixels[i] = fcol;
                m_rc.screenPixelsDat[i] = 1;
            }
        }
    }
}

void Vic::RasterizeScreen_ExtendedColorTextMode()
{
    // text mode
    // fetch char byte
    u16 ch = m_cachedChars[m_charCol];
    u16 charMapAddr = (u16)(m_regs.memoryPointers & CharacterBank) << 10;
    u16 charAddr = charMapAddr + ((ch & 0x3f) * 8) + m_charLine;
    u8 data = ReadVicByte(charAddr);
    u8 extendedColor = (ch >> 6) & 3;
    u8 bgColor = (&m_regs.backgroundColor0)[extendedColor];
    u8 foregroundCol = (ch >> 8) & 0xf;

    // blit each pixel
    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << (7 - i)))
        {
            m_rc.screenPixels[i] = foregroundCol;
            m_rc.screenPixelsDat[i] = 1;
        }
        else
        {
            m_rc.screenPixels[i] = bgColor;
            m_rc.screenPixelsDat[i] = 0;
        }
    }
}

void Vic::RasterizeScreen_NormalBitmapMode()
{
    // text mode
    // fetch char byte
    u16 ch = m_cachedChars[m_charCol];
    u16 bitmapAddr = ((u16)(m_regs.memoryPointers & 0x08) << 10) + (m_charRow * 40 + m_charCol) * 8 + m_charLine;
    u8 data = ReadVicByte(bitmapAddr);
    u8 fgCol = (ch >> 4) & 0xf;
    u8 bgCol = ch & 0xf;

    // blit each pixel
    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << (7 - i)))
        {
            m_rc.screenPixels[i] = fgCol;
            m_rc.screenPixelsDat[i] = 1;
        }
        else
        {
            m_rc.screenPixels[i] = bgCol;
        }
    }
}
void Vic::RasterizeScreen_MulticolorBitmapMode()
{
    // text mode
    // fetch char byte
    u16 ch = m_cachedChars[m_charCol];
    u16 bitmapAddr = ((u16)(m_regs.memoryPointers & 0x08) << 10) + (m_charRow * 40 + m_charCol) * 8 + m_charLine;
    u8 data = ReadVicByte(bitmapAddr);
    u8 col[4] = { m_regs.backgroundColor0, (u8)((ch >> 4) & 0xf), (u8)(ch & 0xf), (u8)((ch >> 8) & 0xf) };

    // blit each pixel
    for (int i = 0; i < 8; i+=2)
    {
        int dat = (data >> (6 - i)) & 3;
        m_rc.screenPixels[i] = col[dat];
        m_rc.screenPixels[i+1] = col[dat];
        if (dat > 1)
        {
            m_rc.screenPixelsDat[i] = 1;
            m_rc.screenPixelsDat[i+1] = 1;
        }
    }
}
void Vic::RasterizeScreen_InvalidMode()
{
    for (int i = 0; i < 8; i += 2)
    {
        m_rc.screenPixels[i] = 0;
        m_rc.screenPixelsDat[i] = 1;
    }
}

void Vic::RasterizeScreen()
{
    // rasterize foreground - being either text or bitmap
    if (m_bHBackground && m_bVBackground)
    {
        // build mode  ECM | BMM | MCM
        int mode = ((m_regs.control1 & ECM) ? 4 : 0) | ((m_regs.control1 & BMM) ? 2 : 0) | ((m_regs.control2 & MCM) ? 1 : 0);
        switch (mode)
        {
            case 0:     //  ECM:0   BMM:0   MCM:0     normal text mode
                RasterizeScreen_NormalTextMode();
                break;

            case 1:     //  ECM:0   BMM:0   MCM:1     multicolor text mode
                RasterizeScreen_MulticolorTextMode();
                break;

            case 2:     //  ECM:0   BMM:1   MCM:0     normal bitmap mode
                RasterizeScreen_NormalBitmapMode();
                break;

            case 3:     //  ECM:0   BMM:1   MCM:1     multicolor bitmap mode
                RasterizeScreen_MulticolorBitmapMode();
                break;

            case 4:     //  ECM:1   BMM:0   MCM:0     extended color text mode
                RasterizeScreen_ExtendedColorTextMode();
                break;

            case 5:     //  ECM:1   BMM:0   MCM:0     INVALID text mode
                RasterizeScreen_InvalidMode();
                break;

            case 6:     //  ECM:1   BMM:1   MCM:0     INVALID bitmap mode
                RasterizeScreen_InvalidMode();
                break;

            case 7:     //  ECM:1   BMM:1   MCM:1     INVALID bitmap mode
                RasterizeScreen_InvalidMode();
                break;
        }

        // handle bit shift
        int xshift = m_regs.control2 & XSCROLL;
        if (xshift != 0)
        {
            // backup the old extra pixels
            u8 tmpPixels[8];
            u8 tmpPixelsDat[8];
            for (int i = 0; i < 8; i++)
            {
                tmpPixels[i] = m_rc.screenSPixels[i];
                tmpPixelsDat[i] = m_rc.screenSPixelsDat[i];
            }

            // shift out the new pixels
            for (int i = 0; i < xshift; i++)
            {
                m_rc.screenSPixels[i] = m_rc.screenPixels[8 - xshift + i];
                m_rc.screenSPixelsDat[i] = m_rc.screenPixelsDat[8 - xshift + i];
            }

            // shift screen pixels
            for (int i = 0; i < 8-xshift; i++)
            {
                m_rc.screenPixels[7 - i] = m_rc.screenPixels[7 - i - xshift];
                m_rc.screenPixelsDat[7 - i] = m_rc.screenPixelsDat[7 - i - xshift];
            }

            // copy in saved pixels
            for (int i = 0; i < xshift; i++)
            {
                m_rc.screenPixels[i] = tmpPixels[i];
                m_rc.screenPixelsDat[i] = tmpPixelsDat[i];
            }
        }
    }
}

void Vic::Step()
{
    u32* videoOut = (u32*)(m_textureMem + (m_rasterLineCycle + m_rasterLine * m_scCurrent->cyclesPerLine) * 8 * 4);

    // prime with background color
    int bgCol = m_regs.backgroundColor0 & 15;
    for (int i = 0; i < 8; i++)
    {
        m_rc.screenPixels[i] = bgCol;
        m_rc.screenPixelsDat[i] = 0;     // 1 if collidable data
        m_rc.spritePixels[i] = bgCol;
        m_rc.spritePixelsPri[i] = 1;     // 1 if background should take priority
        m_rc.spritePixelsDat[i] = 0;     // 1 if collidable data
    }

    RasterizeSprites();
    RasterizeScreen();

    // increase dirty height to include current line
    m_textureDirty.h = m_rasterLine - m_textureDirty.y + 1;

    // mix background and sprites
    u8 pixels[8];
    for (int i = 0; i < 8; i++)
    {
        pixels[i] = ((m_rc.screenPixelsDat[i] & m_rc.spritePixelsPri[i]) | (!m_rc.spritePixelsDat[i])) ? m_rc.screenPixels[i] : m_rc.spritePixels[i];
    }

    // if we're in border, we overwrite the output with the border color
    if (m_bVBorder || m_bHBorder)
    {
        u8 col = m_regs.borderColor & 15;
        if ((m_regs.control1 & CSEL) || m_bVBorder || (m_rasterLineCycle != m_scCurrent->backgroundStartCycle && m_rasterLineCycle != m_scCurrent->rightBorderStartCycle - 2))
        {
            for (int i = 0; i < 8; i++)
            {
                pixels[i] = col;
            }
        }
        else if (m_rasterLineCycle == m_scCurrent->backgroundStartCycle)
        {
            for (int i = 0; i < 6; i++)
            {
                pixels[i] = col;
            }
        }
        else
        {
            for (int i = 6; i < 8; i++)
            {
                pixels[i] = col;
            }
        }
    }

    if (!m_bVBlank && !m_bHBlank)
    {
        // finally write pixels to the video texture
        for (int i = 0; i < 8; i++)
            videoOut[i] = s_palette[pixels[i]&0xf];
    }

    // next cycle...
    m_rasterLineCycle++;
    m_charCol++;
    if (m_rasterLineCycle == m_scCurrent->cyclesPerLine)
    {
        // new line
        m_charCol = 0;
        m_charLine++;
        m_rasterLineCycle = 0;
        m_rasterLine++;

        if (m_charLine == 8)
        {
            m_charLine = 0;
            m_charRow++;
        }

        // hit bottom of screen, reset to top, start a new texture dirty tracking
        if (m_rasterLine == m_scCurrent->screenHeight)
        {
            m_textureDirtyExtra = m_textureDirty;
            m_textureDirty.y = 0;
            m_textureDirty.h = 0;
            m_rasterLine = 0;
            m_charRow = 0;
        }

        // raster interrupts
        if ((m_regs.interruptEnable & IRST) && m_rasterLine == m_interruptRasterline)
        {
            m_interruptLatch = true;
        }
    }
    if (m_interruptLatch)
    {
        TriggerInterrupt();
    }

    // vblank control
    if (m_rasterLine == m_scCurrent->topBorderStartLine)
        m_bVBlank = false;
    else if (m_rasterLine == m_scCurrent->bottomBorderVBlankLine)
        m_bVBlank = true;
    if (m_rasterLineCycle == m_scCurrent->leftBorderStartCycle)
        m_bHBlank = false;
    else if (m_rasterLineCycle == m_scCurrent->hblankStartCycles)
        m_bHBlank = true;

    // horiz border control
    int leftBorder = (m_regs.control2 & CSEL) ? m_scCurrent->backgroundStartCycle : m_scCurrent->backgroundStartCycle + 1;
    int rightBorder = (m_regs.control2 & CSEL) ? m_scCurrent->rightBorderStartCycle : m_scCurrent->rightBorderStartCycle - 2;
    if (m_rasterLineCycle == m_scCurrent->leftBorderStartCycle)
        m_bHBorder = true;
    else if (m_rasterLineCycle == leftBorder)
        m_bHBorder = false;
    else if (m_rasterLineCycle == rightBorder)
        m_bHBorder = true;
    else if (m_rasterLineCycle == m_scCurrent->hblankStartCycles)
        m_bHBorder = false;

    // vert border control
    int topBorder = (m_regs.control1 & RSEL) ? m_scCurrent->backgroundStartLine : m_scCurrent->backgroundStartLine + 4;
    int bottomBorder = (m_regs.control1 & RSEL) ? m_scCurrent->bottomBorderStartLine : m_scCurrent->bottomBorderStartLine-4;
    if (m_rasterLine == m_scCurrent->topBorderStartLine)
        m_bVBorder = true;
    else if (m_rasterLine == topBorder && (m_regs.control1 & DEN))
        m_bVBorder = false;
    else if (m_rasterLine == bottomBorder)
        m_bVBorder = true;
    else if (m_rasterLine == m_scCurrent->bottomBorderVBlankLine)
        m_bVBorder = false;

    // background control
    int bgYOffset = (m_regs.control1 & YSCROLL) - 3;
    int bgXOffset = (m_regs.control2 & XSCROLL);
    if (!m_bVBackground && (m_rasterLine == (m_scCurrent->backgroundStartLine + bgYOffset)) && (m_regs.control1 & DEN))
    {
        m_bVBackground = true;
        m_charRow = 0;
        m_charLine = 0;
    }
    if (m_bVBackground && !m_bHBackground && m_rasterLineCycle == m_scCurrent->backgroundStartCycle)
    {
        m_bHBackground = true;
        m_charCol = 0;

        for (int i = 0; i < 8; i++)
        {
            m_rc.screenSPixels[i] = m_regs.backgroundColor0;
            m_rc.screenSPixelsDat[i] = 0;     // 1 if collidable data
        }
    }

    if (m_bHBackground && m_charCol == 40)
        m_bHBackground = false;

    if (m_bVBackground && m_charRow == 25)
        m_bVBackground = false;

    if (m_bVBackground && m_bHBackground && m_charLine == 0)
        CacheLine();

    // update vic registers
    m_regs.control1 = (m_regs.control1 & 0x7f) | ((m_rasterLine >> 1) & 0x80);
    m_regs.rasterCounter = m_rasterLine & 0xff;
}

u8 Vic::ReadVicRegByte(u16 addr)
{
    u16 bound_addr = addr % sizeof(Registers);
    u8 val = ((u8*)&m_regs)[bound_addr];
    if (bound_addr == (u16)((u64) & (((Registers*)0)->spriteSpriteCollision)))
    {
        m_regs.spriteSpriteCollision = 0;
    }
    return val;
}

void Vic::WriteVicRegByte(u16 addr, u8 val)
{
    u16 bound_addr = addr & 63;
    ((u8*)&m_regs)[bound_addr] = val;

    if (bound_addr == (u16)((u64) & (((Registers*)0)->control1)))
    {
        m_interruptRasterline = (m_interruptRasterline & 0xff) | (((u16)val & 0x80) << 1);
    }
    else if (bound_addr == (u16)((u64) & (((Registers*)0)->rasterCounter)))
    {
        m_interruptRasterline = (m_interruptRasterline & 0x100) | val;
    }
    else if (bound_addr == (u16)((u64) & (((Registers*)0)->interruptRegister)))
    {
        // clear interrupt occurred
        m_interruptLatch = false;
        ((u8*)&m_regs)[bound_addr] = val | 0xf0;
    }
}
