#pragma once
#include "uiItem.h"

class UIItem_EnumButton : public UIItem
{
public:
    UIItem_EnumButton(int initialOption, const vector<string> &options, int fixedWidth, const EnumChangeHook& onChange)
    {
        m_options = options;
        m_onChange = onChange;
//        m_geButtonText = nullptr;
        m_currentOption = initialOption;
        m_fixedWidth = fixedWidth;
    }

    ~UIItem_EnumButton();
    void Draw(SDL_Renderer* renderer);
    void OnButtonDown(int button, int x, int y);
    void OnButtonUp(int button, int x, int y);
    int GetWidth();
    int GetHeight();
    void OnRendererChange(SDL_Renderer* r);
    void SetPos(int x, int y);
    void SetArea(const SDL_Rect& area);
    void UpdateCursor(int x, int y);
    
    bool Overlaps(int x, int y)
    {
        return Contains(m_area, x, y);
    }

protected:
    void BuildGE(SDL_Renderer* r);

    int m_currentOption;
    int m_fixedWidth;
    vector<string> m_options;
    EnumChangeHook m_onChange;
//    GraphicElement* m_geButtonText;
    SDL_Rect m_area;
    bool m_highlight;
};




