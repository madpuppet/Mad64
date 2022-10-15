#include "common.h"
#include "uiItem_container.h"

void UIItem_Container::OnPress(int x, int y)
{
}

void UIItem_Container::OnRelease(int x, int y) 
{
}

void UIItem_Container::OnMouseMove(int x, int y) 
{
}

void UIItem_Container::Add(UIItem* item, float x1, float y1, float x2, float y2, int flags)
{
	ItemDesc desc;
	desc.item = item;
	desc.x1 = x1;
	desc.x2 = x2;
	desc.y1 = y1;
	desc.y2 = y2;
	desc.flags = flags;
	m_items.push_back(desc);
}

void UIItem_Container::OnDraw()
{
	for (auto& desc : m_items)
	{
		desc.item->OnDraw();
	}
}

void UIItem_Container::OnResize()
{
	for (auto& desc : m_items)
	{
		float x1 = (desc.flags & X1_REL) ? (m_area.x + m_area.w * desc.x1) : desc.x1;
		float x2 = (desc.flags & X2_REL) ? (m_area.x + m_area.w * desc.x2) : desc.x2;
		float y1 = (desc.flags & Y1_REL) ? (m_area.y + m_area.h * desc.y1) : desc.y1;
		float y2 = (desc.flags & Y2_REL) ? (m_area.y + m_area.h * desc.y2) : desc.y2;
		SDL_Rect rect;
		rect.x = (int)x1;
		rect.y = (int)y1;
		rect.w = (int)(x2 - x1);
		rect.h = (int)(y2 - y1);
		desc.item->SetArea(rect);
	}
}

