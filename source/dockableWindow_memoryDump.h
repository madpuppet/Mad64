#pragma once

#include "dockableWindow.h"
#include "uiItem_textBox.h"

class DockableWindow_MemoryDump : public DockableWindow
{
public:
    DockableWindow_MemoryDump(const string& title) : m_renderedWidth(64), m_renderedHeight(64), m_memoryStart(0), m_memoryEnd(2048), DockableWindow(title) {}

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

protected:
    void OnRangeChange(const string& text);

    int m_renderedWidth;
    int m_renderedHeight;
    UIItem_TextBox* m_memoryStartBox;
    UIItem_TextBox* m_memoryEndBox;
    u32 m_memoryStart;
    u32 m_memoryEnd;
};


