#pragma once

#include <DMFastDelegate.h>

class TextInput
{
public:
	TextInput(int x, int y, const char *text, DMFastDelegate::FastDelegate1<const string &> onEnter);
	~TextInput();

	void SetPos(int x, int y)
	{
		m_pos.x = x;
		m_pos.y = y;
		Visualize();
	}
	SDL_Rect& GetArea() { return m_area; }

	void SetOnEnter(DMFastDelegate::FastDelegate1<const string &> onEnter) 
	{
		m_onEnter = onEnter;
	}

	enum FlashMode
	{
		MODE_Found,
		MODE_NotFound,
		MODE_Activated
	};

	void SetActive(bool active) { m_isActive = active; m_cursorAnim = 0; if (active) Flash(MODE_Activated); }
	void Update();
	void Draw();
	void OnKeyDown(SDL_Event* e);
	void Visualize();
	void Flash(FlashMode mode) { m_flashTime = 1.0f; m_flashMode = mode; }

protected:
	string m_title;
	float m_flashTime;
	FlashMode m_flashMode;

	bool m_isActive;
	string m_text;
	int m_cursorPos;
	float m_cursorAnim;
	SDL_Point m_pos;
	SDL_Rect m_area;

	GraphicChunk* m_gc;
	DMFastDelegate::FastDelegate1<const string &> m_onEnter;
};
