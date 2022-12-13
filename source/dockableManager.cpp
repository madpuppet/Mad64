#include "common.h"
#include "dockableManager.h"
#include "dockableWindow.h"
#include "dockableWindow_log.h"

DockableManager::DockableManager()
{
    m_mainWindowID = SDL_GetWindowID(gApp->GetWindow());

    m_renderedContentWidth = 128;
    m_renderedContentHeight = 128;

    m_vertScroll = 0;
    m_horizScroll = 0;
    m_targetVertScroll = 0;
    m_targetHorizScroll = 0;
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

DockableWindow* DockableManager::FindWindowByID(int id)
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
        if (e->button.y < m_area.y + settings->lineHeight)
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
            }
        }
        else if (Contains(m_contentArea, e->button.x, e->button.y))
        {
            // check docked windows
            for (auto& win : m_windows)
            {
                if (win.m_enabled && win.m_window->IsDocked() && Contains(win.m_window->GetArea(), e->button.x, e->button.y))
                {
                    win.m_window->OnMouseButtonDown(e->button.button, e->button.x, e->button.y);
                    return true;
                }
            }
        }
        if (Contains(m_vertBarFullArea, e->button.x, e->button.y))
        {
            if (Contains(m_vertBarArea, e->button.x, e->button.y))
            {
                m_grabMode = Grab_VScroll;
                m_dragMouseGrab = { e->button.x - m_vertBarArea.x, e->button.y - m_vertBarArea.y };
                gApp->SetCaptureMouseMotion(DELEGATE(DockableManager::OnMouseMotionCaptured));
                return true;
            }
            else if (e->button.y < m_vertBarArea.y)
            {
                // page up
                m_targetVertScroll -= m_contentArea.h;
                ClampTargetVertScroll();
                return true;
            }
            else if (e->button.y > m_vertBarArea.y + m_vertBarArea.h)
            {
                // page down
                m_targetVertScroll += m_contentArea.h;
                ClampTargetVertScroll();
                return true;
            }
        }
        if (Contains(m_horizBarFullArea, e->button.x, e->button.y))
        {
            if (Contains(m_horizBarArea, e->button.x, e->button.y))
            {
                m_grabMode = Grab_HScroll;
                m_dragMouseGrab = { e->button.x - m_horizBarArea.x, e->button.y - m_horizBarArea.y };
                gApp->SetCaptureMouseMotion(DELEGATE(DockableManager::OnMouseMotionCaptured));
                return true;
            }
            else if (e->button.x < m_horizBarArea.x)
            {
                // page left
                m_targetHorizScroll -= m_contentArea.w;
                ClampTargetHorizScroll();
                return true;
            }
            else if (e->button.x > m_horizBarArea.x + m_horizBarArea.w)
            {
                // page right
                m_targetHorizScroll += m_contentArea.w;
                ClampTargetHorizScroll();
                return true;
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindowByID(e->button.windowID);
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
    m_grabMode = Grab_None;

    // check docked windows
    for (auto& win : m_windows)
    {
        win.m_window->OnMouseButtonUp(e->button.button, e->button.x, e->button.y);
    }
    return false;
}

bool DockableManager::OnMouseMotion(SDL_Event* e)
{
    auto settings = gApp->GetSettings();
    switch (m_grabMode)
    {
        case Grab_VScroll:
            {
                int newBarStart = e->motion.y - m_dragMouseGrab.y;
                m_targetVertScroll = (float)(newBarStart - m_vertBarFullArea.y) / (float)m_vertBarFullArea.h * (float)GetDockedContentHeight();
                ClampTargetVertScroll();
            }
            return true;
        case Grab_HScroll:
            {
                int newBarStart = e->motion.x - m_dragMouseGrab.x;
                m_targetHorizScroll = (float)(newBarStart - m_horizBarFullArea.x) / (float)m_horizBarFullArea.w * (float)GetDockedContentWidth();
                ClampTargetHorizScroll();
            }
            return true;
    }

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
        auto window = FindWindowByID(e->button.windowID);
        if (window)
        {
            window->OnMouseMotion(e->motion.x, e->motion.y, e->motion.xrel, e->motion.yrel);
            return true;
        }
    }
    return false;
}
bool DockableManager::OnMouseWheel(int windowID, int mouseX, int mouseY, int wheelX, int wheelY)
{
    auto settings = gApp->GetSettings();
    if (windowID == m_mainWindowID)
    {
        // clicked on the main window - check its on the dockable area
        if (Contains(m_area, mouseX, mouseY))
        {
            m_targetVertScroll -= wheelY * 50;
            ClampTargetVertScroll();

            // check docked windows
            for (auto& win : m_windows)
            {
                if (Contains(win.m_window->GetArea(), mouseX, mouseY))
                {
                    win.m_window->OnMouseWheel(mouseX, mouseY, wheelX, wheelY);
                    return true;
                }
            }
        }
    }
    else
    {
        // check undocked windows
        auto window = FindWindowByID(windowID);
        if (window)
        {
            window->OnMouseWheel(mouseX, mouseY, wheelX, wheelY);
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
    auto settings = gApp->GetSettings();

    m_area = rect;
    m_contentArea = { rect.x, rect.y + settings->lineHeight, rect.w - settings->lineHeight, rect.h - settings->lineHeight * 2 };
}

void DockableManager::Draw()
{
    ClampTargetVertScroll();
    ClampTargetHorizScroll();

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

    m_renderedContentWidth = 64;
    SDL_RenderSetClipRect(r, &m_contentArea);
    SDL_Rect area = { m_contentArea.x - m_horizScroll, m_contentArea.y - m_vertScroll, m_contentArea.w, m_contentArea.h };
    for (auto &win : m_windows)
    {
        if (win.m_enabled && win.m_window->IsDocked())
        {
            area.h = win.m_window->GetContentHeight();
            win.m_window->SetRect(area);
            if (((area.y + area.h) > m_contentArea.y) && (area.y < (m_contentArea.y + m_contentArea.h)))
            {
                win.m_window->Draw();
                m_renderedContentWidth = SDL_max(m_renderedContentWidth, win.m_window->GetContentWidth());
            }
            area.y += area.h;
        }
    }
    SDL_RenderSetClipRect(r, nullptr);
    m_renderedContentHeight = area.y - (m_contentArea.y - m_vertScroll);

    CalcScrollBars();

    SDL_SetRenderDrawColor(r, 32, 32, 64, 255);
    SDL_RenderFillRect(r, &m_vertBackArea);
    SDL_RenderFillRect(r, &m_horizBackArea);

    SDL_SetRenderDrawColor(r, 128, 128, 196, 255);
    SDL_RenderFillRect(r, &m_vertBarArea);
    SDL_RenderFillRect(r, &m_horizBarArea);

    for (auto& win : m_windows)
    {
        if (win.m_enabled && !win.m_window->IsDocked())
        {
            win.m_window->Draw();
        }
    }
}

int DockableManager::GetDockedContentWidth()
{
    return m_renderedContentWidth;
}

int DockableManager::GetDockedContentHeight()
{
    return m_renderedContentHeight;
}

void DockableManager::ClampTargetVertScroll()
{
    int height = GetDockedContentHeight();
    int maxScroll = max(0, height - m_contentArea.h);
    m_targetVertScroll = SDL_clamp(m_targetVertScroll, 0.0f, (float)maxScroll);
    m_vertScroll = (int)m_targetVertScroll;
}

void DockableManager::ClampTargetHorizScroll()
{
    int width = GetDockedContentWidth();
    int maxScroll = max(0, width - m_contentArea.w);
    m_targetHorizScroll = SDL_clamp(m_targetHorizScroll, 0.0f, (float)maxScroll);
    m_horizScroll = (int)m_targetHorizScroll;
}

void DockableManager::CalcScrollBars()
{
    auto settings = gApp->GetSettings();

    m_vertBackArea = { m_area.x + m_area.w - settings->lineHeight + 2, m_area.y + settings->lineHeight + 2, settings->lineHeight - 4, m_area.h - settings->lineHeight * 2 - 4 };
    m_vertBarFullArea = { m_vertBackArea.x + 2, m_vertBackArea.y + 2, m_vertBackArea.w - 4, m_vertBackArea.h - 4 };

    m_horizBackArea = { m_area.x, m_area.y + m_area.h - settings->lineHeight + 2, m_area.w - settings->lineHeight - 4, settings->lineHeight - 4 };
    m_horizBarFullArea = { m_horizBackArea.x + 2, m_horizBackArea.y + 2, m_horizBackArea.w - 4, m_horizBackArea.h - 4 };

    int vStart = m_vertScroll;
    int vEnd = m_vertScroll + m_contentArea.h;
    int height = GetDockedContentHeight();
    float vStartRel = SDL_clamp((float)vStart / (float)height, 0.0f, 1.0f);
    float vEndRel = SDL_clamp((float)vEnd / (float)height, 0.0f, 1.0f);
    int vBarStart = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vStartRel);
    int vBarEnd = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vEndRel);
    m_vertBarArea = { m_vertBarFullArea.x, vBarStart, m_vertBarFullArea.w, vBarEnd - vBarStart + 1 };

    int hStart = m_horizScroll;
    int hEnd = m_horizScroll + m_contentArea.w;
    int width = GetDockedContentWidth();
    float hStartRel = SDL_clamp((float)hStart / (float)width, 0.0f, 1.0f);
    float hEndRel = SDL_clamp((float)hEnd / (float)width, 0.0f, 1.0f);
    int hBarStart = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hStartRel);
    int hBarEnd = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hEndRel);
    m_horizBarArea = { hBarStart, m_horizBarFullArea.y, hBarEnd - hBarStart + 1, m_horizBarFullArea.h };
}

void DockableManager::OnMouseMotionCaptured(int x, int y)
{
    auto settings = gApp->GetSettings();
    switch (m_grabMode)
    {
        case Grab_VScroll:
            {
                int newBarStart = y - m_dragMouseGrab.y;
                m_targetVertScroll = (float)(newBarStart - m_vertBarFullArea.y) / (float)m_vertBarFullArea.h * (float)GetDockedContentHeight();
                ClampTargetVertScroll();
            }
            break;
        case Grab_HScroll:
            {
                int newBarStart = x - m_dragMouseGrab.x;
                m_targetHorizScroll = (float)(newBarStart - m_horizBarFullArea.x) / (float)m_horizBarFullArea.w * (float)GetDockedContentWidth();
                ClampTargetHorizScroll();
            }
            break;
    }
}
