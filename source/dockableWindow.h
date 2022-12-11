#pragma once

#include "uiItem.h"

class DockableWindow
{
public:
    DockableWindow(const string& title);
    virtual ~DockableWindow();

    void SetTitle(const string& str);
    void Draw();
    void OnRendererChange();

    virtual void OnMouseButtonDown(int button, int x, int y);
    virtual void OnMouseButtonUp(int button, int x, int y);
    virtual void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);
    virtual void OnMouseWheel(int x, int y);

    virtual int GetContentHeight() = 0;
    virtual int GetContentWidth() = 0;
    virtual void SetRect(const SDL_Rect& rect);

    bool IsDocked() { return m_isDocked; }

    void CreateIcons();
    void LayoutIcons();

    void Undock();
    void Dock();

    int GetID();
    SDL_Rect& GetArea() { return m_renderArea; }

    // hide or show the window if undocked
    void ShowWindow(bool enable);

    // mark content dirty = we'll update the scroll bars before the next render
    void SetContentDirty() { m_contentDirty = true; }

protected:
    virtual void CreateChildIcons() = 0;
    virtual void OnChildRendererChange() = 0;

    void OnResize();
    SDL_Renderer* GetRenderer();
    void UpdateContentArea();

    virtual void DrawChild() = 0;

    enum GrabMode
    {
        None,
        Dragging,
        ResizeLeft,
        ResizeRight,
        VScrollbar,
        HScrollbar
    } m_grabMode;

    bool m_isDocked;
    bool m_contentDirty;

    SDL_Point m_dragMouseGrab;

    SDL_Rect m_renderArea;
    SDL_Rect m_contentArea;

    SDL_Rect m_dockedArea;
    SDL_Rect m_windowArea;

    string m_title;
    GraphicElement* m_geTitle;

    vector <UIItem*> m_titleIconsRight;
    vector <UIItem*> m_titleIconsLeft;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    void GenerateTitleGE();

    void OnDockPress();

    // vscroll/hscroll
    void ClampTargetVertScroll();
    void ClampTargetHorizScroll();
    bool CalcVertScrollBar(int& start, int& end);
    bool CalcHorizScrollBar(int& start, int& end);
    int m_vertScroll;
    int m_horizScroll;
    float m_targetVertScroll;
    float m_targetHorizScroll;

    // scroll bar areas
    SDL_Rect m_vertBackArea;
    SDL_Rect m_vertBarFullArea;
    SDL_Rect m_vertBarArea;
    SDL_Rect m_horizBackArea;
    SDL_Rect m_horizBarFullArea;
    SDL_Rect m_horizBarArea;
    void CalcScrollBars();
};

