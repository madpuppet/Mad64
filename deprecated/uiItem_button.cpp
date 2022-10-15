#include "common.h"
#include "uiItem_button.h"

UIItem_Button::UIItem_Button(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Texture* frame, i32 x, i32 y, const UIItemCallback& callback, void* userData) : UIItem(userData)
{
	m_onPressed = callback;
	m_geBorder = GraphicElement::CreateFromTexture(frame, x, y, false);

	SDL_Color col = { 255,255,255,255 };
	m_geText = GraphicElement::CreateFromText(font, text, col, 0, 0);

	// center the text
	i32 frame_w, frame_h;
	SDL_QueryTexture(frame, NULL, NULL, &frame_w, &frame_h);
	i32 text_w, text_h;
	SDL_QueryTexture(frame, NULL, NULL, &text_w, &text_h);
	i32 tx = x + frame_w / 2 - text_w / 2;
	i32 ty = y + frame_h / 2 - text_h / 2;
	m_geText->SetPos(tx, ty);


}

void UIItem_Button::OnPress(int x, int y)
{
}

void UIItem_Button::OnRelease(int x, int y)
{
}

void UIItem_Button::OnDraw()
{
	auto r = gApp->GetRenderer();
	SDL_RenderCopy(r, m_geBorder->GetTexture(), NULL, &m_geBorder->GetRect());
	SDL_RenderCopy(r, m_geText->GetTexture(), NULL, &m_geText->GetRect());
}


