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
	m_overwriteMode = false;
	m_dragMode = DRAG_None;

	CalcRects();
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
			fore = { 255,255,255,255 };
		}
		else
		{
			back = { 32,32,64,255 };
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

		// draw text
		SDL_RenderSetClipRect(r, &m_sourceEditRect);

		for (int i = startLine; i < endLine; i++)
		{
			auto line = file->GetLines()[i];
			int y = m_sourceEditRect.y + i * settings->lineHeight - m_activeSourceFileItem->scroll;
			int lineWidth = line->GetLineWidth() + settings->textXMargin;
			SDL_Rect lineQuad1 = { settings->xPosText, y, lineWidth, settings->lineHeight };
			SDL_Rect lineQuad2 = { settings->xPosText + lineWidth, y, m_sourceEditRect.w - lineWidth, settings->lineHeight };

			SDL_SetRenderDrawColor(r, 0, 0, 128 - ((i & 1) ? 16 : 0), 255);
			SDL_RenderFillRect(r, &lineQuad1);
			if (lineWidth < m_sourceEditRect.w)
			{
				SDL_SetRenderDrawColor(r, 0, 0, 80 - ((i & 1) ? 16 : 0), 255);
				SDL_RenderFillRect(r, &lineQuad2);
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

			// draw cursor
			if (i == m_activeSourceFileItem->activeLine)
			{
				int cursorX1, cursorX2;
				line->GetCharX(m_activeSourceFileItem->activeColumn, cursorX1, cursorX2);
				int cursorY = m_sourceEditRect.y + m_activeSourceFileItem->activeLine * settings->lineHeight - m_activeSourceFileItem->scroll;
				int brightness = max(0, (int)(100 + cosf(m_cursorAnimTime) * 128));
				SDL_Rect cursorRect = { m_sourceEditRect.x + settings->textXMargin + cursorX1, cursorY, m_overwriteMode ? (cursorX2-cursorX1) : 4, settings->lineHeight };
				SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(r, 255, 255, 255, brightness);
				SDL_RenderFillRect(r, &cursorRect);
				SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
			}

			// - mem addr
			// - machine code
			// - text
			if (line->GetGCText())
				line->GetGCText()->DrawAt(settings->xPosText + settings->textXMargin, y + settings->textYMargin);
		}

		// draw scroll bar
		int barY1, barY2;
		CalcScrollBar(barY1, barY2);

		SDL_Rect BarBack = { settings->xPosContextHelp - 16, m_sourceEditRect.y, 16, m_sourceEditRect.h };
		SDL_Rect Bar = { settings->xPosContextHelp - 16, barY1, 16, barY2-barY1 };
		SDL_SetRenderDrawColor(r, 0, 0, 32, 255);
		SDL_RenderFillRect(r, &BarBack);
		SDL_SetRenderDrawColor(r, 64, 64, 255, 255);
		SDL_RenderFillRect(r, &Bar);
	}

	SDL_RenderSetClipRect(r, nullptr);

	// - context help

	// draw status bar
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &m_statusRect);

	// draw separator bars
	DrawColouredLine(settings->xPosDecode, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(settings->xPosText, m_sourceEditRect.y, m_sourceEditRect.y+m_sourceEditRect.h, false);
	DrawColouredLine(settings->xPosContextHelp, m_sourceEditRect.y, m_sourceEditRect.y + m_sourceEditRect.h, false);

	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
}

void EditWindow::OnResize()
{
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
	m_contextHelpRect = { settings->xPosContextHelp, settings->lineHeight, windowWidth - settings->xPosContextHelp, editHeight };
	m_statusRect = { 0, windowHeight - settings->lineHeight, windowWidth, settings->lineHeight };
}


void EditWindow::CalcScrollBar(int &start, int &end)
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
	}
}


void EditWindow::OnFileLoaded(SourceFile* file)
{
	// add new file tab
	auto sfi = new SourceFileItem();
	sfi->file = file;
	sfi->geText = GraphicElement::CreateFromText(gApp->GetFont(), file->GetName(), { 255,255,255,255 }, 0, 0);
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


	// remove file tab

}

