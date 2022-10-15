#pragma once

class UIItem_Button : public UIItem
{
public:
	UIItem_Button(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Texture* frame, i32 x, i32 y, const UIItemCallback& callback, void* userData);

	virtual void OnPress(int x, int y);
	virtual void OnRelease(int x, int y);
	virtual void OnDraw();

protected:
	GraphicElement* m_geBorder;
	GraphicElement* m_geText;
	UIItemCallback m_onPressed;

	bool m_isDown;
};

