#pragma once

#pragma once

#include "dockableWindow.h"
#include "uiItem_textBox.h"

class DockableWindow_SearchAndReplace : public DockableWindow
{
public:
    DockableWindow_SearchAndReplace(const string& title) : m_searchBox(0), m_replaceBox(0), m_renderedWidth(64), DockableWindow(title) {}

    int GetContentHeight();
    int GetContentWidth();

    void DrawChild();
    void OnChildRendererChange();
    void CreateChildIcons();

    void OnMouseButtonDown(int button, int x, int y);
    void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);

    void SetSearchText(const string& text);
    const string& GetSearchText() { return m_searchBox->GetText(); }

    void SetReplaceText(const string& text);
    const string& GetReplaceText() { return m_replaceBox->GetText(); }

    void SelectSearchBox();
    void SelectReplaceBox();

    void OnSearchStringEnter(const string& text);
    void OnSearchStringChange(const string& text);
    void OnReplaceStringEnter(const string& text);
    void OnReplaceStringChange(const string& text);

    vector<int>& GetFoundLines() { return m_searchFoundLines; }
    bool ContainsLine(int lineIdx);

protected:
    void Clear();
    void LogText(const string& text, int lineNmbr, int colorIdx, int addr = -1);
    void LogFoundItems();
    void ReSearch(const string& text, int minSize);

    void ActivateSearch();
    void ActivateReplace();

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

    UIItem_TextBox* m_searchBox;
    UIItem_TextBox* m_replaceBox;

    vector<int> m_searchFoundLines;
};


