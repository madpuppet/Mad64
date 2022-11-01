#include "common.h"
#include "logWindow.h"

static const char* s_titles[] = { "Compiler", "Contextual Help", "Labels", "Memory"};
static const char* s_short_titles[] = { "COMPILER", "HELP", "LABELS", "MEMORY" };

LogWindow::LogWindow()
{
	auto settings = gApp->GetSettings();
	for (int i = 0; i < LF_MAX; i++)
	{
		m_logGroups[i].m_groupOpen = true;
		m_logGroups[i].m_geTitle = 0;
		m_logGroups[i].m_geIcon = 0;
	}
	m_dragMode = DRAG_None;
	m_scroll = 0;
	m_targetScroll = 0.0f;
	m_highlightRow = -1;
	m_autoScroll = 0;
	m_logGroups[2].m_groupOpen = false;

	BuildIcons();
}

LogWindow::~LogWindow()
{
}

GraphicElement* LogWindow::GetGroupTitleGE(LogFilter group)
{
	auto& lg = m_logGroups[group];

	if (!lg.m_geTitle)
	{
		auto settings = gApp->GetSettings();
		SDL_Color col = settings->helpGroupColor;
		lg.m_geTitle = GraphicElement::CreateFromText(gApp->GetFont(), s_titles[group], col, 0, 0);

		if (lg.m_geTitle == 0)
		{
			Log("FAILED TO CREATE GRAPHIC ELEMENT");
		}

	}
	return lg.m_geTitle;
}

void LogWindow::LogText(LogFilter filter, string text, int lineNmbr, int colIdx)
{
	auto settings = gApp->GetSettings();
	SDL_Color color;
	switch (colIdx)
	{
		case 0:
			color = settings->helpTitleColor;
			break;
		case 1:
			color = settings->helpBodyColor1;
			break;
		case 2:
			color = settings->helpBodyColor2;
			break;
	}

	auto logItem = new LogItem();
	logItem->col = color;
	logItem->text = text;
	logItem->line = lineNmbr;
	m_logGroups[filter].m_logLines.push_back(logItem);
}

void LogWindow::ClearAllLogs()
{
	for (int i = 0; i < LF_MAX; i++)
		ClearLog((LogFilter)i);
}

void LogWindow::ClearLog(LogFilter filter)
{
	auto settings = gApp->GetSettings();
	auto &lg = m_logGroups[filter];

	for (auto line : lg.m_logLines)
		delete line;
	lg.m_logLines.clear();
	ClampTargetScroll();
}

void LogWindow::BuildIcons()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();
	SDL_Color col = { 255, 255, 255, 255 };
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];
		lg.m_geIcon = GraphicElement::CreateFromText(gApp->GetFont(), s_short_titles[i], col, 0, 0);
	}
}

void LogWindow::LayoutIcons()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();
	int iconX = m_titleArea.x + settings->textXMargin;
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];
		lg.m_geIcon->SetPos(iconX, m_titleArea.y + settings->textYMargin);
		iconX += settings->textXMargin*2 + lg.m_geIcon->GetRect().w;
	}
}

GraphicElement *LogWindow::LogItem::GetGE()
{
	if (!ge)
		ge = GraphicElement::CreateFromText(gApp->GetFont(), text.c_str(), col, 0, 0);
	return ge;
}

void LogWindow::Update()
{
	if (m_autoScroll)
	{
		m_targetScroll += TIMEDELTA * m_autoScroll;
		ClampTargetScroll();
	}
	m_scroll += (int)((m_targetScroll - (float)m_scroll) * 0.25f);
}

void LogWindow::DrawLine(int lineIdx, int y, bool highlight)
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();

	int brighten = highlight ? 16 : 0;
	int col = 24 - ((lineIdx & 1) ? 4 : 0) + brighten;
	SDL_Rect lineQuad = { m_logArea.x, y, m_logArea.w, settings->lineHeight };
	SDL_SetRenderDrawColor(r, col, col, col, 255);
	SDL_RenderFillRect(r, &lineQuad);
}

