#include "common.h"
#include "dockableWindow_emulatorScreen.h"

#define BORDER_SIZE 2

int DockableWindow_EmulatorScreen::GetScreenRenderWidth()
{
	if (m_zoomLevel == 0)
		return m_contentArea.w;
	else
	{
		auto vic = gApp->GetEmulator()->GetVic();
		return vic->GetScreenWidth() * m_zoomLevel;
	}
}
int DockableWindow_EmulatorScreen::GetScreenRenderHeight()
{
	auto vic = gApp->GetEmulator()->GetVic();
	int vic_w = vic->GetScreenWidth();
	int vic_h = vic->GetScreenHeight();
	if (m_zoomLevel == 0)
		return (int)((float)m_contentArea.w / vic_w * vic_h);
	else
		return vic_h * m_zoomLevel;
}

int DockableWindow_EmulatorScreen::GetContentHeight()
{
	auto settings = gApp->GetSettings();
	return GetScreenRenderHeight() + BORDER_SIZE * 2;
}

int DockableWindow_EmulatorScreen::GetContentWidth()
{
	return GetScreenRenderWidth() + BORDER_SIZE * 2;
}

void DockableWindow_EmulatorScreen::DrawChild()
{
	auto settings = gApp->GetSettings();
	auto r = GetRenderer();
	auto vic = gApp->GetEmulator()->GetVic();
	auto tex = vic->FlushTexture();
	SDL_Rect area = { m_contentArea.x + BORDER_SIZE - m_horizScroll, m_contentArea.y + BORDER_SIZE - m_vertScroll, GetScreenRenderWidth(), GetScreenRenderHeight() };
	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
	SDL_RenderCopy(r, tex, nullptr, &area);

	if (!gApp->IsEmulatorRunning())
	{
		// draw flashing box at current raster line / raster byte
#if 0
		int rasterLine = vic->CurrentRasterLine();
		int rasterCol = vic->CurrentRasterRow();
		int sx = 8 * m_emulatorZoom;
		int sy = 2;
		int xx = item.area.x + rasterCol * 8 * m_emulatorZoom;
		int yy = item.area.y + rasterLine * m_emulatorZoom;
		int brightness = (int)(sinf(m_markerAnim * 3.1452f * 2) * 120) + 128;
		SDL_SetRenderDrawColor(r, brightness, brightness, brightness, brightness);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_RenderDrawLine(r, xx, yy - sy, xx + sx, yy - sy);
		SDL_RenderDrawLine(r, xx + sx, yy - sy, xx + sx, yy + sy);
		SDL_RenderDrawLine(r, xx + sx, yy + sy, xx, yy + sy);
		SDL_RenderDrawLine(r, xx, yy + sy, xx, yy - sy);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
#endif
	}
}

void DockableWindow_EmulatorScreen::OnChildRendererChange()
{
	auto vic = gApp->GetEmulator()->GetVic();
	vic->RecreateTexture(GetRenderer());
}

void DockableWindow_EmulatorScreen::CreateChildIcons()
{

}

void DockableWindow_EmulatorScreen::OnMouseButtonDown(int button, int x, int y)
{
    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_EmulatorScreen::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}



