#include "common.h"
#include "editWindow.h"
#include "dockableWindow_log.h"
#include "dockableWindow_emulatorScreen.h"
#include "dockableWindow_searchAndReplace.h"

#define TAB_ACTIVE_IMAGE "data/button_frame"
#define TAB_INACTIVE_IMAGE "data/button_frame"

EditWindow::EditWindow()
{
	auto r = gApp->GetRenderer();
	SDL_Surface* surface = SDL_LoadBMP(TAB_ACTIVE_IMAGE);
	m_tabActiveTex = SDL_CreateTextureFromSurface(r, surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP(TAB_ACTIVE_IMAGE);
	m_tabInactiveTex = SDL_CreateTextureFromSurface(r, surface);
	SDL_FreeSurface(surface);

	m_activeSourceFileItem = nullptr;
	m_cursorAnimTime = 0.0f;
	m_dragMode = DRAG_None;
	m_dragOffset = 0;
	m_autoScroll = 0;
	m_autoScroll_mouseX = 0;
	m_autoScroll_mouseY = 0;
	m_marked = false;
	m_keyMarking = false;
	m_mouseMarking = false;
	m_shiftDown = false;
	m_markStartLine = 0;
	m_markStartColumn = 0;
	m_markEndLine = 0;
	m_markEndColumn = 0;

	m_mouseX = 0;
	m_mouseY = 0;

	CalcRects();
	InitStatus();
}

void DrawColouredLine(int x, int y1, int y2, bool highlighted)
{
	auto r = gApp->GetRenderer();

	SDL_Color colEdge = { 0,0,32,255 };
	SDL_Color colCenter = { 0,0,0,255 };
	SDL_Color highlightedCol = { 255,255,0,255 };

	SDL_Color* c1 = highlighted ? &highlightedCol : &colEdge;
	SDL_Color* c2 = highlighted ? &highlightedCol : &colCenter;

	SDL_SetRenderDrawColor(r, c1->r, c1->g, c1->b, c1->a);
	SDL_RenderDrawLine(r, x-1, y1, x-1, y2);
	SDL_RenderDrawLine(r, x+1, y1, x+1, y2);
	SDL_SetRenderDrawColor(r, c2->r, c2->g, c2->b, c2->a);
	SDL_RenderDrawLine(r, x, y1, x, y2);
}

void EditWindow::ClearVisuals()
{
	delete m_status.m_ge;
	m_status.m_ge = nullptr;

	for (auto sfi : m_fileTabs)
	{
		delete sfi->geText;
		sfi->geText = GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), sfi->file->GetName().c_str(), { 255,255,255,255 }, 0, 0);
	}
	LayoutTabs();
	CalcRects();
}

bool IsEmulationAtLine(CompilerSourceInfo *csi, int line)
{
	if (csi && csi->m_lines.size() > line)
	{
		int emulatorAddr = gApp->GetEmulator()->GetCurrentPC();
		auto cli = csi->m_lines[line];
		u16 memStart = cli->memAddr;
		u16 memEnd = memStart + (u16)cli->data.size();
		return (emulatorAddr >= memStart && emulatorAddr < memEnd&& cli->data.size() > 0);
	}
	return false;
}

