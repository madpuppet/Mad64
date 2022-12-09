#pragma once

// Manage a list of dockable windows

typedef DMFastDelegate::FastDelegate1<class DockableWindow *> OnWindowDockChanged;

class DockableWindowTitleIcon
{
public:
    virtual void Draw();

    void SetArea(const SDL_Rect& rect);

protected:
    SDL_Rect m_area;
};

class DockableWindow
{
public:
    DockableWindow(const string& title) : m_title(title), m_isDocked(true), m_dragging(false), m_geTitle(nullptr), m_windowArea({ 100,100,640,480 }), m_dockedArea({ 0,0,640,480 }) {}
    virtual ~DockableWindow();

    void SetTitle(const string& str);
    void Draw();

    virtual void OnMouseButtonDown(int button, int x, int y);
    virtual void OnMouseButtonUp(int button, int x, int y);
    virtual void OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel);
    virtual void OnMouseWheel(int x, int y);

    virtual int GetHeight() = 0;
    virtual void SetRect(const SDL_Rect& rect) { m_dockedArea = rect; }

    bool IsDocked() { return m_isDocked; }

    void Undock();
    void Dock();

    int GetID();

protected:
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

    vector <DockableWindowTitleIcon*> m_titleIcons;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
};

class DWLog : public DockableWindow
{
public:
    DWLog(const string& title) : DockableWindow(title) {}

    void Log(const string &text, int colorIdx);

    virtual int GetHeight() { return 100; }
    virtual void DrawChild();

protected:
    struct LineItem
    {
        string text;
        GraphicElement* ge;
    };
    vector<LineItem> m_items;
};

class DockableManager
{
public:
    DockableManager();
    ~DockableManager();

    void SetRect(const SDL_Rect& rect);
    void Draw();

    void OnMouseDown(SDL_Event* e);
    void OnMouseUp(SDL_Event* e);
    void OnMouseMotion(SDL_Event* e);
    void OnMouseWheel(SDL_Event* e);

protected:
    DockableWindow* FindWindow(int id);

    SDL_Rect m_area;

    struct DockableWindowItem
    {
        DockableWindowItem() : m_id(0), m_geTitle(0), m_window(0) {}
        ~DockableWindowItem() {}

        int m_id;
        GraphicElement* m_geTitle;
        DockableWindow* m_window;
    };
    vector<DockableWindowItem> m_windows;
};

