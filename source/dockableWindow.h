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

    virtual int GetHeight() = 0;
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

protected:
    virtual void CreateChildIcons() = 0;
    virtual void OnChildRendererChange() = 0;

    void OnResize();
    SDL_Renderer* GetRenderer();

    virtual void DrawChild() = 0;

    bool m_isDocked;
    bool m_dragging;
    bool m_resizing;
    SDL_Point m_dragMouseGrab;

    SDL_Rect m_renderArea;
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
};


