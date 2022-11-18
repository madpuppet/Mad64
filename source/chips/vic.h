#pragma once

class Vic
{
public:
    Vic();

    struct ScreenConfig
    {
        int screenHeight;           // total raster lines
        int screenWidth;            // total 'pixels' wide
        int cyclesPerLine;          // machine cycles per line
        int badLineCycles;          // machine cycles used up on badlines
        int leftBorderStartCycle;   // first cycle for border rendering for the line
        int backgroundStartCycle;   // first cycle for background rendering for the line
        int rightBorderStartCycle;  // first cycle where right border should start rendering
        int hblankStartCycles;      // start of hblank cycles after right border
        int topBorderStartLine;
        int backgroundStartLine;
        int bottomBorderStartLine;
        int bottomBorderVBlankLine;
    };
    void Reset();
    void Step();

    void Render(int x, int y, int zoom);

    enum Control1
    {
        YSCROLL = 7,
        RSEL = 8,
        DEN = 16,
        BMM = 32,
        ECM = 64,
        RST8 = 128
    };
    enum Control2
    {
        XSCROLL = 7,
        CSEL = 8,
        MCM = 16,
        RES = 32
    };
 
    enum MemoryPointers
    {
        CharacterBank = 0xe,
        VideoMatrix = 0xf0
    };

    struct Registers
    {
        u8 sprite0X;
        u8 sprite0Y;
        u8 sprite1X;
        u8 sprite1Y;
        u8 sprite2X;
        u8 sprite2Y;
        u8 sprite3X;
        u8 sprite3Y;
        u8 sprite4X;
        u8 sprite4Y;
        u8 sprite5X;
        u8 sprite5Y;
        u8 sprite6X;
        u8 sprite6Y;
        u8 sprite7X;
        u8 sprite7Y;
        u8 spriteXMSB;
        u8 control1;
        u8 rasterCounter;
        u8 lightPenX;
        u8 lightPenY;
        u8 spriteEnable;
        u8 control2;
        u8 spriteYEnlarge;
        u8 memoryPointers;
        u8 interruptRegister;
        u8 interruptEnable;
        u8 spritePriority;
        u8 spriteMulticolor;
        u8 spriteXEnlarge;
        u8 spriteSpriteCollision;
        u8 spriteDataCollision;
        u8 borderColor;
        u8 backgroundColor0;
        u8 backgroundColor1;
        u8 backgroundColor2;
        u8 backgroundColor3;
        u8 spriteMulticolor0;
        u8 spriteMulticolor1;
        u8 spriteColor0;
        u8 spriteColor1;
        u8 spriteColor2;
        u8 spriteColor3;
        u8 spriteColor4;
        u8 spriteColor5;
        u8 spriteColor6;
        u8 spriteColor7;
    };

    struct SpriteCache
    {
        u8 pixels[24 + 24 + 8];     // all pixels expanded, based on multicolor/size settings
        u32 data;
        u16 baseAddr;
        u16 x;
        u8 y;
        u8 color;
        u8 firstCycle;
        u8 cycle;
        u8 cycleCount;
        u8 rasterLine;
        u8 pri;                     // 1 if background should be above sprite
        bool startraster;           // started to render the sprite (have hit first rasterline)
        bool startcycle;            // started to render sprite for this rasterline
        bool sizeX;                 // locked in sizeX for this rasterline
        bool sizeY;                 // locked in sizeY for this rasterline
        bool extraLineLatch;        // if clear & sizeY, then we don't increment raster line... just set this latch instead
        bool multicolor;            // multicolor mode
    };
    SpriteCache m_spriteCache[8];
    void ResetSpriteFrame()
    {
        for (int i=0; i<8; i++)
        {
            m_spriteCache[i].startraster = false;
            m_spriteCache[i].startcycle = false;
        }
    }

    Registers& Regs() { return m_regs; }
    int CurrentRasterLine() { return m_rasterLine; }
    int CurrentRasterRow() { return m_rasterLineCycle; }
    int GetScreenWidth() { return m_scCurrent->screenWidth; }
    int GetScreenHeight() { return m_scCurrent->screenHeight; }

    // give access to 16k of memory
    void SetReadVicByte(const ReadByteHook& hook) { ReadVicByte = hook; }
    void SetReadColorByte(const ReadByteHook& hook) { ReadColorByte = hook; }

    // give access to 16k of memory
    u8 ReadVicRegByte(u16 addr) 
    {
        u16 bound_addr = addr % sizeof(Registers);
        return ((u8*)&m_regs)[bound_addr];
    }
    void WriteVicRegByte(u16 addr, u8 val)
    {
        u16 bound_addr = addr % sizeof(Registers);
        ((u8*)&m_regs)[bound_addr] = val;
    }

private:
    void RasterizeSprite(int i, u8 pixels[8], u8 pixelsPri[8], u8 pixelsDat[8]);

    ScreenConfig m_scPal;
    ScreenConfig m_scNtsc;
    ScreenConfig *m_scCurrent;

    // memory hooks
    ReadByteHook ReadVicByte;
    ReadByteHook ReadColorByte;

    SDL_Texture* m_texture;
    u8* m_textureMem;

    Registers m_regs;
    int m_rasterLine;
    int m_rasterLineCycle;
    int m_backgroundCycle;
    int m_charRow;      // which Row of character (0..25)
    int m_charLine;     // which line inside a single row (0..7)
    int m_charCol;      // which character this cycle (0..40)
    bool m_bHBlank;
    bool m_bVBlank;
    bool m_bHBorder;
    bool m_bVBorder;
    bool m_bBGVert;     // in vertical background area
    bool m_bBGHoriz;    // in horizontal background area

    // data fetched for this 8 line block
    u8 m_bBMM;
    u8 m_bECM;
    u8 m_bMCM;
    u16 m_cachedChars[40];     // text characters for this line [12 bits where high bits are from color memory]
    void CacheLine();

    SDL_Rect m_textureDirty;
    SDL_Rect m_textureDirtyExtra;
    void FlushTexture();
};


