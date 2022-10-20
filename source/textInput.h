#pragma once

#include <DMFastDelegate.h>

class TextInput
{
public:
	TextInput(DMFastDelegate::FastDelegate1<const string &> onEnter);
	~TextInput();

	void SetArea(const SDL_Rect& rect)
	{
		m_area = rect;
	}
	SDL_Rect& GetArea() { return m_area; }

	void SetOnEnter(DMFastDelegate::FastDelegate1<const string &> onEnter) 
	{
		m_onEnter = onEnter;
	}

	void SetActive(bool active) { m_isActive = active; m_cursorAnim = 0; }
	void Update();
	void Draw();
	void OnKeyDown(SDL_Event* e);
	void Visualize();

protected:
	bool m_isActive;
	string m_text;
	int m_cursorPos;
	float m_cursorAnim;
	SDL_Rect m_area;

	GraphicChunk* m_gc;
	DMFastDelegate::FastDelegate1<const string &> m_onEnter;
};
