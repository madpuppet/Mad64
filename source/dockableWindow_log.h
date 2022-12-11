#pragma once

#include "dockableWindow.h"

class DockableWindow_Log : public DockableWindow
{
public:
    DockableWindow_Log(const string& title) : m_renderedWidth(64), DockableWindow(title) {}

    void Clear();
    void LogText(const string& text, int lineNmbr, int colorIdx, int addr = -1);
    void LogTextArray(const char** textArray, int col);

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

protected:
    struct LineItem
    {
        int y;
        string text;
        GraphicElement* ge;
        int lineNmbr;
        int colorIdx;
        int addr;
    };
    struct RenderedItem
    {
        SDL_Rect area;
        int lineNmbr;
    };
    vector<LineItem> m_items;
    vector<RenderedItem> m_renderedItems;
    int m_renderedWidth;
};


