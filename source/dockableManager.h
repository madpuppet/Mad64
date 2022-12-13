#pragma once

#undef FindWindow

// Manage a list of dockable windows
class DockableManager
{
public:
    DockableManager();
    ~DockableManager();

    void AddWindow(class DockableWindow* window, const char* iconText, bool enabled, bool docked);
    bool IsWindowEnabled(DockableWindow* window);
    class DockableWindow* FindWindowByID(int id);

    void SetRect(const SDL_Rect& rect);
    SDL_Rect& GetRect() { return m_area; }

    void Draw();

    bool OnMouseDown(SDL_Event* e);
    bool OnMouseUp(SDL_Event* e);
    bool OnMouseMotion(SDL_Event* e);
    bool OnMouseWheel(int windowID, int mouseX, int mouseY, int wheelX, int wheelY);

protected:
    void OnMouseMotionCaptured(bool lostCapture, int x, int y);

    int m_mainWindowID;
    SDL_Rect m_area;
    int m_scroll;

    struct DockableWindowItem
    {
        DockableWindowItem() : m_geTitle(0), m_window(0) {}
        ~DockableWindowItem() {}

        bool m_enabled;
        GraphicElement* m_geTitle;
        DockableWindow* m_window;
    };
    vector<DockableWindowItem> m_windows;

    enum GrabMode
    {
        Grab_None,
        Grab_HScroll,
        Grab_VScroll
    } m_grabMode;

    int GetDockedContentWidth();
    int GetDockedContentHeight();

    // vscroll/hscroll
    void ClampTargetVertScroll();
    void ClampTargetHorizScroll();
    int m_vertScroll;
    int m_horizScroll;
    float m_targetVertScroll;
    float m_targetHorizScroll;
    SDL_Point m_dragMouseGrab;

    // scroll bar areas
    SDL_Rect m_contentArea;
    int m_renderedContentWidth;
    int m_renderedContentHeight;

    SDL_Rect m_vertBackArea;
    SDL_Rect m_vertBarFullArea;
    SDL_Rect m_vertBarArea;
    SDL_Rect m_horizBackArea;
    SDL_Rect m_horizBarFullArea;
    SDL_Rect m_horizBarArea;
    void CalcScrollBars();
};

