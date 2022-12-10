#include "common.h"
#include "dockableWindow_Log.h"

void DockableWindow_Log::OnChildRendererChange()
{
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
}

void DockableWindow_Log::DrawChild()
{
    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_renderArea.y + settings->lineHeight;
    int x = m_renderArea.x + settings->textXMargin;
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
            y += settings->lineHeight;
        }
    }
}

void DockableWindow_Log::CreateChildIcons()
{

}

void DockableWindow_Log::Log(const string& text, int color, int lineNmbr)
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
    y += settings->lineHeight;
    m_items.push_back(item);
}

int DockableWindow_Log::GetHeight()
{
    auto settings = gApp->GetSettings();
    return settings->lineHeight * (m_items.size() + 1);
}