void EditWindow::Draw()
{
	auto r = gApp->GetRenderer();
	auto settings = gApp->GetSettings();
	SourceFile* file = m_activeSourceFileItem ? m_activeSourceFileItem->file : nullptr;
	CompilerSourceInfo* csi = file ? file->GetCompileInfo() : nullptr;

	// draw file tabs
	SDL_Color highlight = { 255, 255, 255, 255 };
	SDL_Color inactive = { 64, 84, 100, 255 };

	int windowWidth, windowHeight;
	SDL_GetRendererOutputSize(gApp->GetRenderer(), &windowWidth, &windowHeight);

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &m_titleTabsRect);
	for (auto sfi : m_fileTabs)
	{
		SDL_Color fore, back;
		if (sfi == m_activeSourceFileItem)
		{
			back = { 64,64,128,255 };
			if (sfi->file->IsDirty())
				fore = { 255,100,100,255 };
			else
				fore = { 255,255,255,255 };
		}
		else
		{
			back = { 32,32,64,255 };
			if (sfi->file->IsDirty())
				fore = { 128,64,64,255 };
			else
				fore = { 96,96,96,255 };
		}
		SDL_Rect border = sfi->geText->GetRect();
		SDL_SetRenderDrawColor(r, back.r, back.g, back.b, back.a);
		SDL_RenderFillRect(r, &border);
		SDL_SetTextureColorMod(sfi->geText->GetTexture(), fore.r, fore.g, fore.b);
		SDL_RenderCopy(r, sfi->geText->GetTexture(), NULL, &sfi->geText->GetRect());
	}

	// draw lines
	m_renderedContentWidth = 8;
	m_renderedContentHeight = 8;
	if (m_activeSourceFileItem)
	{
		int startLine = max(0, m_activeSourceFileItem->vertScroll / settings->lineHeight);
		int endLine = min((m_sourceEditRect.h + m_activeSourceFileItem->vertScroll) / settings->lineHeight + 1, (int)file->GetLines().size());
		m_renderedContentHeight = settings->lineHeight * (int)file->GetLines().size();

		// draw addresses
		{
			ClipRectScope crs(r, &m_memAddrRect);
			int sourceVersion = file->GetSourceVersion();
			u16 emulatorAddr = gApp->GetEmulator()->GetCurrentPC();
			for (int i = startLine; i < endLine; i++)
			{
				int brighten = (m_activeSourceFileItem->activeLine == i) ? 16 : 0;
				int y = m_memAddrRect.y + i * settings->lineHeight - m_activeSourceFileItem->vertScroll;
				auto gc = csi ? csi->GetMemAddrGC(i) : nullptr;
				bool emulating = IsEmulationAtLine(csi, i);
				u8 breakpoint = file->GetLines()[i]->GetBreakpoint();
				if (settings->renderLineBackgrounds || brighten || emulating)
				{
					SDL_Rect lineQuad = { m_memAddrRect.x, y, m_memAddrRect.w, settings->lineHeight };
					int red, green, blue;
					if (emulating)
					{
						red = 96 + brighten;
						green = 32 + brighten;
						blue = 96 + brighten;
					}
					else
					{
						red = brighten;
						green = brighten;
						blue = brighten + 128 - ((i & 1) ? 16 : 0);
					}
					SDL_SetRenderDrawColor(r, red, green, blue, 255);
					SDL_RenderFillRect(r, &lineQuad);
				}
				if (gc)
					gc->DrawAt(m_memAddrRect.x + settings->textXMargin, y + settings->textYMargin);
				if (breakpoint)
				{
					SDL_Rect lineQuad = { m_memAddrRect.x, y, m_memAddrRect.w, settings->lineHeight };
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
					SDL_SetRenderDrawColor(r, 255, 0, 0, 128);
					SDL_RenderFillRect(r, &lineQuad);
				}
			}
		}

		// draw decode
		{
			ClipRectScope src(r, &m_decodeRect);
			for (int i = startLine; i < endLine; i++)
			{
				int brighten = (m_activeSourceFileItem->activeLine == i) ? 16 : 0;
				auto gc = csi ? csi->GetDecodeGC(i) : nullptr;
				int y = m_decodeRect.y + i * settings->lineHeight - m_activeSourceFileItem->vertScroll;
				SDL_Rect lineQuad = { m_decodeRect.x, y, m_decodeRect.w, settings->lineHeight };
				bool emulating = IsEmulationAtLine(csi, i);
				if (settings->renderLineBackgrounds || brighten || emulating)
				{
					int red, green, blue;
					if (emulating)
					{
						red = 96 + brighten;
						green = 32 + brighten;
						blue = 96 + brighten;
					}
					else
					{
						red = brighten;
						green = brighten;
						blue = brighten + 128 - ((i & 1) ? 16 : 0);
					}
					SDL_SetRenderDrawColor(r, red, green, blue, 255);
					SDL_RenderFillRect(r, &lineQuad);
				}
				if (gc)
					gc->DrawAt(m_decodeRect.x + settings->textXMargin, y + settings->textYMargin);
			}
		}

		// draw text
		{
			ClipRectScope crs(r, &m_sourceEditRect);
			int branchDepth = 0;
			m_activeSourceFileItem->editWindowTextWidth = 0;
			for (int i = startLine; i < endLine; i++)
			{
				int brighten = (m_activeSourceFileItem->activeLine == i) ? 32 : 0;
				auto line = file->GetLines()[i];
				int y = m_sourceEditRect.y + i * settings->lineHeight - m_activeSourceFileItem->vertScroll;
				int lineWidth = line->GetLineWidth() + settings->textXMargin;
				if (settings->renderLineBackgrounds || brighten)
				{
					if (line->GetChars().empty())
					{
						SDL_Rect lineQuad = { m_sourceEditRect.x, y, m_sourceEditRect.w, settings->lineHeight };
						SDL_SetRenderDrawColor(r, brighten, brighten, brighten + 80 - ((i & 1) ? 8 : 0), 255);
						SDL_RenderFillRect(r, &lineQuad);
					}
					else
					{
						int red, green, blue;
						red = brighten;
						green = brighten;
						blue = brighten + 128 - ((i & 1) ? 16 : 0);
						SDL_Rect lineQuad1 = { m_activeXPosText - m_horizScroll, y, lineWidth, settings->lineHeight };
						SDL_Rect lineQuad2 = { m_activeXPosText - m_horizScroll + lineWidth, y, m_sourceEditRect.w - lineWidth + m_activeSourceFileItem->horizScroll, settings->lineHeight };
						SDL_SetRenderDrawColor(r, red, green, blue, 255);
						SDL_RenderFillRect(r, &lineQuad1);
						if (lineWidth < m_sourceEditRect.w + m_horizScroll)
						{
							SDL_SetRenderDrawColor(r, brighten, brighten, brighten + 80 - ((i & 1) ? 8 : 0), 255);
							SDL_RenderFillRect(r, &lineQuad2);
						}
					}
				}

				// draw marking
				int markStartCol, markEndCol;
				if (CheckLineMarked(i, markStartCol, markEndCol))
				{
					int startX1, startX2, endX1, endX2;
					line->GetCharX(markStartCol, startX1, startX2);
					line->GetCharX(markEndCol, endX1, endX2);
					SDL_Rect markedRect = { m_sourceEditRect.x + settings->textXMargin + startX1 - m_horizScroll, y, endX2 - startX1, settings->lineHeight };
					SDL_SetRenderDrawColor(r, 128, 128, 128, 255);
					SDL_RenderFillRect(r, &markedRect);
				}

				// search highlight
				if (gApp->GetWindowSearchAndReplace()->ContainsLine(i))
				{
					auto& search = gApp->GetWindowSearchAndReplace()->GetSearchText();
					if (!search.empty())
					{
						auto& str = line->GetChars();
						size_t off = 0;
						while (true)
						{
							off = StrFind(str, search, off);
							if (off == string::npos)
								break;

							int x1, x2, stub;
							line->GetCharX((int)off, x1, stub);
							line->GetCharX((int)off + (int)search.size() - 1, stub, x2);
							SDL_Rect markedRect = { m_sourceEditRect.x + settings->textXMargin + x1 - m_horizScroll, y, x2 - x1, settings->lineHeight };
							SDL_SetRenderDrawColor(r, 128, 128, 0, 255);
							SDL_RenderFillRect(r, &markedRect);

							off += search.size();
						}
					}
				}

				// - text
				if (line->GetGCText())
				{
					int lineStartX = m_activeXPosText + settings->textXMargin - m_horizScroll;
					int lineStartY = y + settings->textYMargin;
					int maxWidth = line->GetGCText()->CalcMaxWidth();
					line->GetGCText()->DrawAt(lineStartX, lineStartY);
					m_activeSourceFileItem->editWindowTextWidth = SDL_max(m_activeSourceFileItem->editWindowTextWidth, maxWidth);
					if (m_horizScroll == 0)
						m_activeSourceFileItem->editWindowHScrollWidth = 0;
					m_activeSourceFileItem->editWindowHScrollWidth = SDL_max(m_activeSourceFileItem->editWindowTextWidth, m_activeSourceFileItem->editWindowHScrollWidth);
					m_renderedContentWidth = SDL_max(m_renderedContentWidth, m_activeSourceFileItem->editWindowTextWidth);
				}

				// draw branches
				if (csi && csi->m_lines.size() > i)
				{
					auto cli = csi->m_lines[i];
					if (!cli->error && (cli->addressMode == Cpu6502::AM_Rel))
					{
						int branchAddress = cli->memAddr + (int)cli->operand + 2;
						int branchLine = csi->FindLineByAddress(branchAddress);
						if (branchLine != -1)
						{
							int y1 = y + settings->lineHeight / 2;
							int y2 = m_sourceEditRect.y + branchLine * settings->lineHeight - m_activeSourceFileItem->vertScroll + settings->lineHeight / 2;
							int x1 = m_sourceEditRect.x - 2;
							int x2 = x1 - branchDepth - 10;

							ClipRectScope crs(r, nullptr);
							SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
							SDL_RenderDrawLine(r, x1, y1, x2, y1);
							SDL_RenderDrawLine(r, x2, y1, x2, y2);
							SDL_RenderDrawLine(r, x2, y2, x1, y2);
							SDL_RenderDrawLine(r, x1, y2, x1 - 5, y2 - 5);
							SDL_RenderDrawLine(r, x1, y2, x1 - 5, y2 + 5);
							branchDepth += 4;
						}
					}
				}

				// draw cursor
				if (i == m_activeSourceFileItem->activeLine)
				{
					int cursorX1, cursorX2;
					line->GetCharX(m_activeSourceFileItem->activeColumn, cursorX1, cursorX2);
					int cursorY = m_sourceEditRect.y + m_activeSourceFileItem->activeLine * settings->lineHeight - m_activeSourceFileItem->vertScroll;
					int brightness = max(0, (int)(100 + cosf(m_cursorAnimTime) * 128));
					SDL_Rect cursorRect = { m_sourceEditRect.x + settings->textXMargin + cursorX1 - m_horizScroll, cursorY, settings->overwriteMode ? (cursorX2 - cursorX1) : 2, settings->lineHeight };
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
					SDL_SetRenderDrawColor(r, 255, 255, 255, brightness);
					SDL_RenderFillRect(r, &cursorRect);
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
				}
			}
		}
	}

	// scroll bar area
	CalcScrollBars();

	SDL_SetRenderDrawColor(r, 32, 32, 64, 255);
	SDL_RenderFillRect(r, &m_vertBackArea);
	SDL_RenderFillRect(r, &m_horizBackArea);

	SDL_SetRenderDrawColor(r, 128, 128, 196, 255);
	SDL_RenderFillRect(r, &m_vertBarArea);
	SDL_RenderFillRect(r, &m_horizBarArea);

	// draw search lines
	if (m_activeSourceFileItem)
	{
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 64);
		auto& sfl = gApp->GetWindowSearchAndReplace()->GetFoundLines();
		float maxLines = (float)(m_activeSourceFileItem->file->GetLines().size() - 1);
		for (auto l : sfl)
		{
			float relY = (float)l / maxLines;

			int y = (int)(m_vertBarFullArea.y + (float)(relY * m_vertBarFullArea.h));
			SDL_RenderDrawLine(r, m_vertBarFullArea.x + 1, y, m_vertBarFullArea.x + m_vertBarFullArea.w - 1, y);
		}
	}

