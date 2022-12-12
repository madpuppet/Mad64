#include "common.h"
#include "dockableWindow_log.h"

void DockableWindow_Log::OnChildRendererChange()
{
    Log("ChildRendererChange: Destroy");
    for (auto& it : m_items)
    {
        DeleteClear(it.ge);
    }
}

void DockableWindow_Log::Clear()
{
    for (auto& it : m_items)
    {
        delete it.ge;
    }
    m_items.clear();
    SetContentDirty();
}

void DockableWindow_Log::DrawChild()
{
    m_renderedWidth = 64;
    m_renderedItems.clear();

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    for (auto& it : m_items)
    {
        if (!it.ge)
        {
            SDL_Color col = { 255, 255, 255, 255 };
            it.ge = GraphicElement::CreateFromText(GetRenderer(), gApp->GetFont(), it.text.c_str(), col, x, y);
        }

        if (it.ge)
        {
            it.ge->RenderAt(r, x, y);
            if (it.addr != -1)
            {
                auto emu = gApp->GetEmulator();
                auto str = FormatString("%02x %02x %02x %02x", emu->GetByte(it.addr), emu->GetByte(it.addr + 1),
                    emu->GetByte(it.addr + 2), emu->GetByte(it.addr + 3));
                GraphicElement::RenderText(r, gApp->GetFont(), str.c_str(), { 0,255,0,255 }, it.ge->GetRect().w + x + 16, y);
            }
            if (it.lineNmbr != -1)
            {
                RenderedItem item;
                item.area = { x, y, it.ge->GetRect().w, it.ge->GetRect().h };
                item.lineNmbr = it.lineNmbr;
                m_renderedItems.push_back(item);
                m_renderedWidth = SDL_max(m_renderedWidth, item.area.w);
            }
            y += settings->lineHeight;
        }
    }
}

void DockableWindow_Log::CreateChildIcons()
{

}

void DockableWindow_Log::LogText(const string& text, int lineNmbr, int color, int addr)
{
    auto settings = gApp->GetSettings();
    int y = 0;
    SDL_Color col = { 255,255,255,255 };
    LineItem item;
    item.text = text;
    item.y = y;
    item.ge = nullptr;
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
    return settings->lineHeight * ((int)m_items.size() + 1);
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


