#include "common.h"
#include "vic.h"

Vic::Vic()
{
    m_scPal.screenHeight = 312;
    m_scPal.screenWidth = 504;
    m_scPal.cyclesPerLine = 63;
    m_scPal.badLineCycles = 40;
    m_scPal.insideX = 14;
    m_scPal.insideY = 100;
    m_scPal.insideWidth = 320;
    m_scPal.insideHeight = 200;

    m_scNtsc.screenHeight = 312;
    m_scNtsc.screenWidth = 504;
    m_scNtsc.cyclesPerLine = 63;
    m_scNtsc.badLineCycles = 40;
    m_scNtsc.insideX = 14;
    m_scNtsc.insideY = 100;
    m_scNtsc.insideWidth = 320;
    m_scNtsc.insideHeight = 200;

    m_scCurrent = &m_scPal;

    m_texture = SDL_CreateTexture(gApp->GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_scCurrent->screenWidth, m_scCurrent->screenHeight);
    m_textureMem = (u8*)SDL_malloc(m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);

//    memset(m_textureMem, 0x80, m_scCurrent->screenWidth * m_scCurrent->screenHeight * 4);
    for (int x = 0; x < m_scCurrent->screenWidth; x++)
    {
        for (int y = 0; y < m_scCurrent->screenHeight; y++)
        {
            u8* ptr = &m_textureMem[x * 4 + y * m_scCurrent->screenWidth * 4];
            ptr[0] = x/2; ptr[1] = y/2; ptr[2] = 0; ptr[3] = 255;
        }
    }

    SDL_Rect area = { 0, 0, m_scCurrent->screenWidth, m_scCurrent->screenHeight };
    SDL_UpdateTexture(m_texture, &area, m_textureMem, m_scCurrent->screenWidth * 4);
}

void Vic::Render(int x, int y, int zoom)
{
    SDL_Rect dest = { x, y, m_scCurrent->screenWidth * zoom, m_scCurrent->screenHeight * zoom };
    SDL_RenderCopy(gApp->GetRenderer(), m_texture, nullptr, &dest);
}

void Vic::Reset()
{
}

void Vic::Step()
{
}

