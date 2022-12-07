#include "common.h"
#include "DockableManager.h"

#define EXPERIMENTAL

DockableWindow::~DockableWindow()
{
}

void DockableWindow::OnMouseButtonDown(int button, int x, int y)
{
}

void DockableWindow::OnMouseButtonUp(int button, int x, int y)
{
}

void DockableWindow::OnMouseMotion(int x, int y) 
{
}

void DockableWindow::OnMouseWheel(int x, int y)
{
}


void DockableWindow::Draw()
{
    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    SDL_RenderSetClipRect(r, NULL);
    SDL_SetRenderDrawColor(r, settings->backColor.r, settings->backColor.g, settings->backColor.b, 255);
    SDL_RenderFillRect(r, NULL);

    SDL_Rect titleRect = { m_renderArea.x, m_renderArea.y, m_renderArea.w, settings->lineHeight };
    SDL_SetRenderDrawColor(r, 32, 64, 128, 255);
    SDL_RenderFillRect(r, &titleRect);

    if (!m_geTitle)
    {
        m_geTitle = GraphicElement::CreateFromText(r, gApp->GetFont(), m_title.c_str(), settings->helpGroupColor, m_renderArea.x + settings->textXMargin, m_renderArea.y + settings->textYMargin);
    }
    m_geTitle->Render(r);

    DrawChild();

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_Color col = { 255,255,255,255 };
    auto ge = GraphicElement::CreateFromText(r, gApp->GetFont(), "ABCabcdef1232336", col, 50, 50);
    ge->Render(r);
    delete ge;


    SDL_RenderPresent(r);
}

void DWLog::DrawChild()
{
    auto r = GetRenderer();

    int y = 20;
    for (auto& it : m_items)
    {
        it.ge->RenderAt(r, 50, 50);
        y += it.ge->GetRect().h;
    }
}

SDL_Renderer* DockableWindow::GetRenderer()
{
    return m_renderer ? m_renderer : gApp->GetRenderer();
}

void DockableWindow::SetTitle(const string& str)
{
    m_title = str;
    delete m_geTitle;
    m_geTitle = nullptr;
}

void DockableWindow::Undock()
{
    m_window = SDL_CreateWindow("LogWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowArea.w, m_windowArea.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_window == NULL)
    {
        Log("ERROR: Cannot undock window");
        return;
    }
    SDL_SetWindowPosition(m_window, m_windowArea.x, m_windowArea.y);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    m_isDocked = true;

    m_renderArea.x = 0;
    m_renderArea.y = 0;
    m_renderArea.w = m_windowArea.w;
    m_renderArea.h = m_windowArea.h;
}

int DockableWindow::GetID()
{
    if (m_window)
        return SDL_GetWindowID(m_window);
    else
        return -1;
}


void DockableWindow::Dock()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_window = 0;
    m_renderer = 0;
    m_isDocked = false;

    m_renderArea = m_dockedArea;
}

DockableManager::DockableManager()
{
#if defined(EXPERIMENTAL)
    DockableWindowItem item;
    SDL_Color col = { 255,255,255 };
    DWLog* dwLog = new DWLog("Compiler Log");
    item.m_geTitle = GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), "COM", col, 0, 0);
    item.m_window = dwLog;
    m_windows.push_back(item);

    dwLog->Undock();
    dwLog->Log("Test line", 0);

#endif
}

DockableWindow* DockableManager::FindWindow(int id)
{
    for (auto& it : m_windows)
    {
        if (id == it.m_window->GetID())
            return it.m_window;
    }
    return nullptr;
}

void DockableManager::OnMouseDown(SDL_Event* e)
{
    auto window = FindWindow(e->button.windowID);
    if (window)
    {
        window->OnMouseButtonDown(e->button.button, e->button.x, e->button.y);
    }
}
void DockableManager::OnMouseUp(SDL_Event* e)
{
    auto window = FindWindow(e->button.windowID);
    if (window)
    {
        window->OnMouseButtonUp(e->button.button, e->button.x, e->button.y);
    }
}
void DockableManager::OnMouseMotion(SDL_Event* e)
{
    auto window = FindWindow(e->button.windowID);
    if (window)
    {
        window->OnMouseMotion(e->motion.x, e->motion.y);
    }
}
void DockableManager::OnMouseWheel(SDL_Event* e)
{
    auto window = FindWindow(e->button.windowID);
    if (window)
    {
        window->OnMouseWheel(e->wheel.x, e->wheel.y);
    }
}

void DWLog::Log(const string& text, int color)
{
    SDL_Color col = { 255,255,255,255 };
    LineItem item;
    item.text = text;
    item.ge = GraphicElement::CreateFromText(GetRenderer(), gApp->GetFont(), text.c_str(), col, 0, 0);
    m_items.push_back(item);
}

DockableManager::~DockableManager()
{
#if defined(EXPERIMENTAL)
    for (auto& it : m_windows)
    {
        delete it.m_geTitle;
        delete it.m_window;
    }
#endif
}

void DockableManager::SetRect(const SDL_Rect& rect)
{
    // layout all the docked windows...
    m_area = rect;
    SDL_Rect area = rect;
    for (auto& it : m_windows)
    {
        if (it.m_window->IsDocked())
        {
            area.h = it.m_window->GetHeight();
            it.m_window->SetRect(area);
            area.y += area.h;
        }
    }
}

void DockableManager::Draw()
{
#if defined(EXPERIMENTAL)
    auto r = gApp->GetRenderer();
    for (auto& it : m_windows)
    {
        it.m_window->Draw();
    }
#endif
}


