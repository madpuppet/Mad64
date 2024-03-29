#pragma once

class GraphicElement
{
public:
    GraphicElement(SDL_Texture* tex, i32 x, i32 y, bool ownTexture);
    ~GraphicElement();

    static GraphicElement* CreateFromImage(SDL_Renderer* r, const char* path, i32 x, i32 y);
    static GraphicElement* CreateFromTexture(SDL_Texture* tex, i32 x, i32 y, bool ownTexture);

    void SetPos(i32 x, i32 y) { m_quad.x = x; m_quad.y = y; }
    void SetTexture(SDL_Texture *tex, bool ownTexture);

    void Render(SDL_Renderer *r)
    {
        SDL_RenderCopy(r, m_tex, 0, &m_quad);
    }

    void RenderAt(SDL_Renderer* r, int x, int y)
    {
        SDL_Rect rect = { x, y, m_quad.w, m_quad.h };
        SDL_RenderCopy(r, m_tex, 0, &rect);
    }

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() { return m_enabled; }
    const SDL_Rect& GetRect() { return m_quad; }
    SDL_Texture* GetTexture() { return m_tex; }

protected:
    bool m_enabled;         // element won't be rendered if not enabled
    bool m_ownTexture;      // the texture is owned by this element and will be destroyed if a new texture is assigned, or the element is destroyed

    SDL_Rect m_quad;
    SDL_Texture *m_tex;
};

class GraphicChunk
{
public:
    void Add(GraphicElement* e) { elements.push_back(e); }
    void Draw();
    void DrawAt(i32 x, i32 y);
    void Clear();
    bool IsEmpty() { return elements.empty(); }
    int CalcMaxWidth();

    int Size() { return (int)elements.size(); }
    void DrawElemAt(int i, i32 x, i32 y);

    ~GraphicChunk();

protected:
    std::vector<GraphicElement*> elements;
};



