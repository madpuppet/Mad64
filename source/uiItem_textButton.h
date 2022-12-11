#pragma once
#include "uiItem.h"

class UIItem_TextButton : public UIItem
{
public:
    UIItem_TextButton(const string& text, const ButtonPressHook& onPress)
    {
        m_text = text;
        m_onButtonPress = onPress;
        m_geButtonText = nullptr;
    }

    ~UIItem_TextButton();
    void Draw(SDL_Renderer* renderer);
    void OnButtonDown(int button, int x, int y);
    void OnButtonUp(int button, int x, int y);
    int GetWidth();
    int GetHeight();
    void OnRendererChange(SDL_Renderer* r);
    void SetPos(int x, int y);
    void SetArea(const SDL_Rect& area);
    bool Overlaps(int x, int y)
    {
        return Contains(m_area, x, y);
    }

protected:
    void BuildGE(SDL_Renderer* r);

    string m_text;
    ButtonPressHook m_onButtonPress;
    GraphicElement* m_geButtonText;
    SDL_Rect m_area;
};