void LogWindow::Draw()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();

	int y = m_logArea.y - m_scroll;
	int lineIdx = 0;
	int fileHeight = CalcLogHeight();
	int startLine = max(0, m_scroll / settings->lineHeight);
	int endLine = min((m_logArea.h + m_scroll) / settings->lineHeight + 1, fileHeight);

	// draw toggle buttons
	SDL_RenderSetClipRect(r, &m_titleArea);
	SDL_SetRenderDrawColor(r, 64, 32, 0, 255);
	SDL_RenderFillRect(r, &m_titleArea);

	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];

		SDL_SetRenderDrawColor(r, 64, 32, 0, 255);
		SDL_RenderFillRect(r, &lg.m_geIcon->GetRect());

		if (lg.m_groupOpen)
		{
			SDL_SetTextureColorMod(lg.m_geIcon->GetTexture(), 255, 255, 0);
		}
		else
		{
			SDL_SetTextureColorMod(lg.m_geIcon->GetTexture(), 96, 96, 0);
		}
		lg.m_geIcon->Render(r);
	}

	SDL_RenderSetClipRect(r, &m_logArea);
	for (int i = 0; i < LF_MAX; i++)
	{
		auto &lg = m_logGroups[i];
		if (!lg.m_groupOpen)
			continue;

		int x = m_logArea.x + settings->textXMargin;
		if (lineIdx >= startLine && lineIdx < endLine)
		{
			DrawLine(lineIdx, y, lineIdx == m_highlightRow);
			GetGroupTitleGE((LogFilter)i)->RenderAt(r, x, y + settings->textYMargin);
			lineIdx++;
			y += settings->lineHeight;
		}

		for (auto line : lg.m_logLines)
		{
			DrawLine(lineIdx, y, lineIdx == m_highlightRow);
			line->GetGE()->RenderAt(r, x, y + settings->textYMargin);
			lineIdx++;
			y += settings->lineHeight;
		}
	}

	// draw scroll bar
	int barY1, barY2;
	CalcScrollBar(barY1, barY2);

	SDL_Rect BarBack = { m_logArea.x + m_logArea.w - settings->scrollBarWidth, m_logArea.y, settings->scrollBarWidth, m_logArea.h };
	SDL_Rect Bar = { BarBack.x + 4, barY1, settings->scrollBarWidth - 4, barY2 - barY1 };
	SDL_SetRenderDrawColor(r, 0, 0, 32, 255);
	SDL_RenderFillRect(r, &BarBack);
	if (m_dragMode == DRAG_LogVertScroll)
	{
		SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
	}
	else
	{
		SDL_SetRenderDrawColor(r, 64, 64, 255, 255);
	}
	SDL_RenderFillRect(r, &Bar);
	SDL_RenderSetClipRect(r, nullptr);
}

int LogWindow::CalcLogHeight()
{
	auto settings = gApp->GetSettings();
	int y = 0;
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[(LogFilter)i];
		if (lg.m_groupOpen)
		{
			y += settings->lineHeight * (int)lg.m_logLines.size() + settings->lineHeight;
		}
	}
	return y;
}

bool LogWindow::CalcScrollBar(int& start, int& end)
{
	int lineHeight = gApp->GetSettings()->lineHeight;
	start = 0;
	end = 0;
	int fileHeight = CalcLogHeight();

	int viewStart = m_scroll;
	int viewEnd = m_scroll + m_logArea.h;
	float relStart = SDL_clamp((float)viewStart / (float)fileHeight, 0.0f, 1.0f);
	float relEnd = SDL_clamp((float)viewEnd / (float)fileHeight, 0.0f, 1.0f);
	start = (int)(m_logArea.y + relStart * m_logArea.h);
	end = (int)(m_logArea.y + relEnd * m_logArea.h);

	return relStart > 0.0f || relEnd < 1.0f;
}
bool LogWindow::FindLogLineAt(int y, int& line)
{
	int group, item;
	if (FindLogItemAt(y, group, item))
	{
		if (item != -1)
		{
			int line = m_logGroups[group].m_logLines[item]->line;
			return line != -1;
		}
	}
	return false;
}

