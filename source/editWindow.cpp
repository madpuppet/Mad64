#include "common.h"
#include "editWindow.h"

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
	m_autoScroll = 0;
	m_autoScroll_mouseX = 0;
	m_autoScroll_mouseY = 0;
	m_marked = false;
	m_keyMarking = false;
	m_mouseMarking = false;
	m_inputCapture = IC_None;

	m_cursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	m_cursorIBeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	m_cursorHoriz = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	m_cursorVert = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	m_cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

	m_searchBox = new TextInput(0,0,"S",DELEGATE(EditWindow::OnSearchEnter));
	m_searchBox->SetOnChange(DELEGATE(EditWindow::OnSearchChange));
	m_replaceBox = new TextInput(0, 0, "R", DELEGATE(EditWindow::OnReplaceEnter));

	m_mouseX = 0;
	m_mouseY = 0;

	CalcRects();
	InitStatus();
}

void EditWindow::OnSearchChange(const string& text)
{
	// search all
	if (m_activeSourceFileItem)
	{
		// find all occurances...
		m_searchFoundLines.clear();
		auto& lines = m_activeSourceFileItem->file->GetLines();
		for (int ln = 0; ln < lines.size(); ln++)
		{
			if (StrFind(lines[ln]->GetChars(), text) != string::npos)
			{
				m_searchFoundLines.push_back(ln);
			}
		}
	}
}
void EditWindow::OnSearchEnter(const string& text)
{
	// search all
	if (m_activeSourceFileItem)
	{
		// find all occurances...
		m_searchFoundLines.clear();
		auto& lines = m_activeSourceFileItem->file->GetLines();
		for (int ln = 0; ln < lines.size(); ln++)
		{
			if (StrFind(lines[ln]->GetChars(), text) != string::npos)
			{
				m_searchFoundLines.push_back(ln);
			}
		}

		int startLine = (m_activeSourceFileItem->activeLine + 1) % lines.size();
		int ln = startLine;
		do
		{
			auto line = lines[ln];
			auto& chars = line->GetChars();
			int col = (int)StrFind(line->GetChars(), text);
			if (col != string::npos)
			{
				m_searchBox->Flash(TextInput::MODE_Found);
				GotoLineCol(ln, col, MARK_None, true);
				return;
			}
			ln = (ln + 1) % m_activeSourceFileItem->file->GetLines().size();
		} while (ln != m_activeSourceFileItem->activeLine);
		m_searchBox->Flash(TextInput::MODE_NotFound);
	}
}
void EditWindow::OnReplaceEnter(const string& text)
{
	// replace all
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

void EditWindow::Draw()
{
	auto r = gApp->GetRenderer();
	auto settings = gApp->GetSettings();

	// draw file tabs
	SDL_Color highlight = { 255, 255, 255, 255 };
	SDL_Color inactive = { 64, 84, 100, 255 };

	int windowWidth, windowHeight;
	SDL_GetWindowSize(gApp->GetWindow(), &windowWidth, &windowHeight);

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
	if (m_activeSourceFileItem)
	{
		SourceFile* file = m_activeSourceFileItem->file;
		int startLine = max(0, m_activeSourceFileItem->scroll / settings->lineHeight);
		int endLine = min((m_sourceEditRect.h + m_activeSourceFileItem->scroll) / settings->lineHeight + 1, (int)file->GetLines().size());

		// draw addresses
		SDL_RenderSetClipRect(r, &m_memAddrRect);
		int sourceVersion = file->GetSourceVersion();
		for (int i = startLine; i < endLine; i++)
		{
			int brighten = (m_activeSourceFileItem->activeLine == i) ? 16 : 0;
			int y = m_memAddrRect.y + i * settings->lineHeight - m_activeSourceFileItem->scroll;
			auto gc = gApp->GetCompiler()->GetMemAddrGC(file, i, sourceVersion);
			if (gc)
			{
				SDL_Rect lineQuad = { m_memAddrRect.x, y, m_memAddrRect.w, settings->lineHeight };
				SDL_SetRenderDrawColor(r, brighten, brighten, brighten + 128 - ((i & 1) ? 16 : 0), 255);
				SDL_RenderFillRect(r, &lineQuad);
				gc->DrawAt(m_memAddrRect.x + settings->textXMargin, y + settings->textYMargin);
			}
		}

		// draw decode
		SDL_RenderSetClipRect(r, &m_decodeRect);
		for (int i = startLine; i < endLine; i++)
		{
			int brighten = (m_activeSourceFileItem->activeLine == i) ? 16 : 0;
			auto gc = gApp->GetCompiler()->GetDecodeGC(file, i, sourceVersion);
			if (gc)
			{
				int y = m_decodeRect.y + i * settings->lineHeight - m_activeSourceFileItem->scroll;
				SDL_Rect lineQuad = { m_decodeRect.x, y, m_decodeRect.w, settings->lineHeight };
				SDL_SetRenderDrawColor(r, brighten, brighten + 32 - ((i & 1) ? 8 : 0), brighten, 255);
				SDL_RenderFillRect(r, &lineQuad);
				gc->DrawAt(m_decodeRect.x + settings->textXMargin, y + settings->textYMargin);
			}
		}

		int branchDepth = 0;

		// draw text
		SDL_RenderSetClipRect(r, &m_sourceEditRect);

		for (int i = startLine; i < endLine; i++)
		{
			int brighten = (m_activeSourceFileItem->activeLine == i) ? 32 : 0;
			auto line = file->GetLines()[i];
			int y = m_sourceEditRect.y + i * settings->lineHeight - m_activeSourceFileItem->scroll;
			int lineWidth = line->GetLineWidth() + settings->textXMargin;

			if (line->GetChars().empty())
			{
				SDL_Rect lineQuad = { settings->xPosText, y, m_sourceEditRect.w, settings->lineHeight };
				SDL_SetRenderDrawColor(r, brighten, brighten, brighten + 80 - ((i & 1) ? 8 : 0), 255);
				SDL_RenderFillRect(r, &lineQuad);
			}
			else
			{
				SDL_Rect lineQuad1 = { settings->xPosText, y, lineWidth, settings->lineHeight };
				SDL_Rect lineQuad2 = { settings->xPosText + lineWidth, y, m_sourceEditRect.w - lineWidth, settings->lineHeight };
				SDL_SetRenderDrawColor(r, 0, brighten, 128 - ((i & 1) ? 16 : 0), 255);
				SDL_RenderFillRect(r, &lineQuad1);
				if (lineWidth < m_sourceEditRect.w)
				{
					SDL_SetRenderDrawColor(r, 0, brighten, 80 - ((i & 1) ? 8 : 0), 255);
					SDL_RenderFillRect(r, &lineQuad2);
				}
			}

			// draw marking
			int markStartCol, markEndCol;
			if (CheckLineMarked(i, markStartCol, markEndCol))
			{
				int startX1, startX2, endX1, endX2;
				line->GetCharX(markStartCol, startX1, startX2);
				line->GetCharX(markEndCol, endX1, endX2);
				SDL_Rect markedRect = { m_sourceEditRect.x + settings->textXMargin + startX1, y, endX2 - startX1, settings->lineHeight };
				SDL_SetRenderDrawColor(r, 128, 128, 128, 255);
				SDL_RenderFillRect(r, &markedRect);
			}

			// search highlight
			auto& search = m_searchBox->GetText();
			if (!search.empty())
			{
				auto& str = line->GetChars();
				size_t off = 0;
				while (true)
				{
					off = StrFind(str, search, off);
					if (off == string::npos)
						break;

					int x1,x2,stub;
					line->GetCharX((int)off, x1, stub);
					line->GetCharX((int)off + (int)search.size() - 1, stub, x2);
					SDL_Rect markedRect = { m_sourceEditRect.x + settings->textXMargin + x1, y, x2-x1, settings->lineHeight };
					SDL_SetRenderDrawColor(r, 128, 128, 0, 255);
					SDL_RenderFillRect(r, &markedRect);

					off += search.size();
				}
			}

			// - text
			if (line->GetGCText())
				line->GetGCText()->DrawAt(settings->xPosText + settings->textXMargin, y + settings->textYMargin);

			// draw branches
			auto csi = m_activeSourceFileItem->file->GetCompileInfo();
			if (csi && csi->m_lines.size() > i)
			{
				auto cli = csi->m_lines[i];
				if (!cli->error && (cli->addressMode == AM_Relative))
				{
					int branchAddress = cli->memAddr + (int)cli->operand + 2;
					int branchLine = csi->FindLineByAddress(branchAddress);
					if (branchLine != -1)
					{
						int y1 = y + settings->lineHeight / 2;
						int y2 = m_sourceEditRect.y + branchLine * settings->lineHeight - m_activeSourceFileItem->scroll + settings->lineHeight / 2;
						int x1 = m_sourceEditRect.x - 2;
						int x2 = x1 - branchDepth - 10;

						SDL_RenderSetClipRect(r, nullptr);
						SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
						SDL_RenderDrawLine(r, x1, y1, x2, y1);
						SDL_RenderDrawLine(r, x2, y1, x2, y2);
						SDL_RenderDrawLine(r, x2, y2, x1, y2);
						SDL_RenderDrawLine(r, x1, y2, x1 - 5, y2 - 5);
						SDL_RenderDrawLine(r, x1, y2, x1 - 5, y2 + 5);
						SDL_RenderSetClipRect(r, &m_sourceEditRect);

						branchDepth += 4;
					}
				}
			}

			// draw cursor
			if (i == m_activeSourceFileItem->activeLine && m_inputCapture == IC_None)
			{
				int cursorX1, cursorX2;
				line->GetCharX(m_activeSourceFileItem->activeColumn, cursorX1, cursorX2);
				int cursorY = m_sourceEditRect.y + m_activeSourceFileItem->activeLine * settings->lineHeight - m_activeSourceFileItem->scroll;
				int brightness = max(0, (int)(100 + cosf(m_cursorAnimTime) * 128));
				SDL_Rect cursorRect = { m_sourceEditRect.x + settings->textXMargin + cursorX1, cursorY, settings->overwriteMode ? (cursorX2-cursorX1) : 2, settings->lineHeight };
				SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(r, 255, 255, 255, brightness);
				SDL_RenderFillRect(r, &cursorRect);
				SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
			}
		}

		// draw scroll bar
		int barY1, barY2;
		CalcScrollBar(barY1, barY2);

		SDL_Rect BarBack = { settings->xPosContextHelp - settings->scrollBarWidth, m_sourceEditRect.y, settings->scrollBarWidth, m_sourceEditRect.h };
		SDL_Rect Bar = { settings->xPosContextHelp - settings->scrollBarWidth + 4, barY1, settings->scrollBarWidth - 4, barY2-barY1 };
		SDL_SetRenderDrawColor(r, 0, 0, 32, 255);
		SDL_RenderFillRect(r, &BarBack);
		if (m_dragMode == DRAG_EditVertScroll)
		{
			SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(r, 64, 64, 255, 255);
		}
		SDL_RenderFillRect(r, &Bar);

		// draw search lines
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		for (auto l : m_searchFoundLines)
		{
			float relY = (float)l / (float)(m_activeSourceFileItem->file->GetLines().size()-1);

			int y = (int)(m_sourceEditRect.y + (float)(relY * m_sourceEditRect.h));
			SDL_RenderDrawLine(r, Bar.x+1, y, Bar.x + Bar.w - 1, y);
		}
	}
	SDL_RenderSetClipRect(r, nullptr);

	// - context help
	gApp->GetLogWindow()->Draw();

	// draw status bar
	DrawStatus();

	// draw search boxes
	SDL_Rect sarRect = { settings->xPosContextHelp, settings->lineHeight, windowWidth - settings->xPosContextHelp, settings->lineHeight };
	SDL_SetRenderDrawColor(r, 32, 64, 32, 255);
	SDL_RenderFillRect(r, &sarRect);
	m_searchBox->Draw();
	m_replaceBox->Draw();

	// draw context bar split
	SDL_Rect divider = { settings->xPosContextHelp, settings->lineHeight * 2 - 1, windowWidth - settings->xPosContextHelp, 3 };
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &divider);

	// draw separator bars
	DrawColouredLine(settings->xPosDecode, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(settings->xPosText, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(settings->xPosContextHelp, m_sourceEditRect.y, m_sourceEditRect.y + m_sourceEditRect.h, false);

	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
}

void EditWindow::OnResize()
{
	// clamp the divides so they are on screen
	auto settings = gApp->GetSettings();
	int windowWidth, windowHeight;
	SDL_GetWindowSize(gApp->GetWindow(), &windowWidth, &windowHeight);
	settings->xPosContextHelp = min(settings->xPosContextHelp, windowWidth - 16);
	settings->xPosText = min(settings->xPosText, settings->xPosContextHelp-32);
	settings->xPosDecode = min(settings->xPosDecode, settings->xPosText - 16);

	CalcRects();

}

void EditWindow::CalcRects()
{
	int windowWidth, windowHeight;
	SDL_GetWindowSize(gApp->GetWindow(), &windowWidth, &windowHeight);
	auto settings = gApp->GetSettings();
	int editHeight = windowHeight - settings->lineHeight*2;

	m_titleTabsRect = { 0, 0, windowWidth, settings->lineHeight };
	m_memAddrRect = { 0, settings->lineHeight, settings->xPosDecode, editHeight };
	m_decodeRect = { settings->xPosDecode, settings->lineHeight, settings->xPosText - settings->xPosDecode, editHeight };
	m_sourceEditRect = { settings->xPosText, settings->lineHeight, settings->xPosContextHelp - settings->xPosText, editHeight };
	m_statusRect = { 0, windowHeight - settings->lineHeight, windowWidth, settings->lineHeight };
	m_searchBox->SetPos(settings->xPosContextHelp, settings->lineHeight);
	m_replaceBox->SetPos(settings->xPosContextHelp + 200, settings->lineHeight);
	m_contextHelpRect = { settings->xPosContextHelp, settings->lineHeight * 2, windowWidth - settings->xPosContextHelp, editHeight - settings->lineHeight };

	gApp->GetLogWindow()->SetRect(m_contextHelpRect);
}


bool EditWindow::CalcScrollBar(int &start, int &end)
{
	int lineHeight = gApp->GetSettings()->lineHeight;
	start = 0;
	end = 0;
	if (m_activeSourceFileItem)
	{
		int fileHeight = lineHeight * (int)m_activeSourceFileItem->file->GetLines().size();

		int viewStart = m_activeSourceFileItem->scroll;
		int viewEnd = m_activeSourceFileItem->scroll + m_sourceEditRect.h;
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
	sfi->geText = GraphicElement::CreateFromText(gApp->GetFont(), file->GetName().c_str(), { 255,255,255,255 }, 0, 0);
	sfi->activeColumn = 0;
	sfi->activeLine = 0;
	sfi->activeTargetX = 0;
	m_fileTabs.push_back(sfi);
	LayoutTabs();

	// make new file the active file
	m_activeSourceFileItem = sfi;
}

void EditWindow::SetActiveFile(SourceFile* file)
{
	for (auto sfi : m_fileTabs)
	{
		if (sfi->file == file)
		{
			m_activeSourceFileItem = sfi;
			return;
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
				m_activeSourceFileItem = nullptr;
			else
				m_activeSourceFileItem = m_fileTabs.back();
			LayoutTabs();
			return;
		}
	}
}

void EditWindow::OnMouseWheel(SDL_Event* e)
{
	if (Contains(m_sourceEditRect,m_mouseX, m_mouseY))
	{
		if (m_activeSourceFileItem)
		{
			m_activeSourceFileItem->targetScroll += (e->wheel.preciseY * -80.0f);
			ClampTargetScroll();
		}
	}
	else if (Contains(m_contextHelpRect, m_mouseX, m_mouseY))
	{
		gApp->GetLogWindow()->OnMouseWheel(e);
	}
}

void EditWindow::ClampTargetScroll()
{
	if (m_activeSourceFileItem)
	{
		int fileHeight = gApp->GetSettings()->lineHeight * (int)m_activeSourceFileItem->file->GetLines().size();
		int maxScroll = max(0, fileHeight - m_sourceEditRect.h);
		m_activeSourceFileItem->targetScroll = SDL_clamp(m_activeSourceFileItem->targetScroll, 0.0f, (float)maxScroll);
	}
}

void EditWindow::Update()
{
	m_cursorAnimTime += TIMEDELTA * 10.0f;

	if (m_autoScroll)
	{
		m_activeSourceFileItem->targetScroll += TIMEDELTA * m_autoScroll;
		ClampTargetScroll();

		ProcessMouseMarking(m_autoScroll_mouseX, m_autoScroll_mouseY);
	}

	auto s = gApp->GetSettings();
	if (m_activeSourceFileItem)
	{
		m_activeSourceFileItem->scroll += (int)((m_activeSourceFileItem->targetScroll - (float)m_activeSourceFileItem->scroll) * 0.25f);
	}

	UpdateStatus();

	m_searchBox->Update();
	m_replaceBox->Update();
}

void EditWindow::OnMouseDown(SDL_Event* e)
{
	auto settings = gApp->GetSettings();
	if (e->button.button == 1)
	{
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
		else if (Contains(m_searchBox->GetArea(), e->button.x, e->button.y))
		{
			m_searchBox->SetActive(true);
			m_replaceBox->SetActive(false);
			m_inputCapture = IC_Search;
		}
		else if (Contains(m_replaceBox->GetArea(), e->button.x, e->button.y))
		{
			m_searchBox->SetActive(false);
			m_replaceBox->SetActive(true);
			m_inputCapture = IC_Replace;
		}
		else if (abs(e->button.x - settings->xPosDecode) < 2)
		{
			// drag first divide
			m_dragMode = DRAG_DivideDecode;
			m_dragOffset = e->button.x - settings->xPosDecode;
		}
		else if (abs(e->button.x - settings->xPosText) < 2)
		{
			// drag second divide
			m_dragMode = DRAG_DivideText;
			m_dragOffset = e->button.x - settings->xPosText;
		}
		else if (abs(e->button.x - settings->xPosContextHelp) < 2)
		{
			// drag third divide
			m_dragMode = DRAG_DivideContext;
			m_dragOffset = e->button.x - settings->xPosContextHelp;
		}
		else if (Contains(m_contextHelpRect, e->button.x, e->button.y))
		{
			if (e->button.x > m_contextHelpRect.x + m_contextHelpRect.w - settings->scrollBarWidth)
			{
				m_dragMode = DRAG_LogVertScroll;
				gApp->GetLogWindow()->SnapScrollBarToMouseY(e->button.y);
			}
			else
			{
				gApp->GetLogWindow()->OnMouseDown(e);
			}
			return;
		}
		else if (Contains(m_sourceEditRect, e->button.x, e->button.y))
		{
			if (e->button.x > m_sourceEditRect.x + m_sourceEditRect.w - settings->scrollBarWidth)
			{
				m_dragMode = DRAG_EditVertScroll;
				SnapScrollBarToMouseY(e->button.y);
			}
			else
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
}
void EditWindow::SnapScrollBarToMouseY(int y)
{
	if (m_activeSourceFileItem)
	{
		float rel = (float)(y - m_sourceEditRect.y) / (float)m_sourceEditRect.h;
		int fileHeight = (int)m_activeSourceFileItem->file->GetLines().size() * gApp->GetSettings()->lineHeight;
		m_activeSourceFileItem->targetScroll = fileHeight * rel - m_sourceEditRect.h * 0.5f;
		ClampTargetScroll();
	}
}

void EditWindow::OnMouseUp(SDL_Event* e)
{
	m_autoScroll = 0;
	m_marked = m_mouseMarking && !(m_markStartLine == m_markEndLine && m_markStartColumn == m_markEndColumn);
	m_mouseMarking = false;
	m_dragMode = DRAG_None;
	gApp->GetLogWindow()->OnMouseUp(e);
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

	int windowWidth;
	SDL_GetWindowSize(gApp->GetWindow(), &windowWidth, 0);

	auto settings = gApp->GetSettings();
	if (m_mouseMarking)
	{
		ProcessMouseMarking(e->motion.x, e->motion.y);
	}
	else if (m_dragMode != DRAG_None)
	{
		switch (m_dragMode)
		{
		case DRAG_DivideDecode:
			{
				settings->xPosDecode = SDL_clamp(e->motion.x + m_dragOffset, 16, settings->xPosText - 16);
				CalcRects();
			}
			return;
		case DRAG_DivideText:
			{
				settings->xPosText = SDL_clamp(e->motion.x + m_dragOffset, settings->xPosDecode + 16, settings->xPosContextHelp - 32);
				CalcRects();
			}
			return;
		case DRAG_DivideContext:
			{
				settings->xPosContextHelp = SDL_clamp(e->motion.x + m_dragOffset, settings->xPosText + 32, windowWidth - 16);
				CalcRects();
			}
			return;
		case DRAG_EditVertScroll:
			SnapScrollBarToMouseY(e->motion.y);
			break;
		case DRAG_LogVertScroll:
			gApp->GetLogWindow()->SnapScrollBarToMouseY(e->motion.y);
			break;
		}
	}
	else
	{
		gApp->GetLogWindow()->OnMouseMotion(e);
		SelectCursor(e->motion.x, e->motion.y);
	}
}

void EditWindow::SelectCursor(int x, int y)
{
	auto settings = gApp->GetSettings();
	if (Contains(m_titleTabsRect, x, y))
	{
		for (auto sfi : m_fileTabs)
		{
			if (Contains(sfi->geText->GetRect(), x, y))
			{
				SDL_SetCursor(m_cursorHand);
				return;
			}
		}
	}
	else if (abs(x - settings->xPosDecode) < 2)
	{
		SDL_SetCursor(m_cursorHoriz);
		return;
	}
	else if (abs(x - settings->xPosText) < 2)
	{
		SDL_SetCursor(m_cursorHoriz);
		return;
	}
	else if (abs(x - settings->xPosContextHelp) < 2)
	{
		SDL_SetCursor(m_cursorHoriz);
		return;
	}
	else if (Contains(m_sourceEditRect, x, y))
	{
		if (x > m_sourceEditRect.x + m_sourceEditRect.w - settings->scrollBarWidth)
		{
			SDL_SetCursor(m_cursorVert);
		}
		else
		{
			SDL_SetCursor(m_cursorIBeam);
		}
		return;
	}
	else if (Contains(m_contextHelpRect, x, y))
	{
		if (x > m_contextHelpRect.x + m_contextHelpRect.w - settings->scrollBarWidth)
		{
			SDL_SetCursor(m_cursorVert);
			return;
		}
		else
		{
			int line;
			if (gApp->GetLogWindow()->FindLogLineAt(y, line))
			{
				SDL_SetCursor(m_cursorHand);
				return;
			}
		}
	}
	SDL_SetCursor(m_cursorArrow);
}

bool EditWindow::MouseToRowCol(int x, int y, int& row, int& col)
{
	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		auto settings = gApp->GetSettings();
		int localX = x - m_sourceEditRect.x - settings->textXMargin;
		int localY = y - m_sourceEditRect.y + m_activeSourceFileItem->scroll;
		row = SDL_clamp(localY / settings->lineHeight, 0, (int)(file->GetLines().size() - 1));
		auto line = file->GetLines()[row];
		col = line->GetColumnAtX(localX);
		return true;
	}

	row = 0;
	col = 0;
	return false;
}

char s_shifted[128] =
{
	46,  46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  46,  46,  46, 46,		//00
	46,  46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  46,  46,  46, 46,		//10
	32,  46, 46, 46, 46, 46, 46, 34, 46, 46, 46, 46,  60,  95,  62, 63,		//20
	41,  33, 64, 35, 36, 37, 94, 38, 42, 40, 46, 58,  46,  43,  46, 46,		//30
	46,  46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  46,  46,  46, 46,		//40
	46,  46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 123, 124, 125, 46, 46,		//50
	126, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,  76,  77,  78, 79,		//60
	80,  81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 46,  46,  46,  46, 46 		//70
};

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

	if (m_inputCapture == IC_Search)
	{
		if (e->key.keysym.sym == SDLK_TAB)
		{
			m_searchBox->SetActive(false);
			m_replaceBox->SetActive(true);
			m_inputCapture = IC_Replace;
		}
		else if (e->key.keysym.sym == SDLK_ESCAPE)
		{
			m_searchBox->SetActive(false);
			m_inputCapture = IC_None;
		}
		else
		{
			m_searchBox->OnKeyDown(e);
		}
		return;
	}
	
	if (m_inputCapture == IC_Replace)
	{
		if (e->key.keysym.sym == SDLK_TAB)
		{
			m_searchBox->SetActive(true);
			m_replaceBox->SetActive(false);
			m_inputCapture = IC_Search;
		}
		else if (e->key.keysym.sym == SDLK_ESCAPE)
		{
			m_replaceBox->SetActive(false);
			m_inputCapture = IC_None;
		}
		else
		{
			m_replaceBox->OnKeyDown(e);
		}
		return;
	}

	if (m_activeSourceFileItem)
	{
		MarkingType markingType = e->key.keysym.mod & KMOD_SHIFT ? MARK_Key : MARK_None;
		switch (e->key.keysym.sym)
		{
		case SDLK_f:
			if (e->key.keysym.mod & KMOD_CTRL)
			{
				m_searchBox->SetActive(true);
				m_replaceBox->SetActive(false);
				m_inputCapture = IC_Search;
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

		char ch = e->key.keysym.sym;
		if (ch >= SDLK_SPACE && ch <= SDLK_z)
		{
			ch = KeySymToAscii(e->key.keysym);
			if (m_marked)
			{
				gApp->Cmd_DeleteArea(m_activeSourceFileItem->file, m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn, false);
			}
			gApp->Cmd_InsertChar(ch);
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

char KeySymToAscii(const SDL_Keysym& sym)
{
	char ch = sym.sym;
	if (ch >= SDLK_SPACE && ch <= SDLK_z)
	{
		if (sym.mod & KMOD_SHIFT)
			ch = s_shifted[ch];
	}
	return ch;
}


void EditWindow::MakeActiveLineVisible()
{
	if (m_activeSourceFileItem)
	{
		auto settings = gApp->GetSettings();
		auto file = m_activeSourceFileItem->file;
		int viewY1 = (int)m_activeSourceFileItem->targetScroll;
		int viewY2 = (int)m_activeSourceFileItem->targetScroll + m_sourceEditRect.h;
		int y = m_activeSourceFileItem->activeLine * settings->lineHeight;
		if (y < viewY1)
			m_activeSourceFileItem->targetScroll = (float)(y - settings->lineHeight * 4);
		if (y >= viewY2)
			m_activeSourceFileItem->targetScroll = (float)(y - m_sourceEditRect.h + settings->lineHeight * 4);
		ClampTargetScroll();
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

	GraphicElement* m_geInsertMode;
	GraphicElement* m_geLine;
	GraphicElement* m_geColumn;
	GraphicElement* m_geTotalLines;
	GraphicChunk* m_gc;
} m_status;

void EditWindow::InitStatus()
{
	auto settings = gApp->GetSettings();

	m_status.overwriteMode = settings->overwriteMode;
	m_status.autoIndent = settings->autoIndent;
	m_status.tabsToSpaces = settings->tabsToSpaces;

	if (m_activeSourceFileItem)
	{
		m_status.line = m_activeSourceFileItem->activeLine;
		m_status.column = m_activeSourceFileItem->activeColumn;
		m_status.totalLines = (int)m_activeSourceFileItem->file->GetLines().size();
	}
	else
	{
		m_status.line = 0;
		m_status.column = 0;
		m_status.totalLines = 0;
	}

	m_status.m_geModes = nullptr;
	m_status.m_geLine = nullptr;
	m_status.m_geColumn = nullptr;
	m_status.m_geUndo = nullptr;
}
void EditWindow::UpdateStatus()
{
	auto settings = gApp->GetSettings();

	SDL_Color col = { 0,255,255,255 };
	if (!m_status.m_geModes || m_status.overwriteMode != settings->overwriteMode || m_status.autoIndent != settings->autoIndent || m_status.tabsToSpaces != settings->tabsToSpaces)
	{
		delete m_status.m_geModes;
		m_status.overwriteMode = settings->overwriteMode;
		m_status.autoIndent = settings->autoIndent;
		m_status.tabsToSpaces = settings->tabsToSpaces;

		char text[64];
		sprintf(text, "%s %s %s", settings->overwriteMode ? "OVR" : "INS", settings->autoIndent ? "IND" : "---", settings->tabsToSpaces ? "SPC" : "TAB");
		m_status.m_geModes = GraphicElement::CreateFromText(gApp->GetFont(), text, col, 0, 0);
	};

	if (m_activeSourceFileItem)
	{
		auto file = m_activeSourceFileItem->file;
		auto line = m_activeSourceFileItem->file->GetLines()[m_activeSourceFileItem->activeLine];
		if (!m_status.m_geLine || m_status.line != m_activeSourceFileItem->activeLine || m_status.totalLines != (int)file->GetLines().size())
		{
			delete m_status.m_geLine;
			m_status.line = m_activeSourceFileItem->activeLine;
			m_status.totalLines = (int)m_activeSourceFileItem->file->GetLines().size();

			char temp[256];
			sprintf(temp, "LINE: %d/%d", m_status.line+1, m_status.totalLines);
			m_status.m_geLine = GraphicElement::CreateFromText(gApp->GetFont(), temp, col, 0, 0);
		}

		if (!m_status.m_geColumn || m_status.column != m_activeSourceFileItem->activeColumn || m_status.totalColumns != (int)line->GetChars().size())
		{
			delete m_status.m_geColumn;
			m_status.column = m_activeSourceFileItem->activeColumn;
			m_status.totalColumns = (int)line->GetChars().size();

			char temp[256];
			sprintf(temp, "COL: %d/%d", m_status.column+1, (int)line->GetChars().size());
			m_status.m_geColumn = GraphicElement::CreateFromText(gApp->GetFont(), temp, col, 0, 0);
		}

		auto cmdMgr = m_activeSourceFileItem->file->GetCmdManager();
		int totalCmds = cmdMgr->GetTotalCmds();
		int currentCmd = cmdMgr->GetCurrentCmdIndex();
		if (!m_status.m_geUndo || m_status.undo != currentCmd || m_status.totalUndo != totalCmds)
		{
			delete m_status.m_geUndo;
			m_status.undo = currentCmd;
			m_status.totalUndo = totalCmds;

			char temp[256];
			sprintf(temp, "CMD: %d/%d", currentCmd, totalCmds);
			m_status.m_geUndo = GraphicElement::CreateFromText(gApp->GetFont(), temp, col, 0, 0);
		}
	}
}
void EditWindow::DrawStatus()
{
	auto r = gApp->GetRenderer();
	auto settings = gApp->GetSettings();
	int charW = settings->fontSize;

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &m_statusRect);

	if (m_status.m_geModes)
	{
		SDL_Rect rect = { m_statusRect.w - charW * 70, m_statusRect.y + settings->textYMargin, m_status.m_geModes->GetRect().w, m_status.m_geModes->GetRect().h };
		SDL_RenderCopy(r, m_status.m_geModes->GetTexture(), NULL, &rect);
	}
	if (m_status.m_geLine && m_status.m_geColumn && m_status.m_geUndo)
	{
		int lineW = m_status.m_geLine->GetRect().w;
		int colW = m_status.m_geColumn->GetRect().w;
		int undoW = m_status.m_geUndo->GetRect().w;

		SDL_Rect lineRect = { m_statusRect.w - lineW - 15, m_statusRect.y + settings->textYMargin, lineW, m_status.m_geLine->GetRect().h };
		SDL_Rect colRect = { m_statusRect.w - colW - charW * 20 - 15, m_statusRect.y + settings->textYMargin, colW, m_status.m_geLine->GetRect().h };
		SDL_Rect undoRect = { m_statusRect.w - undoW - charW * 40 - 15, m_statusRect.y + settings->textYMargin, undoW, m_status.m_geLine->GetRect().h };

		SDL_RenderCopy(r, m_status.m_geColumn->GetTexture(), NULL, &colRect);
		SDL_RenderCopy(r, m_status.m_geLine->GetTexture(), NULL, &lineRect);
		SDL_RenderCopy(r, m_status.m_geUndo->GetTexture(), NULL, &undoRect);
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

		m_activeSourceFileItem->activeLine = ln;
		m_activeSourceFileItem->activeColumn = col;

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
	if (m_activeSourceFileItem)
	{
		gApp->GetCompiler()->LogContextualHelp(m_activeSourceFileItem->file, m_activeSourceFileItem->activeLine);
	}
	else
	{
		gApp->GetLogWindow()->ClearLog(LogWindow::LF_InstructionHelp);
	}

}

