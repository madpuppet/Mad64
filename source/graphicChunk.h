#pragma once

struct GraphicElement
{
    static GraphicElement* CreateFromText(SDL_Renderer* renderer, TTF_Font* font, const char* text, const SDL_Color& col, const SDL_Point& pos)
    {
        GraphicElement* ge = new GraphicElement();

        SDL_Surface* surface = TTF_RenderText_Solid(font, text, col);
        ge->tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Point size;
        SDL_QueryTexture(ge->tex, NULL, NULL, &size.x, &size.y);
        ge->quad.x = pos.x;
        ge->quad.y = pos.y;
        ge->quad.w = size.x;
        ge->quad.h = size.y;
        SDL_FreeSurface(surface);
        return ge;
    }

    SDL_Rect quad;
    SDL_Texture* tex;
};

struct GraphicChunk
{
    std::vector<GraphicElement*> elements;

    void Draw(SDL_Renderer* renderer)
    {
        for (auto ge : elements)
        {
            SDL_RenderCopy(renderer, ge->tex, NULL, &ge->quad);
        }
    }

    void DrawAt(SDL_Renderer* renderer, const SDL_Point pos)
    {
        for (auto ge : elements)
        {
            SDL_Rect quad = { ge->quad.x + pos.x, ge->quad.y + pos.y, ge->quad.w, ge->quad.h };
            SDL_RenderCopy(renderer, ge->tex, NULL, &quad);
        }
    }

    void Clear()
    {
        for (auto ge : elements)
        {
            SDL_DestroyTexture(ge->tex);
            delete ge;
        }
        elements.clear();
    }
};
