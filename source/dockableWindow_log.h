#pragma once

#include "dockableWindow.h"

class DockableWindow_Log : public DockableWindow
{
public:
    DockableWindow_Log(const string& title) : DockableWindow(title) {}

    void Clear();
    void Log(const string& text, int colorIdx = 0, int lineNmbr = -1);

    int GetHeight();
    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

protected:
    struct LineItem
    {
        int y;
        string text;
        GraphicElement* ge;
        int lineNmbr;
        int colorIdx;
    };
    struct RenderedItem
    {
        SDL_Rect area;
        int lineItemIdx;
    };
    vector<LineItem> m_items;
    vector<RenderedItem> m_renderedItems;
};