bool LogWindow::FindGroupItemAt(int x, int y, int& group)
{
	auto settings = gApp->GetSettings();
	if (Contains(m_titleArea, x, y))
	{
		for (int i = 0; i < LF_MAX; i++)
		{
			if (Contains(m_logGroups[i].m_geIcon->GetRect(), x, y))
			{
				group = i;
				return true;
			}
		}
	}
	group = -1;
	return false;
}

bool LogWindow::FindLogItemAt(int y, int& group, int& item)
{
	auto settings = gApp->GetSettings();
	int row = (y - m_logArea.y + m_scroll) / settings->lineHeight ;
	for (int i = 0; i < LF_MAX && row >= 0; i++)
	{
		auto& lg = m_logGroups[i];
		if (lg.m_groupOpen)
		{
			if (row < lg.m_logLines.size()+1)
			{
				group = i;
				if (row == 0)
				{
					item = -1;
				}
				else
				{
					item = row - 1;
				}
				return true;
			}
			row -= (int)lg.m_logLines.size() + 1;
		}
	}
	return false;
}

void LogWindow::SnapScrollBarToMouseY(int y)
{
	float rel = (float)(y - m_logArea.y) / (float)m_logArea.h;
	int fileHeight = CalcLogHeight();
	m_targetScroll = fileHeight * rel - m_logArea.h * 0.5f;
	ClampTargetScroll();
}

void LogWindow::LogTextArray(LogFilter filter, const char** textArray, int col)
{
	const char** t = textArray;
	while (*t)
	{
		LogText(filter, string(*t), -1, col);
		t++;
	}
}

void LogWindow::SelectCursor(int x, int y)
{
	if (Contains(m_titleArea, x, y))
	{
		for (auto &group : m_logGroups)
		{
			if (Contains(group.m_geIcon->GetRect(), x, y))
			{
				gApp->SetCursor(Cursor_Hand);
				return;
			}
		}
	}
	else if (Contains(m_logArea, x, y))
	{
		int line;
		if (gApp->GetLogWindow()->FindLogLineAt(y, line))
		{
			gApp->SetCursor(Cursor_Hand);
			return;
		}
	}
	gApp->SetCursor(Cursor_Arrow);
}

void LogWindow::OnMouseDown(SDL_Event* event)
{
	int group, item;
	if (FindGroupItemAt(event->button.x, event->button.y, group))
	{
		if (event->button.button == 3)
		{
			for (int i = 0; i < LF_MAX; i++)
				m_logGroups[i].m_groupOpen = (group == i);
		}
		else
		{
			m_logGroups[group].m_groupOpen = !m_logGroups[group].m_groupOpen;
		}
		ClampTargetScroll();
	}
	else if (FindLogItemAt(event->button.y, group, item))
	{
		if (item != -1)
		{
			int jumpTo = m_logGroups[group].m_logLines[item]->line;
			if (jumpTo != -1)
				gApp->GetEditWindow()->GotoLineCol(jumpTo, 0, MARK_None, true);
		}
	}
}

void LogWindow::SetRect(const SDL_Rect& area)
{
	auto settings = gApp->GetSettings();
	m_area = area;
	m_titleArea = { area.x, area.y, area.w, settings->lineHeight };
	m_logArea = { area.x, area.y + settings->lineHeight, area.w, area.h - settings->lineHeight };
	LayoutIcons();
}

void LogWindow::OnMouseMotion(SDL_Event* event)
{
	auto settings = gApp->GetSettings();
	int x = event->motion.x;
	int y = event->motion.y;

	if (x < m_logArea.x || y >= m_logArea.x + m_logArea.w)
		m_highlightRow = -1;
	else
		m_highlightRow = (y - m_logArea.y + m_scroll) / settings->lineHeight;
}


void LogWindow::OnMouseWheel(SDL_Event* e)
{
	m_targetScroll += (e->wheel.preciseY * -80.0f);
	ClampTargetScroll();
}

void LogWindow::ClampTargetScroll()
{
	int fileHeight = CalcLogHeight();
	int maxScroll = max(0, fileHeight - m_logArea.h);
	m_targetScroll = SDL_clamp(m_targetScroll, 0.0f, (float)maxScroll);
}

void LogWindow::OnMouseUp(SDL_Event* e)
{
	m_autoScroll = 0;
}

