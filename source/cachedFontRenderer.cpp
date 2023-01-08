#include "common.h"
#include "cachedFontRenderer.h"
#include <algorithm>

#define MAX_ITEMS 256

CachedFontRenderer::CachedFontRenderer()
{
}

CachedFontRenderer::~CachedFontRenderer()
{
}

size_t CachedFontRenderer::Hash(SDL_Renderer* renderer, const string& str, FontIDX fontIdx)
{
    size_t hash = std::hash<std::string>()(str);
    intptr_t rhash = (intptr_t)renderer;
    i64 val = fontIdx;
    return hash ^ rhash ^ (0x1b852967 * val);
}

static vector<u16> convert_unicode;

// split render into 2 phases, allowing you to do other things with the render rectangle
CachedFontRenderer::CachedString * CachedFontRenderer::PrepareRender(SDL_Renderer * renderer, const string & str, int x, int y, FontIDX fontIdx)
{
    TTF_Font* font = (fontIdx == StandardFont) ? gApp->GetFont() : gApp->GetFontC64();
    CachedString* cs = nullptr;
    size_t hash = Hash(renderer, str, fontIdx);

    auto it = m_map.find(hash);
    if (it != m_map.end())
    {
        // check for collision
        if (it->second->str != str)
        {
            Log("Collision: %s -> %s", str.c_str(), it->second->str.c_str());
        }
        cs = it->second;

        // move it to end of ordered list - so it is now most recently used
        m_ordered.erase(cs->it);
        m_ordered.push_back(cs);
        cs->it = std::prev(m_ordered.end());

        cs->rect.x = x;
        cs->rect.y = y;
    }
    else
    {
        if (m_ordered.size() == MAX_ITEMS)
        {
            // list is full, grab the least recently used one, move it the most recent position
            cs = m_ordered.front();
            m_map.erase(cs->hash);
            SDL_DestroyTexture(cs->tex);
            m_ordered.erase(m_ordered.begin());
            m_ordered.push_back(cs);
        }
        else
        {
            // list is not full, create a new element and add it to the queue
            cs = new CachedString();
            m_ordered.push_back(cs);
        }

        cs->it = std::prev(m_ordered.end());
        cs->str = str;
        cs->hash = hash;
        cs->renderer = renderer;
        cs->rect.x = x;
        cs->rect.y = y;

        m_map[hash] = cs;

        SDL_Color white = { 255, 255, 255, 255 };
        size_t len = str.size() + 1;
        convert_unicode.resize(len);
        for (size_t i = 0; i < len; i++)
        {
            convert_unicode[i] = (u16)((u8)str[i]);
        }
        SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, convert_unicode.data(), white);
        cs->tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(cs->tex, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surface);
        SDL_QueryTexture(cs->tex, NULL, NULL, &cs->rect.w, &cs->rect.h);
    }
    return cs;
}

void CachedFontRenderer::Render(CachedString* cs, const SDL_Color& col)
{
    // got tex, now render it
    SDL_SetTextureColorMod(cs->tex, col.r, col.g, col.b);
    SDL_SetTextureAlphaMod(cs->tex, col.a);
    SDL_RenderCopy(cs->renderer, cs->tex, NULL, &cs->rect);
}

void CachedFontRenderer::RenderAt(CachedString* cs, const SDL_Color& col, int x, int y)
{
    // got tex, now render it
    SDL_Rect quad = { x, y, cs->rect.w, cs->rect.h };
    SDL_SetTextureColorMod(cs->tex, col.r, col.g, col.b);
    SDL_SetTextureAlphaMod(cs->tex, col.a);
    SDL_RenderCopy(cs->renderer, cs->tex, NULL, &quad);
}

void CachedFontRenderer::RenderText(SDL_Renderer* renderer, const string& str, const SDL_Color& col, int x, int y, FontIDX fontIdx, SDL_Rect* outputQuad, bool bCalcSizeOnly)
{
    auto cs = PrepareRender(renderer, str, x, y, fontIdx);
    if (!bCalcSizeOnly)
        Render(cs, col);
    if (outputQuad)
        *outputQuad = cs->rect;
}