void EditWindow::OnMouseWheel(SDL_Event* e)
{
	if (m_activeSourceFileItem)
	{
		m_activeSourceFileItem->targetScroll += (int)(e->wheel.preciseY * -80);
		ClampTargetScroll();
	}
}

void EditWindow::ClampTargetScroll()
{
	if (m_activeSourceFileItem)
	{
		int fileHeight = gApp->GetSettings()->lineHeight * (int)m_activeSourceFileItem->file->GetLines().size();
		int maxScroll = max(0, fileHeight - m_sourceEditRect.h);
		m_activeSourceFileItem->targetScroll = SDL_clamp(m_activeSourceFileItem->targetScroll, 0, maxScroll);
	}
}

void EditWindow::Update()
{
	m_cursorAnimTime += TIMEDELTA * 10.0f;

	auto s = gApp->GetSettings();
	if (m_activeSourceFileItem)
	{
		int dy = m_activeSourceFileItem->targetScroll - m_activeSourceFileItem->scroll;
		int signDy = sign(dy);
		int amount = dy / 4;
		if (amount == 0)
			amount = signDy;
		m_activeSourceFileItem->scroll += amount;
	}
}

bool Contains(const SDL_Rect& rect, int x, int y)
{
	return (x >= rect.x && x < (rect.x + rect.w) && y >= rect.y && y < (rect.y + rect.h));
}

void EditWindow::OnMouseDown(SDL_Event* e)
{
	auto settings = gApp->GetSettings();
	if (e->button.button == 1)
	{
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
		else if (e->button.x - settings->xPosDecode)
		{
			// drag first divide
			m_dragMode = DRAG_DivideDecode;
			m_dragOffset = e->button.x - settings->xPosDecode;
		}
		else if (e->button.x - settings->xPosText)
		{
			// drag second divide
			m_dragMode = DRAG_DivideText;
			m_dragOffset = e->button.x - settings->xPosText;
		}
		else if (e->button.x - settings->xPosContextHelp)
		{
			// drag third divide
			m_dragMode = DRAG_DivideContext;
			m_dragOffset = e->button.x - settings->xPosContextHelp;
		}
		else if (Contains(m_sourceEditRect, e->button.x, e->button.y))
		{

		}
	}
}
void EditWindow::OnMouseUp(SDL_Event* e)
{

}
void EditWindow::OnMouseMotion(SDL_Event* e)
{

}

void EditWindow::OnKeyDown(SDL_Event* e)
{
	bool shiftHeld = e->key.keysym.mod & KMOD_SHIFT;
	switch (e->key.keysym.sym)
	{
	case SDLK_PAGEUP:
		for (int i = 0; i < 20; i++)
			CursorUp(shiftHeld);
		return;
	case SDLK_PAGEDOWN:
		for (int i = 0; i < 20; i++)
			CursorDown(shiftHeld);
		return;
	case SDLK_UP:
		CursorUp(shiftHeld);
		return;
	case SDLK_DOWN:
		CursorDown(shiftHeld);
		return;
	case SDLK_LEFT:
		CursorLeft(shiftHeld);
		return;
	case SDLK_RIGHT:
		CursorRight(shiftHeld);
		return;
	case SDLK_END:
		CursorEnd(shiftHeld);
		return;
	case SDLK_HOME:
		CursorStart(shiftHeld);
		break;
	}
}

void EditWindow::MakeActiveLineVisible()
{
	if (m_activeSourceFileItem)
	{
		auto settings = gApp->GetSettings();
		auto file = m_activeSourceFileItem->file;
		int viewY1 = m_activeSourceFileItem->targetScroll;
		int viewY2 = m_activeSourceFileItem->targetScroll + m_sourceEditRect.h;
		int y = m_activeSourceFileItem->activeLine * settings->lineHeight;
		if (y < viewY1)
			m_activeSourceFileItem->targetScroll = y - settings->lineHeight * 4;
		if (y >= viewY2)
			m_activeSourceFileItem->targetScroll = y - m_sourceEditRect.h + settings->lineHeight * 4;
		ClampTargetScroll();
	}
}

