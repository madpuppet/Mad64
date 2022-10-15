#pragma once

class UIItem
{
public:
	UIItem(const SDL_Rect& area, void* userData) { m_userData = userData; m_area = area; m_preferredSize = { m_area.w,m_area.h }; }
	UIItem(void* userData) { m_userData = userData; m_area = { 0,0,100,100 }; m_preferredSize = { m_area.w,m_area.h }; }
	virtual ~UIItem() {}

	virtual void OnPress(int x, int y) {}
	virtual void OnRelease(int x, int y) {}
	virtual void OnMouseMove(int x, int y) {}
	virtual void OnResize() {}
	virtual void OnDraw() {}

	void InitArea(const SDL_Rect& area)
	{
		m_area = area;
		m_preferredSize = { area.w, area.h };
	}

	virtual void SetArea(const SDL_Rect& area)
	{
		m_area = area;
		OnResize();
	}
	const SDL_Rect& GetArea() { return m_area; }
	const SDL_Point& GetPreferredSize() { return m_preferredSize; }
	void* GetUserData() { return m_userData; }

protected:
	void* m_userData;
	SDL_Rect m_area;				// current area (from last SetArea() call)
	SDL_Point m_preferredSize;		// preferred size - used to assist parent when it needs to resize this item
};
typedef DMFastDelegate::FastDelegate1<UIItem*> UIItemCallback;

