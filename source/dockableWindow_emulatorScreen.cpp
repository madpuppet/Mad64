#include "common.h"
#include "dockableWindow_emulatorScreen.h"
#include "uiItem_textButton.h"

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

	if (m_capturingInput)
	{
		SDL_Point points[5] = { {area.x,area.y}, {area.x + area.w - 1,area.y}, {area.x + area.w,area.y + area.h - 1}, {area.x, area.y + area.h - 1}, {area.x, area.y} };
		SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
		SDL_RenderDrawLines(r, points, 5);
	}

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
	m_titleIconsLeft.push_back(new UIItem_TextButton("Cold", DELEGATE(DockableWindow_EmulatorScreen::OnColdPress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Reset", DELEGATE(DockableWindow_EmulatorScreen::OnResetPress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Play", DELEGATE(DockableWindow_EmulatorScreen::OnPlayPress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Cyc", DELEGATE(DockableWindow_EmulatorScreen::OnSingleCyclePress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Ins", DELEGATE(DockableWindow_EmulatorScreen::OnSingleInstructionPress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Row", DELEGATE(DockableWindow_EmulatorScreen::OnSingleRowPress)));
	m_titleIconsLeft.push_back(new UIItem_TextButton("Frm", DELEGATE(DockableWindow_EmulatorScreen::OnSingleFramePress)));

	int zoomButtonWidth = gApp->GetWhiteSpaceWidth() * 6;
	vector<string> modes = { "Auto", "x1", "x2", "x4", "x8" };
	m_zoomButton = new UIItem_EnumButton(0, modes, zoomButtonWidth, DELEGATE(DockableWindow_EmulatorScreen::OnZoomChanged));
	m_titleIconsRight.push_back(m_zoomButton);
}

void DockableWindow_EmulatorScreen::OnZoomChanged(int option)
{
	int zoomLevel[] = { 0, 1, 2, 4, 8 };
	m_zoomLevel = zoomLevel[option];
}

void DockableWindow_EmulatorScreen::OnSingleCyclePress()
{
	gApp->DoEmuSingleCycle();
}
void DockableWindow_EmulatorScreen::OnSingleInstructionPress()
{
	gApp->DoEmuSingleInstruction();
}
void DockableWindow_EmulatorScreen::OnSingleRowPress()
{
	gApp->DoEmuSingleRow();
}
void DockableWindow_EmulatorScreen::OnSingleFramePress()
{
	gApp->DoEmuSingleFrame();
}

void DockableWindow_EmulatorScreen::OnColdPress()
{
	gApp->DoEmuColdReset();
	if (!gApp->IsEmulatorRunning())
		gApp->DoEmuTogglePlay();
}

void DockableWindow_EmulatorScreen::OnResetPress()
{
	gApp->DoEmuResetAndPlay();
}

void DockableWindow_EmulatorScreen::OnPlayPress()
{
	gApp->DoEmuTogglePlay();
}

void DockableWindow_EmulatorScreen::OnMouseButtonDown(int button, int x, int y)
{
	gApp->SetCaptureKeyInput(DELEGATE(DockableWindow_EmulatorScreen::OnCapturedKeyInput));
	gApp->SetCaptureTextInput(DELEGATE(DockableWindow_EmulatorScreen::OnCapturedTextInput));
	m_capturingInput = true;

    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_EmulatorScreen::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}

void DockableWindow_EmulatorScreen::OnCapturedKeyInput(bool lostCapture, bool keyDown, u32 sym, u32 mod)
{
	if (lostCapture)
	{
		m_capturingInput = false;
	}
	else if (keyDown)
	{
		gApp->GetEmulator()->OnKeyDown(sym, mod);
	}
	else
	{
		gApp->GetEmulator()->OnKeyUp(sym, mod);
	}
}