void EditWindow::CursorUp(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
		}

		auto file = m_activeSourceFileItem->file;
		if (m_activeSourceFileItem->activeLine > 0)
		{
			auto oldLine = file->GetLines()[m_activeSourceFileItem->activeLine];
			m_activeSourceFileItem->activeLine--;
			auto newLine = file->GetLines()[m_activeSourceFileItem->activeLine];
			m_activeSourceFileItem->activeColumn = newLine->GetColumnAtX(m_activeSourceFileItem->activeTargetX);

			MakeActiveLineVisible();
		}
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

void EditWindow::CursorDown(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_marking = true;
		}

		auto file = m_activeSourceFileItem->file;
		if (m_activeSourceFileItem->activeLine < file->GetLines().size() - 1)
		{
			auto oldLine = file->GetLines()[m_activeSourceFileItem->activeLine];
			m_activeSourceFileItem->activeLine++;
			auto newLine = file->GetLines()[m_activeSourceFileItem->activeLine];
			m_activeSourceFileItem->activeColumn = newLine->GetColumnAtX(m_activeSourceFileItem->activeTargetX);

			MakeActiveLineVisible();
		}
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

void EditWindow::CursorLeft(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_marking = true;
		}

		auto file = m_activeSourceFileItem->file;
		if (m_activeSourceFileItem->activeColumn > 0)
			m_activeSourceFileItem->activeColumn--;
		else if (m_activeSourceFileItem->activeLine > 0)
		{
			m_activeSourceFileItem->activeLine--;
			auto line = file->GetLines()[m_activeSourceFileItem->activeLine];
			m_activeSourceFileItem->activeColumn = (int)line->GetChars().size();
			MakeActiveLineVisible();
		}

		int x1, x2;
		file->GetLines()[m_activeSourceFileItem->activeLine]->GetCharX(m_activeSourceFileItem->activeColumn, x1, x2);
		m_activeSourceFileItem->activeTargetX = x1 + 1;
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

void EditWindow::CursorRight(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_marking = true;
		}

		auto file = m_activeSourceFileItem->file;
		if (m_activeSourceFileItem->activeColumn < (file->GetLines()[m_activeSourceFileItem->activeLine]->GetChars().size()))
			m_activeSourceFileItem->activeColumn++;
		else if (m_activeSourceFileItem->activeLine < file->GetLines().size()-1)
		{
			m_activeSourceFileItem->activeLine++;
			m_activeSourceFileItem->activeColumn = 0;
			MakeActiveLineVisible();
		}

		int x1, x2;
		file->GetLines()[m_activeSourceFileItem->activeLine]->GetCharX(m_activeSourceFileItem->activeColumn, x1, x2);
		m_activeSourceFileItem->activeTargetX = x1 + 1;
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

void EditWindow::CursorStart(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_marking = true;
		}

		auto file = m_activeSourceFileItem->file;
		m_activeSourceFileItem->activeColumn = 0;

		int x1, x2;
		file->GetLines()[m_activeSourceFileItem->activeLine]->GetCharX(m_activeSourceFileItem->activeColumn, x1, x2);
		m_activeSourceFileItem->activeTargetX = x1 + 1;
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

void EditWindow::CursorEnd(bool marking)
{
	if (m_activeSourceFileItem)
	{
		if (marking && !m_marking)
		{
			m_markStartLine = m_activeSourceFileItem->activeLine;
			m_markStartColumn = m_activeSourceFileItem->activeColumn;
			m_marking = true;
		}

		auto file = m_activeSourceFileItem->file;
		m_activeSourceFileItem->activeColumn = (int)file->GetLines()[m_activeSourceFileItem->activeLine]->GetChars().size();

		int x1, x2;
		file->GetLines()[m_activeSourceFileItem->activeLine]->GetCharX(m_activeSourceFileItem->activeColumn, x1, x2);
		m_activeSourceFileItem->activeTargetX = x1 + 1;
		m_cursorAnimTime = 0;

		if (marking)
		{
			m_markEndLine = m_activeSourceFileItem->activeLine;
			m_markEndColumn = m_activeSourceFileItem->activeColumn;
		}
		m_marking = marking;
	}
}

bool EditWindow::CheckLineMarked(int lineNmbr, int& startCol, int& endCol)
{
	if (m_activeSourceFileItem && m_marking)
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



