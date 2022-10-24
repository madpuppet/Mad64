#pragma once

class SARWindow
{
public:
	SARWindow();
	~SARWindow();

	void SetRect(const SDL_Rect& area) { m_area = area; }

	void Update();
	void Draw();

protected:
	SDL_Rect m_area;
};

