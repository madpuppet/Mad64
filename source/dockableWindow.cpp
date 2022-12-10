#include "common.h"
#include "dockableWindow.h"
#include "uiItem_TextButton.h"

DockableWindow::~DockableWindow()
{
}

void DockableWindow::OnMouseButtonDown(int button, int x, int y)
{
    auto settings = gApp->GetSettings();
    if (!m_isDocked && (y < settings->lineHeight) && button == 1)
    {
        // dragging
        m_dragMouseGrab = { x, y };
        m_dragging = true;
    }
    else if (!m_isDocked && (y > m_renderArea.h - settings->lineHeight) && (x > m_renderArea.w - settings->lineHeight))
    {
        int mouseX, mouseY;
        SDL_GetGlobalMouseState(&mouseX, &mouseY);

        // resizing
        m_resizing = true;
        m_dragMouseGrab = { m_renderArea.w - mouseX, m_renderArea.h - mouseY };
    }
    else
    {
        // check against title items
        for (auto item : m_titleIconsLeft)
        {
            if (item->Overlaps(x, y))
            {
                item->OnButtonDown(button, x, y);
                return;
            }
        }
        for (auto item : m_titleIconsRight)
        {
            if (item->Overlaps(x, y))
            {
                item->OnButtonDown(button, x, y);
                return;
            }
        }
    }
}

void DockableWindow::OnMouseButtonUp(int button, int x, int y)
{
    m_dragging = false;
    m_resizing = false;

    // check against title items
    for (auto item : m_titleIconsLeft)
    {
        if (item->Overlaps(x, y))
        {
            item->OnButtonUp(button, x, y);
            return;
        }
    }
    for (auto item : m_titleIconsRight)
    {
        if (item->Overlaps(x, y))
        {
            item->OnButtonDown(button, x, y);
            return;
        }
    }
}

void DockableWindow::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    if (m_dragging)
    {
        int mouseX, mouseY;
        SDL_GetGlobalMouseState(&mouseX, &mouseY);

        int windowPosX, windowPosY;
        windowPosX = mouseX - m_dragMouseGrab.x;
        windowPosY = mouseY - m_dragMouseGrab.y;
        SDL_SetWindowPosition(m_window, windowPosX, windowPosY);
    }
    if (m_resizing)
    {
        int mouseX, mouseY;
        SDL_GetGlobalMouseState(&mouseX, &mouseY);

        int w, h;
        w = SDL_max(32, mouseX + m_dragMouseGrab.x);
        h = SDL_max(32, mouseY + m_dragMouseGrab.y);
        SDL_SetWindowSize(m_window, w, h);
        m_windowArea.w = m_renderArea.w = w;
        m_windowArea.h = m_renderArea.h = h;
    }
}

DockableWindow::DockableWindow(const string& title) : m_title(title), m_isDocked(true), m_dragging(false), m_geTitle(nullptr), 
                                                                m_windowArea({ 100,100,640,480 }), m_dockedArea({ 0,0,640,480 })
{
}

void DockableWindow::OnRendererChange()
{
    auto r = GetRenderer();

    DeleteClear(m_geTitle);
    GenerateTitleGE();

    for (auto it : m_titleIconsLeft)
    {
        it->OnRendererChange(r);
    }
    for (auto it : m_titleIconsRight)
    {
        it->OnRendererChange(r);
    }

    OnChildRendererChange();
}


void DockableWindow::OnMouseWheel(int x, int y)
{
}

void DockableWindow::CreateIcons()
{
    for (auto item : m_titleIconsRight)
        DeleteClear(item);
    m_titleIconsRight.clear();
    for (auto item : m_titleIconsLeft)
        DeleteClear(item);
    m_titleIconsLeft.clear();
    m_titleIconsRight.push_back(new UIItem_TextButton("^", DELEGATE(DockableWindow::OnDockPress)));
    CreateChildIcons();
}