//	// - context help
//	gApp->GetLogWindow()->Draw();

	// draw status bar
	DrawStatus();

	// draw context bar split
	SDL_Rect divider = { m_activeXPosContextHelp, settings->lineHeight * 2 - 1, windowWidth - m_activeXPosContextHelp, 3 };
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &divider);

	// draw separator bars
	DrawColouredLine(m_activeXPosDecode, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(m_activeXPosText, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(m_activeXPosContextHelp, m_sourceEditRect.y, m_sourceEditRect.y + m_sourceEditRect.h, false);

	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
}

void EditWindow::OnResize()
{
	// clamp the divides so they are on screen
	auto settings = gApp->GetSettings();
	int windowWidth, windowHeight;
	SDL_GetRendererOutputSize(gApp->GetRenderer(), &windowWidth, &windowHeight);
	settings->xPosContextHelp = min(settings->xPosContextHelp, windowWidth - 16);
	settings->xPosText = min(settings->xPosText, settings->xPosContextHelp-32);
	settings->xPosDecode = min(settings->xPosDecode, settings->xPosText - 16);

	CalcRects();

}

void EditWindow::CalcRects()
{
	int windowWidth, windowHeight;
	SDL_GetRendererOutputSize(gApp->GetRenderer(), &windowWidth, &windowHeight);

	Log("CalcRects: WindowSize %d,%d", windowWidth, windowHeight);

	auto settings = gApp->GetSettings();

	m_titleTabsRect = { 0, 0, windowWidth, settings->lineHeight };
	m_statusRect = { 0, windowHeight - settings->lineHeight, windowWidth, settings->lineHeight };

	int editY = settings->lineHeight;
	int editHeight = windowHeight - settings->lineHeight * 2;
	if (IsActiveAsmFile())
	{
		m_activeXPosDecode = settings->xPosDecode;
		m_activeXPosText = settings->xPosText;
	}
	else
	{
		m_activeXPosDecode = 0;
		m_activeXPosText = 0;
	}
	m_activeXPosContextHelp = settings->xPosContextHelp;
	m_memAddrRect = { 0, editY, m_activeXPosDecode, editHeight };
	m_decodeRect = { m_activeXPosDecode, editY, m_activeXPosText - m_activeXPosDecode, editHeight };

	m_sourceEditRect = { m_activeXPosText, editY, m_activeXPosContextHelp - m_activeXPosText - settings->lineHeight, editHeight - settings->lineHeight };
	m_sourceEditVBarRect = { m_activeXPosContextHelp - settings->lineHeight, editY, settings->lineHeight, editHeight - settings->lineHeight };
	m_sourceEditHBarRect = { m_activeXPosText, editY + editHeight - settings->lineHeight, m_activeXPosContextHelp - m_activeXPosText - settings->lineHeight, settings->lineHeight };

	m_contentArea = m_sourceEditRect;

	m_allEditRect = { 0, editY, m_activeXPosContextHelp, editHeight };
	m_contextHelpRect = { m_activeXPosContextHelp, editY, windowWidth - m_activeXPosContextHelp, editHeight };

	gApp->GetDockableMgr()->SetRect(m_contextHelpRect);

//	gApp->GetLogWindow()->SetRect(m_contextHelpRect);
//	m_searchBox->SetPos(activeXPosContextHelp, settings->lineHeight);
//	m_replaceBox->SetPos(activeXPosContextHelp + 250, settings->lineHeight);
}

bool EditWindow::CalcHorizScrollBar(int& start, int& end)
{
	auto settings = gApp->GetSettings();
	int viewStart = m_activeSourceFileItem->horizScroll;
	int viewEnd = m_activeSourceFileItem->horizScroll + (m_sourceEditRect.w - settings->scrollBarWidth);
	float relStart = SDL_clamp((float)viewStart / (float)m_activeSourceFileItem->editWindowHScrollWidth, 0.0f, 1.0f);
	float relEnd = SDL_clamp((float)viewEnd / (float)m_activeSourceFileItem->editWindowHScrollWidth, 0.0f, 1.0f);
	start = (int)(m_sourceEditRect.x + relStart * (m_sourceEditRect.w - settings->scrollBarWidth));
	end = (int)(m_sourceEditRect.x + relEnd * (m_sourceEditRect.w - settings->scrollBarWidth));
	return relStart > 0.0f || relEnd < 1.0f;
}

bool EditWindow::CalcVertScrollBar(int &start, int &end)
{
	int lineHeight = gApp->GetSettings()->lineHeight;
	start = 0;
	end = 0;
	if (m_activeSourceFileItem)
	{
		int fileHeight = lineHeight * (int)m_activeSourceFileItem->file->GetLines().size();

		int viewStart = m_activeSourceFileItem->vertScroll;
		int viewEnd = m_activeSourceFileItem->vertScroll + m_sourceEditRect.h;
		float relStart = SDL_clamp((float)viewStart / (float)fileHeight, 0.0f, 1.0f);
		float relEnd = SDL_clamp((float)viewEnd / (float)fileHeight, 0.0f, 1.0f);
		start = (int)(m_sourceEditRect.y + relStart * m_sourceEditRect.h);
		end = (int)(m_sourceEditRect.y + relEnd * m_sourceEditRect.h);

		return relStart > 0.0f || relEnd < 1.0f;
	}
	return false;
}


void EditWindow::OnFileLoaded(SourceFile* file)
{
	// add new file tab
	auto sfi = new SourceFileItem();
	sfi->modified = false;
	sfi->file = file;
	sfi->geText = GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), file->GetName().c_str(), { 255,255,255,255 }, 0, 0);
	sfi->activeColumn = 0;
	sfi->activeLine = 0;
	sfi->activeTargetX = 0;
	sfi->editWindowHScrollWidth = 0;
	sfi->editWindowTextWidth = 0;
	sfi->horizScroll = 0;
	sfi->vertScroll = 0;

	m_fileTabs.push_back(sfi);
	LayoutTabs();

	// make new file the active file
	SetActiveFile(file);
}

