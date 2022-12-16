#pragma once

#include "dockableWindow.h"

class DockableWindow_MemoryImage : public DockableWindow
{
public:
    DockableWindow_MemoryImage(const string& title);

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

    int m_zoomLevel = 2;

    void SetMemMap(u8* colorMap) { memcpy(m_memMap, colorMap, 65536); m_memMapDirty = true; }

protected:
    int GetScreenRenderWidth();
    int GetScreenRenderHeight();

    u8* m_memMap;
    SDL_Texture* m_memMapTexture;
    bool m_memMapDirty;
    float m_markerAnim;
};

