#include "common.h"
#include "dockableWindow.h"
#include "uiItem_textButton.h"

DockableWindow::~DockableWindow()
{
}

void DockableWindow::OnMouseButtonDown(int button, int x, int y)
{
    auto settings = gApp->GetSettings();

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

    if (!m_isDocked)
    {
        if ((y < settings->lineHeight) && button == 1)
        {
            // dragging
            m_dragMouseGrab = { x, y };
            m_grabMode = Dragging;
            return;
        }
        if ((y > m_renderArea.h - settings->lineHeight) && (x > m_renderArea.w - settings->lineHeight))
        {
            int mouseX, mouseY;
            SDL_GetGlobalMouseState(&mouseX, &mouseY);

            // resizing
            m_grabMode = ResizeRight;
            m_dragMouseGrab = { m_renderArea.w - mouseX, m_renderArea.h - mouseY };
            return;
        }
        if ((y > m_renderArea.h - settings->lineHeight) && (x < settings->lineHeight))
        {
            int mouseX, mouseY;
            SDL_GetGlobalMouseState(&mouseX, &mouseY);

            // resizing
            m_grabMode = ResizeLeft;
            m_dragMouseGrab = { x, m_renderArea.h - mouseY };
            return;
        }
        if (Contains(m_vertBarFullArea,x,y))
        {
            if (Contains(m_vertBarArea, x, y))
            {
                m_grabMode = VScrollbar;
                m_dragMouseGrab = { x - m_vertBarArea.x, y - m_vertBarArea.y };
                return;
            }
            else if (y < m_vertBarArea.y)
            {
                // page up
                m_targetVertScroll -= m_contentArea.h;
                ClampTargetVertScroll();
                return;
            }
            else if (y > m_vertBarArea.y + m_vertBarArea.h)
            {
                // page down
                m_targetVertScroll += m_contentArea.h;
                ClampTargetVertScroll();
                return;
            }
        }
        if (Contains(m_horizBarFullArea, x, y))
        {
            if (Contains(m_horizBarArea, x, y))
            {
                m_grabMode = HScrollbar;
                m_dragMouseGrab = { x - m_horizBarArea.x, y - m_horizBarArea.y };
                return;
            }
            else if (x < m_horizBarArea.x)
            {
                // page left
                m_targetHorizScroll -= m_contentArea.w;
                ClampTargetHorizScroll();
                return;
            }
            else if (x > m_horizBarArea.x + m_horizBarArea.w)
            {
                // page right
                m_targetHorizScroll += m_contentArea.w;
                ClampTargetHorizScroll();
                return;
            }
        }
    }
}

void DockableWindow::OnMouseButtonUp(int button, int x, int y)
{
    m_grabMode = None;

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
            item->OnButtonUp(button, x, y);
            return;
        }
    }
}

void DockableWindow::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    switch (m_grabMode)
    {
        case Dragging:
            {
                int mouseX, mouseY;
                SDL_GetGlobalMouseState(&mouseX, &mouseY);

                int windowPosX, windowPosY;
                windowPosX = mouseX - m_dragMouseGrab.x;
                windowPosY = mouseY - m_dragMouseGrab.y;
                SDL_SetWindowPosition(m_window, windowPosX, windowPosY);
            }
            break;
        case ResizeLeft:
            {
                auto settings = gApp->GetSettings();
                int minSize = settings->lineHeight * 3;

                int mouseX, mouseY;
                SDL_GetGlobalMouseState(&mouseX, &mouseY);

                int x, y;
                SDL_GetWindowPosition(m_window, &x, &y);

                int w, h;
                SDL_GetWindowSize(m_window, &w, &h);

                int x2 = x + w;
                x = SDL_min(x2 - minSize, mouseX - m_dragMouseGrab.x);

                w = SDL_max(minSize, x2 - x);
                h = SDL_max(minSize, mouseY + m_dragMouseGrab.y);
                SDL_SetWindowSize(m_window, w, h);
                SDL_SetWindowPosition(m_window, x, y);

                m_windowArea.w = m_renderArea.w = w;
                m_windowArea.h = m_renderArea.h = h;

                OnResize();
            }
            break;
        case ResizeRight:
            {
                auto settings = gApp->GetSettings();
                int minSize = settings->lineHeight * 3;

                int mouseX, mouseY;
                SDL_GetGlobalMouseState(&mouseX, &mouseY);

                int w, h;
                w = SDL_max(minSize, mouseX + m_dragMouseGrab.x);
                h = SDL_max(minSize, mouseY + m_dragMouseGrab.y);
                SDL_SetWindowSize(m_window, w, h);
                m_windowArea.w = m_renderArea.w = w;
                m_windowArea.h = m_renderArea.h = h;

                OnResize();
            }
            break;
        case VScrollbar:
            {
                int newBarStart = yAbs - m_dragMouseGrab.y;
                m_targetVertScroll = (float)(newBarStart - m_vertBarFullArea.y) / (float)m_vertBarFullArea.h * (float)GetContentHeight();
                ClampTargetVertScroll();
            }
            break;
        case HScrollbar:
            {
                int newBarStart = xAbs - m_dragMouseGrab.x;
                m_targetHorizScroll = (float)(newBarStart - m_horizBarFullArea.x) / (float)m_horizBarFullArea.w * (float)GetContentWidth();
                ClampTargetHorizScroll();
            }
            break;
    }
}

