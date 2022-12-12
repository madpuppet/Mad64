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

    void RecreateTexture(SDL_Renderer* r);
    SDL_Texture* FlushTexture();

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
     enum Interrupts
     {
         IRST = 0x01,       // raster interrupt
         IMBC = 0x02,       // sprite to data collision
         IMMC = 0x04,       // sprite to sprite collision
         ILP = 0x08,
         IRQ = 0x80
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

        u8 pad[17];
    };

    struct SpriteCache
    {
        u8 pixels[24 + 24 + 8];     // all pixels expanded, based on multicolor/size settings
        u8 collisionMask;           // collision mask for this cycle
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

    Registers& Regs() { return m_regs; }
    int CurrentRasterLine() { return m_rasterLine; }
    int CurrentRasterRow() { return m_rasterLineCycle; }
    int GetScreenWidth() { return m_scCurrent->screenWidth; }
    int GetScreenHeight() { return m_scCurrent->screenHeight; }

    // give access to 16k of memory
    void SetReadVicByte(const ReadByteHook& hook) { ReadVicByte = hook; }

    // set the callback for triggering a cpu interrupt
    void SetTriggerInterrupt(const InterruptHook& hook) { TriggerInterrupt = hook; }

    // give access to 16k of memory
    u8 ReadVicRegByte(u16 addr);
    void WriteVicRegByte(u16 addr, u8 val);

    u8 ReadVicColorByte(u16 addr)
    {
        return m_colorMem[addr];
    }
    void WriteVicColorByte(u16 addr, u8 val)
    {
        m_colorMem[addr] = val & 0xf;
    }

private:
    struct RasterizeCache
    {
        SpriteCache spriteCache[8];       // cached sprite info for rasterizing
        u8 spritePixels[8];         // final sprite pixel color for this cycle
        u8 spritePixelsPri[8];      // final priority for each sprite pixel (1 == show background if it has data)
        u8 spritePixelsDat[8];      // final sprite pixel data or no data mask
        u8 screenPixels[8];         // background pixel color
        u8 screenPixelsDat[8];      // background pixel has data  (else its just background color)
        u8 screenSPixels[8];        // shifted background pixel color
        u8 screenSPixelsDat[8];     // shifted background pixel has data  (else its just background color)
        u8 screenPixelsMask;        // bits for every background pixel that has data for quick sprite->data collision
    } m_rc;

    void RasterizeSprites();
    void RasterizeSprite(int i);
    void ResetSpriteFrame();

    void RasterizeScreen();
    void RasterizeScreen_NormalTextMode();
    void RasterizeScreen_MulticolorTextMode();
    void RasterizeScreen_ExtendedColorTextMode();
    void RasterizeScreen_NormalBitmapMode();
    void RasterizeScreen_MulticolorBitmapMode();
    void RasterizeScreen_InvalidMode();

    ScreenConfig m_scPal;
    ScreenConfig m_scNtsc;
    ScreenConfig *m_scCurrent;

    // memory hooks
    ReadByteHook ReadVicByte;        // read a byte from the current vic 16k memory bank
    InterruptHook TriggerInterrupt;  // trigger an interrupt

    SDL_Texture* m_texture;
    u8* m_textureMem;

    Registers m_regs;
    int m_rasterLine;
    int m_rasterLineCycle;
    int m_backgroundCycle;
    int m_charRow;           // which Row of character (0..25)
    int m_charLine;          // which line inside a single row (0..7)
    int m_charCol;           // which character this cycle (0..40)

    bool m_bVBlank;          // vertical blanking gap
    bool m_bHBlank;          // horizontal blanking gap
    bool m_bVBorder;         // vertical border enable
    bool m_bHBorder;         // horizontal border enable
    bool m_bVBackground;     // in vertical background area
    bool m_bHBackground;     // in horizontal background area

    // data fetched for this 8 line block
    u8 m_bBMM;
    u8 m_bECM;
    u8 m_bMCM;
    u16 m_cachedChars[40];     // text characters for this line [12 bits where high bits are from color memory]
    void CacheLine();

    // interrupt rasterline
    u16 m_interruptRasterline;
    bool m_interruptLatch;

    // vic holds the color memory
    u8 m_colorMem[1024];

    SDL_Rect m_textureDirty;
    SDL_Rect m_textureDirtyExtra;
};


