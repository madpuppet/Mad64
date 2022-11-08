#pragma once

class Vic
{
public:
    Vic();

    struct ScreenConfig
    {
        int screenHeight;       // total raster lines
        int screenWidth;        // total 'pixels' wide
        int cyclesPerLine;      // machine cycles per line
        int badLineCycles;      // machine cycles used up on badlines
        int insideX;            // start X of background
        int insideY;            // start Y of background
        int insideWidth;        // width of background
        int insideHeight;       // height of background
    };
    void Reset();
    void Step();

    void Render(int x, int y, int zoom);

private:
    ScreenConfig m_scPal;
    ScreenConfig m_scNtsc;
    ScreenConfig *m_scCurrent;

    SDL_Texture* m_texture;
    u8* m_textureMem;
};


