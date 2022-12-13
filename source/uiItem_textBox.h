#pragma once

#include "uiItem.h"

class UIItem_TextBox : public UIItem
{
public:
    UIItem_TextBox(const string& title, const string& hintText, int boxWidth, const TextHook& onChange, const TextHook& onEnter)
    {
        m_titleText = title;
        m_hintText = hintText;
        m_boxWidth = boxWidth;
        m_onEnter = onEnter;
        m_onChange = onChange;
        m_geTitleText = nullptr;
        m_geHintText = nullptr;
        m_geText = nullptr;

        m_cursorPos = 0;
        m_isSelected = false;
    }

    ~UIItem_TextBox();

    // virtuals
    void Draw(SDL_Renderer* renderer);
    void OnButtonDown(int button, int x, int y);
    void OnButtonUp(int button, int x, int y);
    int GetWidth();
    int GetHeight();
    void OnRendererChange(SDL_Renderer* r);
    void SetPos(int x, int y);
    bool Overlaps(int x, int y);

protected:
    void BuildGE(SDL_Renderer* r);
    void OnCapturedTextInput(bool lostCapture, const string& text);
    void OnCapturedKeyInput(bool lostCapture, u32 sym, u32 mod);

    string m_titleText;
    string m_hintText;
    string m_text;
    int m_boxWidth;

    TextHook m_onEnter;
    TextHook m_onChange;

    GraphicElement* m_geTitleText;
    GraphicElement* m_geHintText;
    GraphicElement* m_geText;

    SDL_Rect m_area;

    bool m_isSelected;
    int m_cursorPos;
    float m_cursorAnim;
};



