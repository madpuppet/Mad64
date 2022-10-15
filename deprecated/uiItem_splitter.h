#pragma once

// 2 children and a bar in the middle that can be dragged up/down
class UIItem_Splitter : public UIItem
{
public:
	UIItem_Splitter(const SDL_Rect & area, void* userData) : UIItem(area, userData) {}

	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnPress(int x, int y);
	virtual void OnRelease(int x, int y);
	virtual void OnMouseMove(int x, int y);

	void SetHorizontalSplit(bool horizontal);
	void SetChild(int index, UIItem* item);
	void SetSize(int index, int size);

protected:
	bool m_horizontalSplit;
	struct Split
	{
		UIItem* item;
		int start;
	};
	vector<Split> m_splits;
};
