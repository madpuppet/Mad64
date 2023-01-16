#include "common.h"
#include "uiItem_enumButton.h"

UIItem_EnumButton::~UIItem_EnumButton()
{
}

void UIItem_EnumButton::Draw(SDL_Renderer* r) 
{
    BuildGE(r);

    auto settings = gApp->GetSettings();
    if (m_highlight)
        SDL_SetRenderDrawColor(r, 128, 128, 0, 128);
    else
        SDL_SetRenderDrawColor(r, 0, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &m_area);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 128);
    SDL_RenderDrawLine(r, m_area.x, m_area.y, m_area.x + m_area.w - 1, m_area.y);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderDrawLine(r, m_area.x, m_area.y+m_area.h-1, m_area.x + m_area.w - 1, m_area.y+m_area.h-1);

    SDL_Color col = { 255,255,255,255 };
    auto fr = gApp->GetFontRenderer();
    auto cs = fr->PrepareRender(r, m_options[m_currentOption], 0, 0, CachedFontRenderer::StandardFont);
    int offset = (m_fixedWidth - cs->rect.w) / 2;
    fr->RenderAt(cs, col, m_area.x + offset, m_area.y);
}

void UIItem_EnumButton::OnButtonDown(int button, int x, int y)
{
    if (button == 1)
    {
        m_currentOption = (m_currentOption + 1) % m_options.size();
        m_onChange(m_currentOption);
//        DeleteClear(m_geButtonText);
        m_highlight = true;
    }
    else if (button == 3)
    {
        m_currentOption = (m_currentOption + m_options.size() - 1) % (int)m_options.size();
        m_onChange(m_currentOption);
//        DeleteClear(m_geButtonText);
        m_highlight = true;
    }
}

void UIItem_EnumButton::OnButtonUp(int button, int x, int y) 
{
    m_highlight = false;
}

int UIItem_EnumButton::GetWidth()
{
    return m_fixedWidth;
}

int UIItem_EnumButton::GetHeight()
{
    auto settings = gApp->GetSettings();
    return 64;// m_geButtonText ? settings->lineHeight - settings->textYMargin * 2 : settings->lineHeight - 8;
}

void UIItem_EnumButton::OnRendererChange(SDL_Renderer* r)
{
    Log("UI TextButton Destroy");
//    DeleteClear(m_geButtonText);
    BuildGE(r);
    m_highlight = false;
}

void UIItem_EnumButton::BuildGE(SDL_Renderer *r)
{
//    if (!m_geButtonText)
//        m_geButtonText = GraphicElement::CreateFromText(r, gApp->GetFont(), m_options[m_currentOption].c_str(), { 255,255,255,255 }, 0, 0);
}

void UIItem_EnumButton::SetPos(int x, int y)
{
    auto settings = gApp->GetSettings();
    m_area.x = x;
    m_area.y = y+2;
    m_area.w = GetWidth();
    m_area.h = settings->lineHeight - 4;
}

void UIItem_EnumButton::SetArea(const SDL_Rect& area)
{
    m_area = area;
}

void UIItem_EnumButton::UpdateCursor(int x, int y)
{
    if (Overlaps(x, y))
        gApp->SetCursor(Cursor_Hand);
}