DockableWindow::DockableWindow(const string& title) : m_title(title), m_isDocked(true), m_grabMode(None), m_geTitle(nullptr), 
                                                                m_windowArea({ 100,100,640,480 }), m_dockedArea({ 0,0,640,480 })
{
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowArea.w, m_windowArea.h, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_window == NULL)
    {
        Log("ERROR: Cannot create undocked window for %s",m_title.c_str());
        return;
    }
    SDL_SetWindowPosition(m_window, m_windowArea.x, m_windowArea.y);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

    m_vertScroll = 0;
    m_horizScroll = 0;
    m_targetVertScroll = 0;
    m_targetHorizScroll = 0;
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


void DockableWindow::OnMouseWheel(int mouseX, int mouseY, int wheelX, int wheelY)
{
    m_targetVertScroll -= wheelY * 50;
    ClampTargetVertScroll();
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
    int leftX = m_isDocked ? m_geTitle->GetRect().x + m_geTitle->GetRect().w + settings->textXMargin : settings->textXMargin;
    for (auto item : m_titleIconsLeft)
    {
        int w = item->GetWidth();
        item->SetPos(leftX, m_renderArea.y + 2);
        leftX += w + settings->textXMargin;
    }
    int rightX = m_renderArea.x + m_renderArea.w - 4;
    for (auto item : m_titleIconsRight)
    {
        int w = item->GetWidth();
        item->SetPos(rightX - w, m_renderArea.y + 2);
        rightX -= w + settings->textXMargin;
    }
}

void DockableWindow::OnDockPress()
{
    if (m_isDocked)
        Undock();
    else
        Dock();
}

void DockableWindow::CalcScrollBars()
{
    auto settings = gApp->GetSettings();

    m_vertBackArea = { m_renderArea.w - settings->lineHeight + 2, settings->lineHeight + 2, settings->lineHeight - 4, m_renderArea.h - settings->lineHeight * 2 - 4 };
    m_vertBarFullArea = { m_vertBackArea.x + 2, m_vertBackArea.y + 2, m_vertBackArea.w - 4, m_vertBackArea.h - 4 };

    m_horizBackArea = { settings->lineHeight + 2, m_renderArea.h - settings->lineHeight + 2, m_renderArea.w - settings->lineHeight*2 - 4, settings->lineHeight - 4 };
    m_horizBarFullArea = { m_horizBackArea.x + 2, m_horizBackArea.y + 2, m_horizBackArea.w - 4, m_horizBackArea.h - 4 };

    int vStart = m_vertScroll;
    int vEnd = m_vertScroll + m_contentArea.h;
    int height = GetContentHeight();
    float vStartRel = SDL_clamp((float)vStart / (float)height, 0.0f, 1.0f);
    float vEndRel = SDL_clamp((float)vEnd / (float)height, 0.0f, 1.0f);
    int vBarStart = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vStartRel);
    int vBarEnd = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vEndRel);
    m_vertBarArea = { m_vertBarFullArea.x, vBarStart, m_vertBarFullArea.w, vBarEnd - vBarStart + 1 };

    int hStart = m_horizScroll;
    int hEnd = m_horizScroll + m_contentArea.w;
    int width = GetContentWidth();
    float hStartRel = SDL_clamp((float)hStart / (float)width, 0.0f, 1.0f);
    float hEndRel = SDL_clamp((float)hEnd / (float)width, 0.0f, 1.0f);
    int hBarStart = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hStartRel);
    int hBarEnd = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hEndRel);
    m_horizBarArea = { hBarStart, m_horizBarFullArea.y, hBarEnd - hBarStart + 1, m_horizBarFullArea.h };
}

void DockableWindow::DrawTitle()
{
    auto r = GetRenderer();
    auto settings = gApp->GetSettings();
    SDL_Rect titleRect = { m_renderArea.x, m_renderArea.y, m_renderArea.w, settings->lineHeight };
    SDL_SetRenderDrawColor(r, 32, 64, 128, 255);
    SDL_RenderFillRect(r, &titleRect);

    SDL_SetRenderDrawColor(r, 64, 64, 128, 255);
    GenerateTitleGE();
    m_geTitle->Render(r);

    for (auto icon : m_titleIconsLeft)
    {
        icon->Draw(r);
    }
    for (auto icon : m_titleIconsRight)
    {
        icon->Draw(r);
    }
}

