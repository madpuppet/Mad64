#include "common.h"
#include "vic.h"

static u32 s_palette[16] = {
    0x000000, 0xffffff, 0xa1683c, 0x6abfc6,
    0x626262, 0x9f4e44, 0xc9d487, 0x887ecb,
    0x898989, 0xcb7e74, 0x9ae29b, 0x50439b,
    0xadadad, 0x6d5412, 0x5cab5e, 0xa057a3
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
    m_scPal.topBorderStartLine = 30;
    m_scPal.backgroundStartLine = 46;
    m_scPal.bottomBorderStartLine = 246;
    m_scPal.bottomBorderVBlankLine = 262;

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
    m_bBackground = false;

    //test pattern
    u32* ptr = (u32*)m_textureMem;
    for (int y = 0; y < m_scCurrent->screenHeight; y++)
    {
        for (int x = 0; x < m_scCurrent->screenWidth; x++)
        {
            int c = (x + y) & 1;
            *ptr++ = (c ? 0xff0f0f0f : 0xff000000);// ((*ptr >> 4) & 0x0f0f0f0f) + ;
        }
    }
}

void Vic::CacheLine()
{
    m_bBMM = m_regs.control1 & BMM;
    m_bECM = m_regs.control1 & ECM;
    m_bMCM = m_regs.control1 & MCM;

    // grab 40 bytes of character data
    u16 videoAddr = (((u16)(m_regs.memoryPointers & VideoMatrix)) << 2) + m_charRow * 40;
    u16 colorAddr = m_charRow * 40;
    for (int i = 0; i < 40; i++)
    {
        m_cachedChars[i] = ReadVicByte(videoAddr) | (ReadColorByte(colorAddr) << 8);
    }
}

void Vic::Step()
{
    u32* videoOut = (u32*)(m_textureMem + (m_rasterLineCycle + m_rasterLine * m_scCurrent->cyclesPerLine) * 8 * 4);
    if (!m_bHBlank && !m_bVBlank)
    {
        if (m_bVBorder || m_bHBorder)
        {
            u32 col = s_palette[m_regs.borderColor&15];
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
            *videoOut++ = col;
        }
        else
        {
            if (m_bBackground)
            {
                if (m_regs.control1 & BMM)
                {
                    // bitmap mode
                }
                else
                {
                    // text mode

                }
            }
        }
    }

    // next cycle...
    m_rasterLineCycle++;

    if (!m_textureDirty.h)
        m_textureDirty.h = 1;

    if (m_rasterLineCycle == m_scCurrent->leftBorderStartCycle)
        m_bHBlank = false;
    else if (m_rasterLineCycle == m_scCurrent->hblankStartCycles)
        m_bHBlank = true;
    else if (m_rasterLineCycle == m_scCurrent->backgroundStartCycle)
    {
        m_bHBorder = !m_bHBorder;
        m_bBackground = true;
    }
    else if (m_rasterLineCycle == m_scCurrent->rightBorderStartCycle)
    {
        m_bBackground = false;
        m_bHBorder = !m_bHBorder;
    }
    else if (m_rasterLineCycle == m_scCurrent->cyclesPerLine)
    {
        m_rasterLine++;
        m_rasterLineCycle = 0;
        m_textureDirty.h = min(m_textureDirty.h + 1, m_scCurrent->screenHeight-m_textureDirty.y);

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
            m_bBackground = true;
            m_charRow = 0;
            CacheLine();
        }
        else if (m_rasterLine == m_scCurrent->bottomBorderStartLine)
        {
            m_bVBorder = !m_bVBorder;
            m_bBackground = false;
        }
        else if (m_bBackground && ((m_rasterLine & 7) == 0))
        {
            CacheLine();
            m_charRow++;
        }

        if (m_rasterLine == m_scCurrent->screenHeight)
        {
            m_textureDirtyExtra = m_textureDirty;
            m_textureDirty.y = 0;
            m_textureDirty.h = 0;
            m_rasterLine = 0;
        }
    }
}

