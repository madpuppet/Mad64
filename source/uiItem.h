#pragma once

class UIItem
{
public:
    UIItem() {}
    virtual ~UIItem() {}

    virtual void Draw(SDL_Renderer* renderer) = 0;
    virtual void OnButtonDown(int button, int x, int y) {}
    virtual void OnButtonUp(int button, int x, int y) {}
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual void OnRendererChange(SDL_Renderer* r) = 0;
    virtual void SetPos(int x, int y) = 0;
    virtual bool Overlaps(int x, int y) = 0;
};