void DockableWindow::LayoutIcons()
{
    auto settings = gApp->GetSettings();
    GenerateTitleGE();
    m_geTitle->SetPos(m_renderArea.x + settings->textXMargin, m_renderArea.y + settings->textYMargin);
    int leftX = m_isDocked ? m_geTitle->GetRect().x + m_geTitle->GetRect().w : settings->textXMargin;
    for (auto item : m_titleIconsLeft)
    {
        int w = item->GetWidth();
        item->SetPos(leftX, m_renderArea.y);
        leftX -= w - settings->textXMargin;
    }
    int rightX = m_renderArea.x + m_renderArea.w;
    for (auto item : m_titleIconsRight)
    {
        int w = item->GetWidth();
        item->SetPos(rightX - w, m_renderArea.y);
        rightX += w + settings->textXMargin;
    }
}

void DockableWindow::OnDockPress()
{
    if (m_isDocked)
        Undock();
    else
        Dock();
}


void DockableWindow::Draw()
{
    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    SDL_RenderSetClipRect(r, &m_renderArea);
    SDL_SetRenderDrawColor(r, settings->backColor.r, settings->backColor.g, settings->backColor.b, 255);
    SDL_RenderFillRect(r, &m_renderArea);

    SDL_Rect titleRect = { m_renderArea.x, m_renderArea.y, m_renderArea.w, settings->lineHeight };
    SDL_SetRenderDrawColor(r, 32, 64, 128, 255);
    SDL_RenderFillRect(r, &titleRect);

    if (!m_isDocked)
    {
        SDL_Rect borderLeft = { 0, 0, 2, m_renderArea.h };
        SDL_RenderFillRect(r, &borderLeft);

        SDL_Rect borderRight = { m_renderArea.w - 2, 0, 2, m_renderArea.h };
        SDL_RenderFillRect(r, &borderRight);

        SDL_Rect borderBottom = { 0, m_renderArea.h - 2, m_renderArea.w, 2 };
        SDL_RenderFillRect(r, &borderBottom);
    }

    GenerateTitleGE();
    m_geTitle->Render(r);

    DrawChild();

    for (auto icon : m_titleIconsLeft)
    {
        icon->Draw(r);
    }
    for (auto icon : m_titleIconsRight)
    {
        icon->Draw(r);
    }

    SDL_RenderSetClipRect(r, nullptr);

    if (!m_isDocked)
        SDL_RenderPresent(r);
}

void DockableWindow::SetRect(const SDL_Rect& rect)
{
    m_dockedArea = rect;
    if (m_isDocked)
    {
        m_renderArea = m_dockedArea;
        OnResize();
    }
}

SDL_Renderer* DockableWindow::GetRenderer()
{
    return m_renderer ? m_renderer : gApp->GetRenderer();
}

void DockableWindow::OnResize()
{
    LayoutIcons();
}

void DockableWindow::SetTitle(const string& str)
{
    m_title = str;
    delete m_geTitle;
    m_geTitle = nullptr;
}

void DockableWindow::GenerateTitleGE()
{
    if (!m_geTitle)
    {
        auto settings = gApp->GetSettings();
        auto r = GetRenderer();
        m_geTitle = GraphicElement::CreateFromText(r, gApp->GetFont(), m_title.c_str(), settings->helpGroupColor, m_renderArea.x + settings->textXMargin, m_renderArea.y + settings->textYMargin);
    }
}


void DockableWindow::Undock()
{
    m_window = SDL_CreateWindow("Compiler Log", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowArea.w, m_windowArea.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_window == NULL)
    {
        Log("ERROR: Cannot undock window");
        return;
    }
    SDL_SetWindowPosition(m_window, m_windowArea.x, m_windowArea.y);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    m_isDocked = false;

    m_renderArea.x = 0;
    m_renderArea.y = 0;
    m_renderArea.w = m_windowArea.w;
    m_renderArea.h = m_windowArea.h;

    OnRendererChange();
    LayoutIcons();
}

int DockableWindow::GetID()
{
    if (m_window)
        return SDL_GetWindowID(m_window);
    else
        return -1;
}

void DockableWindow::ShowWindow(bool enable)
{
    if (enable && m_window)
        SDL_ShowWindow(m_window);
    else if (!enable && m_window)
        SDL_HideWindow(m_window);
}

void DockableWindow::Dock()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_window = 0;
    m_renderer = 0;
    m_isDocked = true;
    m_renderArea = m_dockedArea;

    OnRendererChange();
    LayoutIcons();
}