void EditWindow::ClampActiveLine()
{
	if (m_activeSourceFileItem)
	{
		m_activeSourceFileItem->activeLine = min(m_activeSourceFileItem->activeLine, (int)m_activeSourceFileItem->file->GetLines().size() - 1);
		m_activeSourceFileItem->activeColumn = min(m_activeSourceFileItem->activeColumn, (int)m_activeSourceFileItem->file->GetLines()[m_activeSourceFileItem->activeLine]->GetChars().size());
	}
}

void EditWindow::SetActiveFile(SourceFile* file)
{
	if (file == 0)
	{
		m_activeSourceFileItem = nullptr;
	}
	else
	{
		for (int i=0; i<m_fileTabs.size(); i++)
		{
			if (file == m_fileTabs[i]->file)
			{
				SetActiveFileIdx(i);
				return;
			}
		}
	}
}


void EditWindow::SetActiveFileIdx(int idx)
{
	if (idx >= 0 && idx < m_fileTabs.size())
	{
		if (m_activeSourceFileItem != m_fileTabs[idx])
		{
			m_activeSourceFileItem = m_fileTabs[idx];

			m_activeSourceFileItem->vertScroll;
			m_horizScroll = m_activeSourceFileItem->horizScroll;
			m_targetVertScroll = (float)m_vertScroll;
			m_targetHorizScroll = (float)m_horizScroll;

			ClampActiveLine();

			if (HasExtension(m_activeSourceFileItem->file->GetPath().c_str(), ".asm"))
			{
				gApp->GetCompiler()->Compile(m_activeSourceFileItem->file);
				gApp->GetCompiler()->LogContextualHelp(m_activeSourceFileItem->file, m_activeSourceFileItem->activeLine);
			}
			else
			{
				gApp->GetWindowCompiler()->Clear();
				gApp->GetWindowLabels()->Clear();
				gApp->GetWindowHelp()->Clear();
				gApp->GetWindowRegisters()->Clear();
			}
			CalcRects();

			if (!IsActiveAsmFile())
				gApp->ResetAndStopEmulator();
		}
	}
}

void EditWindow::LayoutTabs()
{
	auto settings = gApp->GetSettings();
	int x = m_titleTabsRect.x + settings->textXMargin;
	int y = m_titleTabsRect.y + settings->textYMargin;
	for (auto sfi : m_fileTabs)
	{
		int w = sfi->geText->GetRect().w;
		sfi->geText->SetPos(x, settings->textYMargin);
		x += w + 16;
	}
}

void EditWindow::OnFileClosed(SourceFile* file)
{
	// if this was the active file,  make a different file active
	for (auto it = m_fileTabs.begin(); it != m_fileTabs.end(); it++)
	{
		auto sfi = *it;
		if (sfi->file == file)
		{
			delete sfi->geText;
			m_fileTabs.erase(it);
			if (m_fileTabs.empty())
				SetActiveFile(nullptr);
			else
				SetActiveFile(m_fileTabs.back()->file);
			LayoutTabs();
			return;
		}
	}
}

void EditWindow::OnMouseWheel(int windowID, int mouseX, int mouseY, int wheelX, int wheelY)
{
	if (Contains(m_allEditRect,m_mouseX, m_mouseY))
	{
		if (m_activeSourceFileItem)
		{
			m_targetVertScroll += (wheelY * -80);
			ClampTargetVertScroll();
		}
	}
}

void EditWindow::ClampTargetVertScroll()
{
	int height = GetContentHeight();
	int maxScroll = max(0, height - m_contentArea.h);
	m_targetVertScroll = SDL_clamp(m_targetVertScroll, 0.0f, (float)maxScroll);
	m_vertScroll = (int)m_targetVertScroll;
	if (m_activeSourceFileItem)
	{
		m_activeSourceFileItem->vertScroll = m_vertScroll;
	}
}
void EditWindow::ClampTargetHorizScroll()
{
	auto settings = gApp->GetSettings();
	int width = GetContentWidth();
	int maxScroll = max(0, width - m_contentArea.w + settings->textXMargin);
	m_targetHorizScroll = SDL_clamp(m_targetHorizScroll, 0.0f, (float)maxScroll);
	m_horizScroll = (int)m_targetHorizScroll;
	if (m_activeSourceFileItem)
	{
		m_activeSourceFileItem->horizScroll = m_horizScroll;
	}
}

void EditWindow::Update()
{
	m_cursorAnimTime += TIMEDELTA * 10.0f;

	if (m_autoScroll)
	{
		m_targetVertScroll += TIMEDELTA * m_autoScroll;
		ClampTargetVertScroll();

		ProcessMouseMarking(m_autoScroll_mouseX, m_autoScroll_mouseY);
	}

	auto s = gApp->GetSettings();
	if (m_activeSourceFileItem)
	{
		if (s->lowCPUMode)
		{
			m_vertScroll = (int)m_targetVertScroll;
			m_horizScroll = (int)m_targetHorizScroll;
		}
		else
		{
			m_vertScroll += (int)((m_targetVertScroll - (float)m_vertScroll) * 0.5f);
			m_horizScroll += (int)((m_targetHorizScroll - (float)m_horizScroll) * 0.5f);
		}
	}

	UpdateStatus();
}

extern bool CharInStr(char ch, const char* str);
bool EditWindow::ScanTokenAt(int line, int col, int &startCol, int &endCol)
{
	if (!m_activeSourceFileItem)
		return false;

	auto file = m_activeSourceFileItem->file;
	if (line >= file->GetLines().size())
		return false;

	auto sl = file->GetLines()[line];
	if (col >= sl->GetChars().size())
		return false;

	const char *TOKEN_TERMINATORS = "[]()<>=$%*/#+@-~;:!, \t";

	auto& chars = sl->GetChars();
	if (CharInStr(chars[col], TOKEN_TERMINATORS))
		return false;

	startCol = col;
	while (startCol > 0 && !CharInStr(chars[startCol-1], TOKEN_TERMINATORS))
		startCol--;

	endCol = col;
	while (endCol < chars.size()-1 && !CharInStr(chars[endCol+1], TOKEN_TERMINATORS))
		endCol++;

	return true;
}

