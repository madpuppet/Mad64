#pragma once

class GraphicElement
{
public:
    GraphicElement(SDL_Texture* tex, i32 x, i32 y, bool ownTexture);
    ~GraphicElement();

    static GraphicElement* CreateFromImage(const char* path, i32 x, i32 y);
    static GraphicElement* CreateFromTexture(SDL_Texture* tex, i32 x, i32 y, bool ownTexture);
    static GraphicElement* CreateFromText(TTF_Font* font, const char* text, const SDL_Color& col, i32 x, i32 y);

    void SetPos(i32 x, i32 y) { m_quad.x = x; m_quad.y = y; }
    void SetText(TTF_Font* font, const char* text, const SDL_Color& col);
    void SetTexture(SDL_Texture *tex, bool ownTexture);

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
    ~GraphicChunk();

protected:
    std::vector<GraphicElement*> elements;
};



