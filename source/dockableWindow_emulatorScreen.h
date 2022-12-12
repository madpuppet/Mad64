#pragma once

#include "dockableWindow.h"

class DockableWindow_EmulatorScreen : public DockableWindow
{
public:
    DockableWindow_EmulatorScreen(const string& title) : DockableWindow(title) {}

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

    int m_zoomLevel = 2;
};