#include "common.h"
#include "graphicChunk.h"

GraphicElement::GraphicElement(SDL_Texture *tex, i32 x, i32 y, bool ownTexture)
{
    m_enabled = true;
    m_tex = tex;
    m_ownTexture = ownTexture;

    i32 w, h;
    SDL_QueryTexture(m_tex, NULL, NULL, &w, &h);
    m_quad.x = x;
    m_quad.y = y;
    m_quad.w = w;
    m_quad.h = h;
    m_enabled = true;
}

GraphicElement::~GraphicElement()
{
    if (m_ownTexture)
        SDL_DestroyTexture(m_tex);
}

GraphicElement* GraphicElement::CreateFromImage(SDL_Renderer* r, const char* path, i32 x, i32 y)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);
    return new GraphicElement(tex, x, y, true);
}

GraphicElement* GraphicElement::CreateFromTexture(SDL_Texture* tex, i32 x, i32 y, bool ownTexture)
{
    return new GraphicElement(tex, x, y, ownTexture);
}

static vector<u16> convert_unicode;
GraphicElement* GraphicElement::CreateFromText(SDL_Renderer* r, TTF_Font* font, const char* text, const SDL_Color& col, i32 x, i32 y)
{
    SDL_Color backCol = { 0, 0, 0, 0 };

    size_t len = SDL_strlen(text) + 1;
    convert_unicode.resize(len);
    for (size_t i = 0; i < len; i++)
    {
        convert_unicode[i] = (u16)((u8)text[i]);
    }
    SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, convert_unicode.data(), col);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surface);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surface);
    return new GraphicElement(tex, x, y, true);
}

void GraphicElement::SetText(SDL_Renderer* r, TTF_Font* font, const char* text, const SDL_Color& col)
{
    if (m_ownTexture && m_tex)
        SDL_DestroyTexture(m_tex);

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, col);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);
}

void GraphicElement::SetTexture(SDL_Texture* tex, bool ownTexture)
{
    // never set a texture to itself if you already own it.
    SDL_assert(!m_ownTexture || tex != m_tex);

    if (m_ownTexture && m_tex)
        SDL_DestroyTexture(m_tex);

    m_ownTexture = ownTexture;
    m_tex = tex;
}

void GraphicChunk::Draw()
{
    auto r = gApp->GetRenderer();
    for (auto ge : elements)
    {
        if (ge->IsEnabled())
            SDL_RenderCopy(r, ge->GetTexture(), NULL, &ge->GetRect());
    }
}

void GraphicChunk::DrawAt(i32 x, i32 y)
{
    for (auto ge : elements)
    {
        if (ge->IsEnabled())
        {
            SDL_Rect quad = { ge->GetRect().x + x, ge->GetRect().y + y, ge->GetRect().w, ge->GetRect().h };
            SDL_RenderCopy(gApp->GetRenderer(), ge->GetTexture(), NULL, &quad);
        }
    }
}

int GraphicChunk::CalcMaxWidth()
{
    int w = 0;
    for (auto ge : elements)
    {
        if (ge->IsEnabled())
        {
            w = SDL_max(ge->GetRect().x + ge->GetRect().w, w);
        }
    }
    return w;
}

void GraphicChunk::DrawElemAt(int i, i32 x, i32 y)
{
    if (i < elements.size())
    {
        auto ge = elements[i];
        if (ge->IsEnabled())
        {
            SDL_Rect quad = { ge->GetRect().x + x, ge->GetRect().y + y, ge->GetRect().w, ge->GetRect().h };
            SDL_RenderCopy(gApp->GetRenderer(), ge->GetTexture(), NULL, &quad);
        }
    }
}


GraphicChunk::~GraphicChunk()
{
    Clear();
}

void GraphicChunk::Clear()
{
    for (auto ge : elements)
        delete ge;
    elements.clear();
}

