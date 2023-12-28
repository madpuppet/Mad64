#pragma once

#include <unordered_map>
#include <list>

class CachedFontRenderer
{
public:
    CachedFontRenderer();
    ~CachedFontRenderer();

    enum FontIDX
    {
        StandardFont,
        C64Font
    };

    struct CachedString
    {
        SDL_Renderer* renderer;
        SDL_Rect rect;
        size_t hash;
        string str;
        SDL_Texture* tex;
        int w, h;
        list<CachedString*>::iterator it;
    };

    void RenderText(SDL_Renderer *renderer, const string& str, const SDL_Color& col, int x, int y, FontIDX fontIdx, SDL_Rect *outputQuad, bool bCalcSizeOnly);
    void RenderTextUnicode(SDL_Renderer* renderer, const vector<u16>& str, const SDL_Color& col, int x, int y, FontIDX fontIdx, SDL_Rect* outputQuad, bool bCalcSizeOnly);

    // split render into 2 phases, allowing you to do other things with the render rectangle
    // do not retain the cached string over long periods as it could be reused eventually (after 256 renders)
    struct CachedString *PrepareRender(SDL_Renderer* renderer, const string& str, int x, int y, FontIDX fontIdx);
    struct CachedString* PrepareRenderUnicode(SDL_Renderer* renderer, const vector<u16>& uniStr, int x, int y, FontIDX fontIdx);
    void Render(CachedString *cs, const SDL_Color& col);
    void RenderAt(CachedString* cs, const SDL_Color& col, int x, int y);

protected:
    size_t Hash(SDL_Renderer* renderer, const std::vector<u16>& str, FontIDX fontIdx);
    list<CachedString*> m_ordered;
    unordered_map<size_t, CachedString*> m_map;
};
