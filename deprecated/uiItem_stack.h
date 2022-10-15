#pragma once

// item stack - each item is placed beside the previous
class UIItem_Stack : public UIItem
{
public:
	UIItem_Stack(bool horizontal, const SDL_Rect& area, void* userData) : UIItem(area, userData)
	{
		m_horizontal = horizontal;
	}

	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnPress(int x, int y) {}
	virtual void OnRelease(int x, int y) {}
	virtual void OnMouseMove(int x, int y) {}

	void Add(UIItem* item, float x1, float y1, float x2, float y2, int flags);

protected:
	bool m_horizontal;
	vector<UIItem*> m_items;
};

