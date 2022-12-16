#include "common.h"
#include "uiItem_textButton.h"

UIItem_TextButton::~UIItem_TextButton()
{
}

void UIItem_TextButton::Draw(SDL_Renderer* r) 
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
    SDL_RenderDrawLine(r, m_area.x, m_area.y + m_area.h - 1, m_area.x + m_area.w - 1, m_area.y + m_area.h - 1);

    if (m_geButtonText)
    {
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        m_geButtonText->RenderAt(r, m_area.x + 4, m_area.y + settings->textYMargin);
    }

}

void UIItem_TextButton::OnButtonDown(int button, int x, int y)
{
    if (button == 1)
    {
        m_onButtonPress();
        m_highlight = true;
    }
}

void UIItem_TextButton::OnButtonUp(int button, int x, int y) 
{
    m_highlight = false;
}

int UIItem_TextButton::GetWidth()
{
    auto settings = gApp->GetSettings();
    return m_geButtonText ? m_geButtonText->GetRect().w + 8 : settings->lineHeight - 8;
}

int UIItem_TextButton::GetHeight()
{
    auto settings = gApp->GetSettings();
    return m_geButtonText ? settings->lineHeight - settings->textYMargin * 2 : settings->lineHeight - 8;
}

void UIItem_TextButton::OnRendererChange(SDL_Renderer* r)
{
    Log("UI TextButton Destroy");
    DeleteClear(m_geButtonText);
    BuildGE(r);
}

void UIItem_TextButton::BuildGE(SDL_Renderer *r)
{
    if (!m_geButtonText && !m_text.empty())
        m_geButtonText = GraphicElement::CreateFromText(r, gApp->GetFont(), m_text.c_str(), { 255,255,255,255 }, 0, 0);
}

void UIItem_TextButton::SetPos(int x, int y)
{
    auto settings = gApp->GetSettings();
    m_area.x = x;
    m_area.y = y+2;
    m_area.w = GetWidth();
    m_area.h = settings->lineHeight - 4;
}

void UIItem_TextButton::SetArea(const SDL_Rect& area)
{
    m_area = area;
}

void UIItem_TextButton::UpdateCursor(int x, int y)
{
    if (Overlaps(x, y))
        gApp->SetCursor(Cursor_Hand);
}