void EditWindow::OnMouseDown(SDL_Event* e)
{
	auto settings = gApp->GetSettings();

	/*
	if (m_inputCapture == IC_Search && !Contains(m_searchBox->GetArea(), e->button.x, e->button.y))
	{
		m_searchBox->SetActive(false);
		m_inputCapture = IC_None;
	}
	else if (m_inputCapture == IC_Replace && !Contains(m_replaceBox->GetArea(), e->button.x, e->button.y))
	{
		m_replaceBox->SetActive(false);
		m_inputCapture = IC_None;
	}
	*/

	// LEFT button
	if (Contains(m_titleTabsRect, e->button.x, e->button.y))
	{
		// title tabs...
		for (auto sfi : m_fileTabs)
		{
			if (Contains(sfi->geText->GetRect(), e->button.x, e->button.y))
			{
				if (sfi != m_activeSourceFileItem)
				{
					SetActiveFile(sfi->file);
				}
			}
		}
	}
	else if (abs(e->button.x - m_activeXPosDecode) < 4)
	{
		// drag first divide
		m_dragMode = DRAG_DivideDecode;
		m_dragOffset = e->button.x - m_activeXPosDecode;
		gApp->SetCaptureMouseMotion(DELEGATE(EditWindow::OnMouseMotionCaptured));
	}
	else if (abs(e->button.x - m_activeXPosText) < 4)
	{
		// drag second divide
		m_dragMode = DRAG_DivideText;
		m_dragOffset = e->button.x - m_activeXPosText;
		gApp->SetCaptureMouseMotion(DELEGATE(EditWindow::OnMouseMotionCaptured));
	}
	else if (abs(e->button.x - m_activeXPosContextHelp) < 4)
	{
		// drag third divide
		m_dragMode = DRAG_DivideContext;
		m_dragOffset = e->button.x - m_activeXPosContextHelp;
		gApp->SetCaptureMouseMotion(DELEGATE(EditWindow::OnMouseMotionCaptured));
	}
	else if (Contains(m_vertBarFullArea, e->button.x, e->button.y))
	{
		if (Contains(m_vertBarArea, e->button.x, e->button.y))
		{
			m_dragMode = DRAG_EditVertScroll;
			m_dragMouseGrab = { e->button.x - m_vertBarArea.x, e->button.y - m_vertBarArea.y };
			gApp->SetCaptureMouseMotion(DELEGATE(EditWindow::OnMouseMotionCaptured));
		}
		else if (e->button.y < m_vertBarArea.y)
		{
			// page up
			m_targetVertScroll -= m_contentArea.h;
			ClampTargetVertScroll();
		}
		else if (e->button.y > m_vertBarArea.y + m_vertBarArea.h)
		{
			// page down
			m_targetVertScroll += m_contentArea.h;
			ClampTargetVertScroll();
		}
	}
	else if (Contains(m_horizBarFullArea, e->button.x, e->button.y))
	{
		if (Contains(m_horizBarArea, e->button.x, e->button.y))
		{
			m_dragMode = DRAG_EditHorizScroll;
			m_dragMouseGrab = { e->button.x - m_horizBarArea.x, e->button.y - m_horizBarArea.y };
			gApp->SetCaptureMouseMotion(DELEGATE(EditWindow::OnMouseMotionCaptured));
		}
		else if (e->button.x < m_horizBarArea.x)
		{
			// page left
			m_targetHorizScroll -= m_contentArea.w;
			ClampTargetHorizScroll();
		}
		else if (e->button.x > m_horizBarArea.x + m_horizBarArea.w)
		{
			// page right
			m_targetHorizScroll += m_contentArea.w;
			ClampTargetHorizScroll();
		}
	}
	else if (Contains(m_memAddrRect, e->button.x, e->button.y))
	{
		auto file = GetActiveFile();
		if (file)
		{
			auto csi = file->GetCompileInfo();
			if (csi)
			{
				int line, col;
				if (MouseToRowCol(e->button.x, e->button.y, line, col))
				{
					auto l = file->GetLines()[line];
					auto cl = csi->m_lines[line];
					if (l->GetBreakpoint())
						l->SetBreakpoint(0);
					else
					{
						l->SetBreakpoint(cl->type == LT_Instruction ? BRK_Execute : BRK_Read | BRK_Write);
					}
					gApp->ApplyBreakpoints();
				}
			}
		}
	}
	else if (Contains(m_sourceEditRect, e->button.x, e->button.y))
	{
		int line, col;
		if (MouseToRowCol(e->button.x, e->button.y, line, col))
		{
			if (m_shiftDown)
			{
				GotoLineCol(line, col, MARK_Mouse, true);
			}
			else
			{
				if (e->button.clicks == 2)
				{
					// mark current word
					if (m_activeSourceFileItem)
					{
						GotoLineCol(line, col, MARK_None, true);
						auto lines = m_activeSourceFileItem->file->GetLines();
						auto sl = lines[line];
						if (col < sl->GetChars().size())
						{
							int startCol, endCol;
							if (ScanTokenAt(line, col, startCol, endCol))
							{
								m_marked = m_mouseMarking = true;
								m_markStartColumn = startCol;
								m_markEndColumn = endCol+1;
								m_markStartLine = line;
								m_markEndLine = line;
								m_activeSourceFileItem->activeColumn = endCol + 1;
							}
						}
					}
				}
				else
				{
					GotoLineCol(line, col, MARK_None, true);
					m_marked = m_mouseMarking = true;
					m_markStartColumn = col;
					m_markEndColumn = col;
					m_markStartLine = line;
					m_markEndLine = line;
				}
			}
		}
	}
}

void EditWindow::OnMouseUp(SDL_Event* e)
{
	m_autoScroll = 0;
	m_marked = m_mouseMarking && !(m_markStartLine == m_markEndLine && m_markStartColumn == m_markEndColumn);
	m_mouseMarking = false;
	m_dragMode = DRAG_None;
}

void EditWindow::ProcessMouseMarking(int x, int y)
{
	int line, col;
	if (MouseToRowCol(x, y, line, col))
	{
		m_activeSourceFileItem->activeLine = line;
		m_activeSourceFileItem->activeColumn = col;
		m_activeSourceFileItem->activeTargetX = x - m_sourceEditRect.x;
		m_markEndColumn = col;
		m_markEndLine = line;

		// autoScroll
		m_autoScroll_mouseX = x;
		m_autoScroll_mouseY = y;
		int topDx = SDL_clamp(y - (m_sourceEditRect.y + 40), -40, 0);
		int bottomDx = SDL_clamp(y - (m_sourceEditRect.y + m_sourceEditRect.h - 40), 0, 40);
		if (topDx < 0)
			m_autoScroll = 40 * topDx;
		else if (bottomDx > 0)
			m_autoScroll = 40 * bottomDx;
		else
			m_autoScroll = 0;
	}
}

void EditWindow::OnMouseMotion(SDL_Event* e)
{
	m_mouseX = e->motion.x;
	m_mouseY = e->motion.y;

	int windowHeight, windowWidth;
	SDL_GetRendererOutputSize(gApp->GetRenderer(), &windowWidth, &windowHeight);

	auto settings = gApp->GetSettings();
	if (m_mouseMarking)
	{
		ProcessMouseMarking(e->motion.x, e->motion.y);
	}
}

void EditWindow::OnMouseMotionCaptured(bool lostCapture, int x, int y)
{
	auto settings = gApp->GetSettings();
	if (lostCapture)
		m_dragMode = DRAG_None;

	switch (m_dragMode)
	{
		case DRAG_DivideDecode:
			{
				settings->xPosDecode = SDL_clamp(x + m_dragOffset, 16, m_activeXPosText - 16);
				CalcRects();
			}
			return;
		case DRAG_DivideText:
			{
				settings->xPosText = SDL_clamp(x + m_dragOffset, m_activeXPosDecode + 16, m_activeXPosContextHelp - 32);
				CalcRects();
			}
			return;
		case DRAG_DivideContext:
			{
				int windowWidth, windowHeight;
				SDL_GetRendererOutputSize(gApp->GetRenderer(), &windowWidth, &windowHeight);
				settings->xPosContextHelp = SDL_clamp(x + m_dragOffset, m_activeXPosText + 32, windowWidth - 16);
				CalcRects();
			}
			return;
		case DRAG_EditVertScroll:
			{
				int newBarStart = y - m_dragMouseGrab.y;
				m_targetVertScroll = (float)(newBarStart - m_vertBarFullArea.y) / (float)m_vertBarFullArea.h * (float)GetContentHeight();
				ClampTargetVertScroll();
			}
			break;
		case DRAG_EditHorizScroll:
			{
				int newBarStart = x - m_dragMouseGrab.x;
				m_targetHorizScroll = (float)(newBarStart - m_horizBarFullArea.x) / (float)m_horizBarFullArea.w * (float)GetContentWidth();
				ClampTargetHorizScroll();
			}
			break;
	}
}

