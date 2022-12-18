#pragma once

#include "dockableWindow.h"
#include "uiItem_textBox.h"
#include "uiItem_enumButton.h"

class DockableWindow_MemoryDump : public DockableWindow
{
public:
    DockableWindow_MemoryDump(const string& title) : 
        m_renderedWidth(64), m_renderedHeight(64), m_memoryStart(0x800), m_memoryEnd(0x2000), m_dataCount(16), m_currentMode(MODE_Hex8),
        m_memMap(nullptr), m_memMapSize(0), m_memMapTexture(nullptr), m_zoomLevel(8),
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
    void FreeTexture();

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
    void DrawCharSet();
    void DrawBitmap();

    void CalcClampedMemoryRange(int &startMem, int &endMem, int &dataCount);
    void OnZoomChanged(int option);

    // for sprite memory dump
    int m_textureMode;
    u8* m_memMap;
    int m_memMapSize;
    SDL_Texture* m_memMapTexture;
    int m_zoomLevel;
    int m_textureWidth;
    int m_textureHeight;
    int m_visMemoryStart;
    int m_visMemoryEnd;
    int m_visDataCount;
    UIItem_EnumButton* m_zoomButton;
};