void DockableWindow::DrawContent()
{
    if (m_contentDirty)
    {
        ClampTargetVertScroll();
        ClampTargetHorizScroll();
        m_contentDirty = false;
    }

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    SDL_SetRenderDrawColor(r, settings->backColor.r, settings->backColor.g, settings->backColor.b, 255);
    SDL_RenderFillRect(r, &m_contentArea);

    if (!m_isDocked)
    {
        SDL_SetRenderDrawColor(r, 64, 64, 128, 255);

        // left border
        SDL_Rect borderLeft = { 0, settings->lineHeight, 2, m_renderArea.h-settings->lineHeight };
        SDL_RenderFillRect(r, &borderLeft);

        // right border
        SDL_Rect borderRight = { m_renderArea.w - settings->lineHeight, settings->lineHeight, settings->lineHeight, m_renderArea.h - settings->lineHeight };
        SDL_RenderFillRect(r, &borderRight);

        // bottom border
        SDL_Rect borderBottom = { 0, m_renderArea.h - settings->lineHeight, m_renderArea.w, settings->lineHeight };
        SDL_RenderFillRect(r, &borderBottom);

        // resize triangle
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_Vertex verts[6] = {
            { {(float)m_renderArea.w-settings->lineHeight,(float)m_renderArea.h}, {32, 64, 128, 255}, {0,0} },
            { {(float)m_renderArea.w,(float)m_renderArea.h}, {64, 128, 255, 255}, {0,0} },
            { {(float)m_renderArea.w,(float)m_renderArea.h - settings->lineHeight}, {32, 64, 128, 255}, {0,0} },

            { {(float)settings->lineHeight, (float)m_renderArea.h}, { 32, 64, 128, 255 }, { 0,0 } },
            { {(float)0,(float)m_renderArea.h}, {64, 128, 255, 255}, {0,0} },
            { {(float)0,(float)m_renderArea.h - settings->lineHeight}, {32, 64, 128, 255}, {0,0} }
        };

        int indices[6] = { 0, 1, 2, 3,4,5 };
        SDL_RenderGeometry(r, nullptr, verts, 6, indices, 6);

        CalcScrollBars();

        SDL_SetRenderDrawColor(r, 32, 32, 64, 255);
        SDL_RenderFillRect(r, &m_vertBackArea);
        SDL_RenderFillRect(r, &m_horizBackArea);

        SDL_SetRenderDrawColor(r, 128, 128, 196, 255);
        SDL_RenderFillRect(r, &m_vertBarArea);
        SDL_RenderFillRect(r, &m_horizBarArea);
    }

    if (!m_isDocked)
    {
        SDL_RenderSetClipRect(r, &m_contentArea);
        DrawChild();
        SDL_RenderSetClipRect(r, nullptr);
        SDL_RenderPresent(r);
    }
    else
    {
        DrawChild();
    }
}

void DockableWindow::SetRect(const SDL_Rect& rect)
{
    auto settings = gApp->GetSettings();
    m_dockedArea = rect;
    if (m_isDocked)
    {
        m_renderArea = rect;
        OnResize();
    }
}

SDL_Renderer* DockableWindow::GetRenderer()
{
    return m_isDocked ? gApp->GetRenderer() : m_renderer;
}

void DockableWindow::OnResize()
{
    UpdateContentArea();
    LayoutIcons();
    ClampTargetVertScroll();
    ClampTargetHorizScroll();
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

void DockableWindow::UpdateContentArea()
{
    auto settings = gApp->GetSettings();
    if (m_isDocked)
    {
        m_contentArea = { m_renderArea.x, m_renderArea.y + settings->lineHeight, m_renderArea.w, m_renderArea.h - settings->lineHeight };
    }
    else
    {
        m_contentArea = { m_renderArea.x, m_renderArea.y + settings->lineHeight, m_renderArea.w - settings->lineHeight, m_renderArea.h - settings->lineHeight * 2 };
    }
}

void DockableWindow::Undock()
{
    SDL_ShowWindow(m_window);

    m_isDocked = false;

    m_renderArea.x = 0;
    m_renderArea.y = 0;
    m_renderArea.w = m_windowArea.w;
    m_renderArea.h = m_windowArea.h;

    UpdateContentArea();
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
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
}

void DockableWindow::Dock()
{
    auto settings = gApp->GetSettings();

    SDL_HideWindow(m_window);

    m_isDocked = true;
    m_renderArea = m_dockedArea;

    UpdateContentArea();
    OnRendererChange();
    LayoutIcons();
}

void DockableWindow::ClampTargetVertScroll()
{
    int height = GetContentHeight();
    int maxScroll = max(0, height - m_contentArea.h);
    m_targetVertScroll = SDL_clamp(m_targetVertScroll, 0.0f, (float)maxScroll);
    m_vertScroll = (int)m_targetVertScroll;
}
void DockableWindow::ClampTargetHorizScroll()
{
    int width = GetContentWidth();
    int maxScroll = max(0, width - m_contentArea.w);
    m_targetHorizScroll = SDL_clamp(m_targetHorizScroll, 0.0f, (float)maxScroll);
    m_horizScroll = (int)m_targetHorizScroll;
}

