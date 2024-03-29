#include "common.h"
#include "dockableWindow_log.h"

void DockableWindow_Log::OnChildRendererChange()
{
}

void DockableWindow_Log::Clear()
{
    m_items.clear();
    SetContentDirty();
}

void DockableWindow_Log::DrawChild()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;
    m_renderedItems.clear();

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();
    auto fr = gApp->GetFontRenderer();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    const string& filter = m_filterBox->GetText();

    for (auto& it : m_items)
    {
        if (!filter.empty() && StrFind(it.text.c_str(), filter) == string::npos)
            continue;

        SDL_Rect rect = { 0,0,100,100 };
        SDL_Color colWhite = { 255, 255, 255, 255 };
        {
            fr->RenderText(r, it.text, colWhite, x, y, CachedFontRenderer::StandardFont, &rect, false);
        }
        if (it.addr != -1)
        {
            auto emu = gApp->GetEmulator();
            auto str = FormatString("%02x %02x %02x %02x", emu->GetByte(it.addr), emu->GetByte(it.addr + 1),
                emu->GetByte(it.addr + 2), emu->GetByte(it.addr + 3));

            SDL_Color col = { 0,255,0,255 };
            fr->RenderText(r, str, col, rect.w + x + 16, y, CachedFontRenderer::StandardFont, nullptr, false);
        }
        if (it.lineNmbr != -1)
        {
            RenderedItem item;
            item.area = { x, y, rect.w, rect.h };
            item.lineNmbr = it.lineNmbr;
            m_renderedItems.push_back(item);
        }
        m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + rect.w);
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
}

void DockableWindow_Log::OnFilterStringEnter(const string& text)
{
}
void DockableWindow_Log::OnFilterStringChange(const string& text)
{
}

void DockableWindow_Log::CreateChildIcons()
{
    m_filterBox = new UIItem_TextBox("Filter", "<filter>", 200, DELEGATE(DockableWindow_Log::OnFilterStringEnter), DELEGATE(DockableWindow_Log::OnFilterStringChange));
    m_titleIconsLeft.push_back(m_filterBox);
}

void DockableWindow_Log::LogText(const string& text, int lineNmbr, int color, int addr)
{
    auto settings = gApp->GetSettings();
    int y = 0;
    SDL_Color col = { 255,255,255,255 };
    LineItem item;
    item.text = text;
    item.y = y;
    item.lineNmbr = lineNmbr;
    item.colorIdx = color;
    item.addr = addr;
    y += settings->lineHeight;
    m_items.push_back(item);
    SetContentDirty();
}

int DockableWindow_Log::GetContentHeight()
{
    auto settings = gApp->GetSettings();
    return settings->lineHeight * (int)m_items.size();
}

int DockableWindow_Log::GetContentWidth()
{
    return m_renderedWidth;
}

void DockableWindow_Log::OnMouseButtonDown(int button, int x, int y)
{
    if (Contains(m_contentArea, x, y))
    {
        for (auto& item : m_renderedItems)
        {
            if (Contains(item.area, x, y))
            {
                gApp->GetEditWindow()->GotoLineCol(item.lineNmbr, 0, MARK_None, true);
                return;
            }
        }
    }
    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_Log::LogTextArray(const char** textArray, int col)
{
    const char** t = textArray;
    while (*t)
    {
        LogText(string(*t), -1, col);
        t++;
    }
}


void DockableWindow_Log::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    for (auto& item : m_renderedItems)
    {
        if (Contains(item.area, xAbs, yAbs))
        {
            gApp->SetCursor(Cursor_Hand);
        }
    }
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}


