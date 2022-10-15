#pragma once

// simple container - each child is given a relative part of this container's rectangle
class UIItem_Container : public UIItem
{
public:
	UIItem_Container(const SDL_Rect& area, void* userData) : UIItem(area, userData) {}

	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnPress(int x, int y);
	virtual void OnRelease(int x, int y);
	virtual void OnMouseMove(int x, int y);

	void Add(UIItem* item, float x1, float y1, float x2, float y2, int flags);

	enum ItemFlags
	{
		X1_REL = 1,
		X2_REL = 2,
		Y1_REL = 4,
		Y2_REL = 8
	};

protected:
	struct ItemDesc
	{
		UIItem* item;
		float x1, y1, x2, y2;
		int flags;
	};
	vector<ItemDesc> m_items;
};

