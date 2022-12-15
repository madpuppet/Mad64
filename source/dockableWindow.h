#pragma once

#include "uiItem.h"

class DockableWindow
{
public:
    DockableWindow(const string& title);
    virtual ~DockableWindow();

    void SetTitle(const string& str);
    void DrawTitle();
    void DrawContent();
    void OnRendererChange();
    void UpdateCursor(int x, int y);

    virtual void OnMouseButtonDown(int button, int x, int y);
    virtual void OnMouseButtonUp(int button, int x, int y);
    virtual void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);
    virtual void OnMouseWheel(int mouseX, int mouseY, int wheelX, int wheelY);

    virtual int GetContentHeight() = 0;
    virtual int GetContentWidth() = 0;
    virtual void SetDockedRect(const SDL_Rect& rect);

    void SetClipRect(const SDL_Rect& rect) { m_clipArea = rect; }

    bool IsDocked() { return m_isDocked; }

    void CreateIcons();
    void LayoutIcons();

    void Undock();
    void Dock();

    int GetID();
    SDL_Rect& GetArea() { return m_renderArea; }
    SDL_Rect& GetWindowArea() { return m_windowArea; }

    // hide or show the window if undocked
    void ShowWindow(bool enable);

    // mark content dirty = we'll update the scroll bars before the next render
    void SetContentDirty() { m_contentDirty = true; }

protected:
    virtual void CreateChildIcons() = 0;
    virtual void OnChildRendererChange() = 0;
    virtual void UpdateChildCursor(int x, int y) {}

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

    SDL_Rect m_renderArea;      // entire render area (including title line & scroll bars)
    SDL_Rect m_contentArea;     // render area of just the content
    SDL_Rect m_clipArea;        // area that is visible

    SDL_Rect m_dockedArea;      // entire docked render area
    SDL_Rect m_windowArea;      // entire undocked render area (window size)

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


