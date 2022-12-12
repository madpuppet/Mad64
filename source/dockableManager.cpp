#include "common.h"
#include "dockableManager.h"
#include "dockableWindow.h"
#include "dockableWindow_log.h"

DockableManager::DockableManager()
{
    m_mainWindowID = SDL_GetWindowID(gApp->GetWindow());
}

void DockableManager::AddWindow(class DockableWindow* window, const char* iconText, bool enabled, bool docked)
{
    DockableWindowItem item;

    SDL_Color col = { 255,255,255 };
    item.m_geTitle = GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), iconText, col, 0, 0);
    item.m_window = window;
    item.m_enabled = enabled;
    m_windows.push_back(item);

    window->CreateIcons();
    if (!docked)
        window->Undock();
    else
        window->OnRendererChange();
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

bool DockableManager::IsWindowEnabled(DockableWindow* window)
{
    for (auto& it : m_windows)
    {
        if (it.m_window == window)
            return it.m_enabled;
    }
    return false;
}

bool DockableManager::OnMouseDown(SDL_Event* e)
{
    auto settings = gApp->GetSettings();
    if (e->button.windowID == m_mainWindowID)
    {
        // clicked on the main window - check its on the dockable area
        if (Contains(m_area, e->button.x, e->button.y))
        {
            // check docked windows
            for (auto& win : m_windows)
            {
                if (Contains(win.m_geTitle->GetRect(), e->button.x, e->button.y))
                {
                    win.m_enabled = !win.m_enabled;
                    if (!win.m_window->IsDocked())
                        win.m_window->ShowWindow(win.m_enabled);
                    return true;
                }
                else if (win.m_enabled && win.m_window->IsDocked() && Contains(win.m_window->GetArea(), e->button.x, e->button.y))
                {
                    win.m_window->OnMouseButtonDown(e->button.button, e->button.x, e->button.y);
                    return true;
                }
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindow(e->button.windowID);
        if (window)
        {
            window->OnMouseButtonDown(e->button.button, e->button.x, e->button.y);
            return true;
        }
    }
    return false;
}


bool DockableManager::OnMouseUp(SDL_Event* e)
{
    auto settings = gApp->GetSettings();
    if (e->button.windowID == m_mainWindowID)
    {
        // clicked on the main window - check its on the dockable area
        if (Contains(m_area, e->button.x, e->button.y))
        {
            // check docked windows
            for (auto& win : m_windows)
            {
                if (Contains(win.m_window->GetArea(), e->button.x, e->button.y))
                {
                    win.m_window->OnMouseButtonUp(e->button.button, e->button.x, e->button.y);
                    return true;
                }
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindow(e->button.windowID);
        if (window)
        {
            window->OnMouseButtonUp(e->button.button, e->button.x, e->button.y);
            return true;
        }
    }
    return false;
}
bool DockableManager::OnMouseMotion(SDL_Event* e)
{
    auto settings = gApp->GetSettings();
    if (e->button.windowID == m_mainWindowID)
    {
        // clicked on the main window - check its on the dockable area
        if (Contains(m_area, e->button.x, e->button.y))
        {
            // check docked windows
            for (auto& win : m_windows)
            {
                if (Contains(win.m_window->GetArea(), e->button.x, e->button.y))
                {
                    win.m_window->OnMouseMotion(e->motion.x, e->motion.y, e->motion.xrel, e->motion.yrel);
                    return true;
                }
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindow(e->button.windowID);
        if (window)
        {
            window->OnMouseMotion(e->motion.x, e->motion.y, e->motion.xrel, e->motion.yrel);
            return true;
        }
    }
    return false;
}
bool DockableManager::OnMouseWheel(SDL_Event* e)
{
    auto settings = gApp->GetSettings();
    if (e->button.windowID == m_mainWindowID)
    {
        // clicked on the main window - check its on the dockable area
        if (Contains(m_area, e->button.x, e->button.y))
        {
            // check docked windows
            for (auto& win : m_windows)
            {
                if (Contains(win.m_window->GetArea(), e->button.x, e->button.y))
                {
                    win.m_window->OnMouseWheel(e->wheel.x, e->wheel.y);
                    return true;
                }
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindow(e->button.windowID);
        if (window)
        {
            window->OnMouseWheel(e->wheel.x, e->wheel.y);
            return true;
        }
    }
    return false;
}

DockableManager::~DockableManager()
{
    for (auto& it : m_windows)
    {
        delete it.m_geTitle;
        delete it.m_window;
    }
}

void DockableManager::SetRect(const SDL_Rect& rect)
{
    m_area = rect;
}

void DockableManager::Draw()
{
    auto r = gApp->GetRenderer();
    auto settings = gApp->GetSettings();

    SDL_SetRenderDrawColor(r, 64, 64, 64, 255);
    SDL_Rect titleRect = { m_area.x, m_area.y, m_area.w, settings->lineHeight };
    SDL_RenderFillRect(r, &titleRect);

    int titleX = m_area.x + settings->textXMargin;
    for (auto& win : m_windows)
    {
        win.m_geTitle->SetPos(titleX, m_area.y + settings->textYMargin);
        if (win.m_enabled)
        {
            SDL_SetTextureColorMod(win.m_geTitle->GetTexture(), 255, 255, 255);
        }
        else
        {
            SDL_SetTextureColorMod(win.m_geTitle->GetTexture(), 128, 128, 128);
        }
        win.m_geTitle->Render(r);
        titleX += win.m_geTitle->GetRect().w + settings->textXMargin * 2;
    }

    SDL_Rect area = m_area;
    area.y += settings->lineHeight;
    for (auto &win : m_windows)
    {
        if (win.m_enabled && win.m_window->IsDocked())
        {
            area.h = win.m_window->GetContentHeight();
            win.m_window->SetRect(area);
            win.m_window->Draw();
            area.y += area.h;
        }
    }

    for (auto& win : m_windows)
    {
        if (win.m_enabled && !win.m_window->IsDocked())
        {
            win.m_window->Draw();
        }
    }
}

