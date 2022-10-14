#pragma once

#define BUTTON_IMAGE "data/button_frame"

class UIItem
{
public:
	UIItem(void* userData) { m_userData = userData; }
	virtual ~UIItem() {}

	virtual void OnPress(int x, int y) {}
	virtual void OnRelease(int x, int y) {}
	virtual void OnMouseMove(int x, int y) {}
	virtual void OnResize() {}
	virtual void OnDraw() {}

	virtual void SetArea(const SDL_Rect& area)
	{ 
		m_area = area; 
		OnResize(); 
	}
	const SDL_Rect& GetArea() { return m_area; }
	void* GetUserData() { return m_userData; }

protected:
	void* m_userData;
	SDL_Rect m_area;
};
typedef DMFastDelegate::FastDelegate1<UIItem*> UIItemCallback;

// 2 children and a bar in the middle that can be dragged up/down
class UIItem_Split : public UIItem
{
public:
	virtual void OnDraw();
	virtual void OnResize();
	virtual void OnPress(int x, int y) {}
	virtual void OnRelease(int x, int y) {}
	virtual void OnMouseMove(int x, int y) {}

	void SetHorizontalSplit(bool horizontal);
	void SetChild(int index, UIItem* item);
	void SetSize(int index, int size);

protected:
	bool m_horizontalSplit;
	UIItem* m_child[2];
	int m_childSize;
};

// simple container - each child is given a relative part of this container's rectangle
class UIItem_RelativeContainer : public UIItem
{
};


class UIItem_Button : public UIItem
{
public:
	UIItem_Button(SDL_Renderer *renderer, TTF_Font* font, const char* text, SDL_Texture* frame, i32 x, i32 y, const UIItemCallback& callback, void *userData);

	virtual void OnPress(int x, int y);
	virtual void OnRelease(int x, int y);
	virtual void OnDraw();

protected:
	GraphicElement* m_geBorder;
	GraphicElement* m_geText;
	UIItemCallback m_onPressed;

	bool m_isDown;
};


class UIManager
{
public:
	UIManager();
	~UIManager();

	void Draw();

protected:
	vector<UIItem*> m_items;
	SDL_Texture* m_buttonTex;

	// events
	void OnLoadPressed(UIItem* item) {}
};