void EditWindow::UpdateCursor(int x, int y)
{
	auto settings = gApp->GetSettings();
	if (Contains(m_titleTabsRect, x, y))
	{
		for (auto sfi : m_fileTabs)
		{
			if (Contains(sfi->geText->GetRect(), x, y))
			{
				gApp->SetCursor(Cursor_Hand);
				return;
			}
		}
	}
	else if (abs(x - m_activeXPosDecode) < 4)
	{
		gApp->SetCursor(Cursor_Horiz);
		return;
	}
	else if (abs(x - m_activeXPosText) < 4)
	{
		gApp->SetCursor(Cursor_Horiz);
		return;
	}
	else if (abs(x - m_activeXPosContextHelp) < 4)
	{
		gApp->SetCursor(Cursor_Horiz);
		return;
	}
	else if (Contains(m_sourceEditRect, x, y))
	{
		gApp->SetCursor(Cursor_IBeam);
		return;
	}
	else if (Contains(m_horizBarFullArea, x, y))
	{
		if (Contains(m_horizBarArea, x, y))
		{
			gApp->SetCursor(Cursor_Horiz);
		}
		else
		{
			gApp->SetCursor(Cursor_Hand);
		}
		return;
	}
	else if (Contains(m_vertBarFullArea, x, y))
	{
		if (Contains(m_vertBarArea, x, y))
		{
			gApp->SetCursor(Cursor_Vert);
		}
		else
		{
			gApp->SetCursor(Cursor_Hand);
		}
		return;
	}
}

bool EditWindow::MouseToRowCol(int x, int y, int& row, int& col)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		auto settings = gApp->GetSettings();
		int localX = x - m_sourceEditRect.x + m_activeSourceFileItem->horizScroll - settings->textXMargin;
		int localY = y - m_sourceEditRect.y + m_activeSourceFileItem->vertScroll - settings->textYMargin;
		row = SDL_clamp(localY / settings->lineHeight, 0, (int)(file->GetLines().size() - 1));
		auto line = file->GetLines()[row];
		col = line->GetColumnAtX(localX);
		return true;
	}

	row = 0;
	col = 0;
	return false;
}

void EditWindow::OnTextInput(SDL_Event* e)
{
	u32string unicode_text = UTF8toUNICODE(string(e->text.text));
	for (auto ch : unicode_text)
	{
		if (ch <= 255)
		{
			if (m_marked)
			{
				gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, false);
			}
			gApp->Cmd_InsertChar((char)ch);
		}
	}
}

void EditWindow::OnKeyDown(SDL_Event* e)
{
	auto settings = gApp->GetSettings();

	// Don't need a file for these keys
	if (e->key.keysym.sym == SDLK_INSERT)
	{
		if (e->key.keysym.mod & KMOD_SHIFT)
			settings->autoIndent = !settings->autoIndent;
		else if (e->key.keysym.mod & KMOD_CTRL)
			settings->tabsToSpaces = !settings->tabsToSpaces;
		else
			settings->overwriteMode = !settings->overwriteMode;
		return;
	}

	if (m_activeSourceFileItem)
	{
		MarkingType markingType = e->key.keysym.mod & KMOD_SHIFT ? MARK_Key : MARK_None;

		// convert keypad back to normal
		if ((e->key.keysym.mod & KMOD_NUM) == 0)
		{
			switch (e->key.keysym.sym)
			{
				case SDLK_KP_0: e->key.keysym.sym = SDLK_INSERT; break;
				case SDLK_KP_1: e->key.keysym.sym = SDLK_END; break;
				case SDLK_KP_2: e->key.keysym.sym = SDLK_DOWN; break;
				case SDLK_KP_3: e->key.keysym.sym = SDLK_PAGEDOWN; break;
				case SDLK_KP_4: e->key.keysym.sym = SDLK_LEFT; break;
				case SDLK_KP_6: e->key.keysym.sym = SDLK_RIGHT; break;
				case SDLK_KP_7: e->key.keysym.sym = SDLK_HOME; break;
				case SDLK_KP_8: e->key.keysym.sym = SDLK_UP; break;
				case SDLK_KP_9: e->key.keysym.sym = SDLK_PAGEUP; break;
				case SDLK_KP_PERIOD: e->key.keysym.sym = SDLK_DELETE; break;
				case SDLK_KP_ENTER: e->key.keysym.sym = SDLK_RETURN; break;
				default: break;
			}
		}

		switch (e->key.keysym.sym)
		{
		case SDLK_f:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				auto winSar = gApp->GetWindowSearchAndReplace();
				winSar->SelectSearchBox();

				// if a single token is marked, copy it over as the new search string
				if (m_marked)
				{
					if (m_markStartLine == m_markEndLine)
					{
						auto &chars = m_activeSourceFileItem->file->GetLines()[m_activeSourceFileItem->activeLine]->GetChars();
						string token = chars.substr(m_markStartColumn, m_markEndColumn - m_markStartColumn);
						winSar->SetSearchText(token);
						m_marked = false;
					}
				}
				else
				{
					winSar->SetSearchText("");
					winSar->SetReplaceText("");
				}
				return;
			}
			break;

		case SDLK_r:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				auto winSar = gApp->GetWindowSearchAndReplace();
				winSar->SelectSearchBox();

				// if a single token is marked, copy it over as the new search string
				if (m_marked)
				{
					if (m_markStartLine == m_markEndLine)
					{
						auto& chars = m_activeSourceFileItem->file->GetLines()[m_activeSourceFileItem->activeLine]->GetChars();
						string token = chars.substr(m_markStartColumn, m_markEndColumn - m_markStartColumn);
						winSar->SetSearchText(token);
						winSar->SetReplaceText("");
						m_marked = false;
					}
				}
				else
				{
					winSar->SetSearchText("");
					winSar->SetReplaceText("");
				}
				return;
			}
			break;

		case SDLK_z:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				if (e->key.keysym.mod & KMOD_SHIFT)
					m_activeSourceFileItem->file->GetCmdManager()->Redo();
				else
					m_activeSourceFileItem->file->GetCmdManager()->Undo();
				return;
			}
			break;
		case SDLK_PAGEUP:
			for (int i = 0; i < 20; i++)
				CursorUp(markingType);
			return;
		case SDLK_PAGEDOWN:
			for (int i = 0; i < 20; i++)
				CursorDown(markingType);
			return;
		case SDLK_UP:
			CursorUp(markingType);
			return;
		case SDLK_DOWN:
			CursorDown(markingType);
			return;
		case SDLK_LEFT:
			CursorLeft(markingType);
			return;
		case SDLK_RIGHT:
			CursorRight(markingType);
			return;
		case SDLK_TAB:
			if (m_activeSourceFileItem && m_marked)
			{
				bool isMarked;
				int startLine, startColumn, endLine, endColumn;
				GetSortedMarking(isMarked, startLine, startColumn, endLine, endColumn);
				if (endColumn == 0)
					endLine--;
				if (e->key.keysym.mod & KMOD_SHIFT)
				{
					gApp->Cmd_UndentLines(startLine, endLine);
				}
				else
				{
					gApp->Cmd_IndentLines(startLine, endLine);
				}
				return;
			}
			else
			{
				int tabsToInsert = settings->tabWidth - (m_activeSourceFileItem->activeColumn % settings->tabWidth);
				if (settings->tabsToSpaces)
					gApp->Cmd_InsertSpaces(tabsToInsert);
				else
					gApp->Cmd_InsertChar('\t');
			}
			break;
		case SDLK_END:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				CursorBottomOfFile(markingType);
			}
			else
			{
				CursorEnd(markingType);
			}
			return;
		case SDLK_HOME:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				CursorTopOfFile(markingType);
			}
			else
			{
				CursorStart(markingType);
			}
			return;
		case SDLK_x:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				if (m_marked)
				{
					gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, true);
				}
				return;
			}
			break;
		case SDLK_a:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				m_marked = true;
				m_markStartLine = 0;
				m_markStartColumn = 0;
				m_markEndLine = (int)m_activeSourceFileItem->file->GetLines().size() - 1;
				m_markEndColumn = (int)m_activeSourceFileItem->file->GetLines().back()->GetChars().size();
				return;
			}
			break;

		case SDLK_c:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				if (m_marked)
				{
					gApp->Cmd_CopyArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn);
				}
				return;
			}
			break;
		case SDLK_v:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				if (m_marked)
				{
					gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, false);
				}
				gApp->Cmd_PasteArea(m_activeSourceFileItem->file);
				return;
			}
			break;
		case SDLK_BACKSPACE:
			if (m_marked)
			{
				gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, false);
			}
			else
			{
				gApp->Cmd_BackspaceChar();
			}
			return;
		case SDLK_DELETE:
			if (m_marked)
			{
				gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, false);
			}
			else
			{
				gApp->Cmd_DeleteChar();
			}
			return;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_shiftDown = true;
			return;
		case SDLK_RETURN:
			if (m_marked)
			{
				gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndColumn, m_markEndColumn, false);
			}
			gApp->Cmd_InsertNewLine();
			return;
		}
	}
}

