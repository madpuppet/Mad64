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
    bool OnMouseWheel(SDL_Event* e);

protected:

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
};

