#pragma once

#include "dockableWindow.h"
#include "uiItem_textBox.h"
#include "uiItem_enumButton.h"

class DockableWindow_MemoryDump : public DockableWindow
{
public:
    DockableWindow_MemoryDump(const string& title) : 
        m_renderedWidth(64), m_renderedHeight(64), m_memoryStart(0), m_memoryEnd(2048), m_dataCount(16), m_currentMode(MODE_Hex8),
        m_memMap(nullptr), m_memMapTexture(nullptr),
        DockableWindow(title) {}

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

protected:
    void OnRangeChange(const string& text);
    void OnModeChange(int option);
    void RecreateTexture();

    void ActivateStartBox();
    void ActivateEndBox();

    int m_renderedWidth;
    int m_renderedHeight;
    UIItem_TextBox* m_memoryStartBox;
    UIItem_TextBox* m_memoryEndBox;
    UIItem_TextBox* m_dataCountBox;
    UIItem_EnumButton* m_modeButton;
    u32 m_memoryStart;
    u32 m_memoryEnd;
    int m_dataCount;

    enum Mode
    {
        MODE_Hex8,
        MODE_Hex16,
        MODE_Bin8,
        MODE_Bin16,
        MODE_Petsci,
        MODE_Sprite,
        MODE_SpriteMC,
        MODE_CharSet,
        MODE_CharSetMC,
        MODE_Bitmap,
        MODE_BitmapMC
    } m_currentMode;

    void DrawHex8();
    void DrawHex16();
    void DrawBin8();
    void DrawBin16();
    void DrawPetsci();
    void DrawSprite();
    void DrawSpriteMC();
    void DrawCharSet();
    void DrawCharSetMC();
    void DrawBitmap();
    void DrawBitmapMC();

    // for sprite/bitmap modes
    u8* m_memMap;
    SDL_Texture* m_memMapTexture;

};