void EditWindow::OnKeyUp(SDL_Event *e)
{
	switch (e->key.keysym.sym)
	{
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			m_shiftDown = false;
			return;
	}
}

void EditWindow::MakeActiveLineVisible()
{
	if (m_activeSourceFileItem)
	{
		auto settings = gApp->GetSettings();
		auto file = m_activeSourceFileItem->file;
		int viewY1 = (int)m_targetVertScroll;
		int viewY2 = (int)m_targetVertScroll + m_sourceEditRect.h;
		int y = m_activeSourceFileItem->activeLine * settings->lineHeight;
		if (y < viewY1)
			m_targetVertScroll = (float)(y - settings->lineHeight * 4);
		if (y >= viewY2)
			m_targetVertScroll = (float)(y - m_sourceEditRect.h + settings->lineHeight * 4);
		ClampTargetVertScroll();
	}
}

void EditWindow::CursorUp(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = m_activeSourceFileItem->activeColumn;

		if (gotoLine > 0)
		{
			gotoLine--;
			gotoColumn = file->GetLines()[gotoLine]->GetColumnAtX(m_activeSourceFileItem->activeTargetX);
		}
		GotoLineCol(gotoLine, gotoColumn, mark, false);
	}
}

void EditWindow::CursorDown(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = m_activeSourceFileItem->activeColumn;

		if (gotoLine < file->GetLines().size() - 1)
		{
			gotoLine++;
			gotoColumn = file->GetLines()[gotoLine]->GetColumnAtX(m_activeSourceFileItem->activeTargetX);
		}
		GotoLineCol(gotoLine, gotoColumn, mark, false);
	}
}

void EditWindow::CursorLeft(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = m_activeSourceFileItem->activeColumn;

		if (gotoColumn > 0)
			gotoColumn--;
		else if (gotoLine > 0)
		{
			gotoLine--;
			auto line = file->GetLines()[gotoLine];
			gotoColumn = (int)line->GetChars().size();
		}
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

void EditWindow::CursorRight(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = m_activeSourceFileItem->activeColumn;

		if (gotoColumn < (file->GetLines()[gotoLine]->GetChars().size()))
			gotoColumn++;
		else if (gotoLine < file->GetLines().size() - 1)
		{
			gotoLine++;
			gotoColumn = 0;
		}
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

void EditWindow::CursorStart(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = 0;
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

void EditWindow::CursorTopOfFile(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = 0;
		int gotoColumn = 0;
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

void EditWindow::CursorEnd(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = m_activeSourceFileItem->activeLine;
		int gotoColumn = (int)file->GetLines()[gotoLine]->GetChars().size();
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

void EditWindow::CursorBottomOfFile(MarkingType mark)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		int gotoLine = (int)m_activeSourceFileItem->file->GetLines().size()-1;
		int gotoColumn = (int)m_activeSourceFileItem->file->GetLines().back()->GetChars().size();
		GotoLineCol(gotoLine, gotoColumn, mark, true);
	}
}

bool EditWindow::CheckLineMarked(int lineNmbr, int& startCol, int& endCol)
{
	if (m_activeSourceFileItem && m_marked && ((m_markStartLine != m_markEndLine) || (m_markStartColumn != m_markEndColumn)))
	{
		int l1, c1, l2, c2;
		if (m_markEndLine < m_markStartLine || (m_markEndLine == m_markStartLine && m_markEndColumn < m_markStartColumn))
		{
			l1 = m_markEndLine;
			c1 = m_markEndColumn;
			l2 = m_markStartLine;
			c2 = m_markStartColumn;

			// step l2/c2 left once
			if (c2 > 0 || l2 > 0)
			{
				c2--;
				if (c2 < 0)
				{
					l2--;
					c2 = (int)m_activeSourceFileItem->file->GetLines()[l2]->GetChars().size();
				}
			}
		}
		else
		{
			l1 = m_markStartLine;
			c1 = m_markStartColumn;
			l2 = m_markEndLine;
			c2 = m_markEndColumn;

			// step l2/c2 left once
			if (c2 > 0 || l2 > 0)
			{
				c2--;
				if (c2 < 0)
				{
					l2--;
					c2 = (int)m_activeSourceFileItem->file->GetLines()[l2]->GetChars().size();
				}
			}
		}

		if (lineNmbr >= l1 && lineNmbr <= l2)
		{
			startCol = 0;
			endCol = (int)m_activeSourceFileItem->file->GetLines()[lineNmbr]->GetChars().size();

			if (lineNmbr == l1)
			{
				startCol = c1;
			}
			if (lineNmbr == l2)
			{
				endCol = c2;
			}
			return true;
		}
	}
	return false;
}


// status bar
struct StatusInfo
{
	bool insertMode;
	int line;
	int column;
	int totalLines;

	GraphicElement* m_geLine;
	GraphicElement* m_geColumn;
	GraphicElement* m_geTotalLines;
	GraphicElement* m_geFPS;
	GraphicChunk* m_gc;
} m_status;

void EditWindow::InitStatus()
{
	auto settings = gApp->GetSettings();
	m_status.m_ge = nullptr;
}
void EditWindow::UpdateStatus()
{
	auto settings = gApp->GetSettings();

	float td = TIMEDELTA;
	m_status.m_avgTimeDelta = (m_status.m_avgTimeDelta + td*3.0f) * 0.25f;

	if (m_activeSourceFileItem)
	{
		SDL_Color col = { 0,255,255,255 };
		auto cmdMgr = m_activeSourceFileItem->file->GetCmdManager();
		auto file = m_activeSourceFileItem->file;
		auto line = m_activeSourceFileItem->file->GetLines()[m_activeSourceFileItem->activeLine];
		int totalCmds = cmdMgr->GetTotalCmds();
		int currentCmd = cmdMgr->GetCurrentCmdIndex();
		int totalLines = (int)m_activeSourceFileItem->file->GetLines().size();

		char fpsText[256];
		sprintf(
			fpsText, "%2.1f MS  %4d FPS    %s %s %s    LINE: %05d/%05d    COL %03d/%03d    CMD: %03d/%03d",
			m_status.m_avgTimeDelta * 1000, (int)(1.0f / m_status.m_avgTimeDelta),
			settings->overwriteMode ? "OVR" : "INS", settings->autoIndent ? "IND" : "---", settings->tabsToSpaces ? "SPC" : "TAB",
			m_activeSourceFileItem->activeLine + 1, totalLines, m_activeSourceFileItem->activeColumn + 1, (int)line->GetChars().size(),
			currentCmd, totalCmds
		);

		delete m_status.m_ge;
		m_status.m_ge = GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), fpsText, col, 0, 0);
	}
}
void EditWindow::DrawStatus()
{
	auto r = gApp->GetRenderer();
	auto settings = gApp->GetSettings();
	int charW = settings->fontSize;

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &m_statusRect);

	m_status.m_ge->RenderAt( r, settings->textXMargin, m_statusRect.y + settings->textYMargin );
}

