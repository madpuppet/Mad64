#include "common.h"
#include "uiItem_textButton.h"

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

    auto fr = gApp->GetFontRenderer();
    SDL_Color col = { 255,255,255,255 };
    fr->RenderText(r, m_text, col, m_area.x + 4, m_area.y, CachedFontRenderer::StandardFont, nullptr, false);
}

void UIItem_TextButton::OnButtonDown(int button, int x, int y)
{
    if (button == 1)
    {
        m_highlight = true;
        m_onButtonPress();
    }
}

void UIItem_TextButton::OnButtonUp(int button, int x, int y) 
{
    m_highlight = false;
}

int UIItem_TextButton::GetWidth()
{
    return m_width;
}

int UIItem_TextButton::GetHeight()
{
    return m_height;
}

void UIItem_TextButton::OnRendererChange(SDL_Renderer* r)
{
    BuildGE(r);
    m_highlight = false;
}

void UIItem_TextButton::BuildGE(SDL_Renderer *r)
{
    auto settings = gApp->GetSettings();
    auto fr = gApp->GetFontRenderer();
    SDL_Color col = { 255,255,255,255 };
    auto cs = fr->PrepareRender(r, m_text, m_area.x + 4, m_area.y, CachedFontRenderer::StandardFont);
    m_width = cs->rect.w + settings->textXMargin;
    m_height = cs->rect.h;
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