void EditWindow::GotoEmuPC()
{
	u16 pc = gApp->GetEmulator()->GetCurrentPC();
	auto sf = m_activeSourceFileItem->file;
	auto csi = sf->GetCompileInfo();
	for (int i = 0; i < csi->m_lines.size(); i++)
	{
		auto cl = csi->m_lines[i];
		if (cl->data.size() > 0 && pc >= cl->memAddr && (pc < cl->memAddr + cl->data.size()))
		{
			if (sf->GetLines().size() > i)
			{
				GotoLineCol(i, (int)sf->GetLines()[i]->GetChars().size(), MARK_None, true);
				return;
			}
		}
	}
}


void EditWindow::GotoLineCol(int ln, int col, MarkingType mark, bool trackXPos)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;

		if ((mark == MARK_Key && !m_keyMarking) || (mark == MARK_Mouse && !m_mouseMarking))
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_keyMarking = true;
		}

		m_activeSourceFileItem->activeLine = SDL_min((int)file->GetLines().size()-1, ln);
		auto line = file->GetLines()[m_activeSourceFileItem->activeLine];
		m_activeSourceFileItem->activeColumn = SDL_min((int)line->GetChars().size(), col);

		if (trackXPos)
		{
			int x1, x2;
			file->GetLines()[m_activeSourceFileItem->activeLine]->GetCharX(m_activeSourceFileItem->activeColumn, x1, x2);
			m_activeSourceFileItem->activeTargetX = x1 + 1;
		}

		m_cursorAnimTime = 0;

		switch (mark)
		{
			case MARK_None:
				m_keyMarking = false;
				m_mouseMarking = false;
				break;
			case MARK_Key:
				m_keyMarking = true;
				m_mouseMarking = false;
				break;
			case MARK_Mouse:
				m_keyMarking = false;
				m_mouseMarking = true;
				break;
		}

		if (mark != MARK_None)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}

		m_marked = (m_keyMarking || m_mouseMarking) && !(m_markStartLine == m_markEndLine && m_markStartColumn == m_markEndColumn);

		MakeActiveLineVisible();
	}
	UpdateContextualHelp();
}

void EditWindow::UpdateContextualHelp()
{
	if (IsActiveAsmFile())
	{
		gApp->GetCompiler()->LogContextualHelp(m_activeSourceFileItem->file, m_activeSourceFileItem->activeLine);
	}
	else
	{
		gApp->GetWindowHelp()->Clear();
	}

}

int EditWindow::GetContentHeight()
{
	return m_renderedContentHeight;
}

int EditWindow::GetContentWidth()
{
	return m_renderedContentWidth;
}

void EditWindow::CalcScrollBars()
{
	auto settings = gApp->GetSettings();

	m_vertBackArea = { m_sourceEditRect.x + m_sourceEditRect.w + 2, m_sourceEditRect.y + 2, settings->lineHeight - 4, m_sourceEditRect.h - 4 };
	m_vertBarFullArea = { m_vertBackArea.x + 2, m_vertBackArea.y + 2, m_vertBackArea.w - 4, m_vertBackArea.h - 4 };

	m_horizBackArea = { 2, m_sourceEditRect.y + m_sourceEditRect.h + 2, m_allEditRect.w - settings->lineHeight - 4, settings->lineHeight - 4 };
	m_horizBarFullArea = { m_horizBackArea.x + 2, m_horizBackArea.y + 2, m_horizBackArea.w - 4, m_horizBackArea.h - 4 };

	int vStart = m_vertScroll;
	int vEnd = m_vertScroll + m_sourceEditRect.h - settings->lineHeight;
	int height = GetContentHeight();
	float vStartRel = SDL_clamp((float)vStart / (float)height, 0.0f, 1.0f);
	float vEndRel = SDL_clamp((float)vEnd / (float)height, 0.0f, 1.0f);
	int vBarStart = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vStartRel);
	int vBarEnd = (int)(m_vertBarFullArea.y + m_vertBarFullArea.h * vEndRel);
	m_vertBarArea = { m_vertBarFullArea.x, vBarStart, m_vertBarFullArea.w, vBarEnd - vBarStart + 1 };

	int hStart = m_horizScroll;
	int hEnd = m_horizScroll + m_sourceEditRect.w;
	int width = GetContentWidth();
	float hStartRel = SDL_clamp((float)hStart / (float)width, 0.0f, 1.0f);
	float hEndRel = SDL_clamp((float)hEnd / (float)width, 0.0f, 1.0f);
	int hBarStart = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hStartRel);
	int hBarEnd = (int)(m_horizBarFullArea.x + m_horizBarFullArea.w * hEndRel);
	m_horizBarArea = { hBarStart, m_horizBarFullArea.y, hBarEnd - hBarStart + 1, m_horizBarFullArea.h };
}