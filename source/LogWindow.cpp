#include "common.h"
#include "LogWindow.h"

static const char* s_titles[] = { "Contextual Help", "Compiler", "Labels", "Memory"};

LogWindow::LogWindow()
{
	auto settings = gApp->GetSettings();
	for (int i = 0; i < LF_MAX; i++)
	{
		m_gc[i] = new GraphicChunk();
		m_groupOpen[i] = true;
		m_geTitle[i] = 0;
	}
	m_dragMode = DRAG_None;
	m_scroll = 0;
	m_targetScroll = 0.0f;
	m_highlightRow = -1;
	m_autoScroll = 0;
}

LogWindow::~LogWindow()
{
	for (int i = 0; i < LF_MAX; i++)
	{
		delete m_gc[i];
	}
}

void LogWindow::LogText(LogFilter filter, string text, int lineNmbr, int colIdx)
{
	auto settings = gApp->GetSettings();
	SDL_Color color = (colIdx == 0) ? settings->helpTitleColor : settings->helpBodyColor;
	m_gc[filter]->Add(GraphicElement::CreateFromText(gApp->GetFont(), text.c_str(), color, 0, 0));
	m_gcLines[filter].push_back(lineNmbr);
}

void LogWindow::ClearLog(LogFilter filter)
{
	auto settings = gApp->GetSettings();
	m_gc[filter]->Clear();
	m_gcLines[filter].clear();

	if (m_geTitle[filter])
	{
		delete m_geTitle[filter];
		m_geTitle[filter] = 0;
	}
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
	SDL_Rect lineQuad = { m_area.x, y, m_area.w, settings->lineHeight };
	SDL_SetRenderDrawColor(r, col, col, col, 255);
	SDL_RenderFillRect(r, &lineQuad);
}

void LogWindow::Draw()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();

	SDL_RenderSetClipRect(r, &m_area);

	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &m_area);

	int y = m_area.y - m_scroll;
	int lineIdx = 0;
	int fileHeight = CalcLogHeight();
	int startLine = max(0, m_scroll / settings->lineHeight);
	int endLine = min((m_area.h + m_scroll) / settings->lineHeight + 1, fileHeight);
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& gc = m_gc[i];
		if (lineIdx >= startLine && lineIdx < endLine)
		{
			if (m_geTitle[i] == nullptr)
			{
				if (m_gcLines[i].empty())
				{
					SDL_Color col = settings->helpGroupColor;
					col.r = col.r * 2 / 3;
					col.g = col.g * 2 / 3;
					col.b = col.b * 2 / 3;
					m_geTitle[i] = GraphicElement::CreateFromText(gApp->GetFont(), FormatString("%c %s", m_groupOpen[i] ? '-' : '+', s_titles[i]).c_str(), col, 0, 0);
				}
				else
				{
					SDL_Color col = settings->helpGroupColor;
					m_geTitle[i] = GraphicElement::CreateFromText(gApp->GetFont(), FormatString("%c %s (%d)", m_groupOpen[i] ? '-' : '+', s_titles[i], m_gcLines[i].size()).c_str(), col, 0, 0);
				}
			}

			DrawLine(lineIdx, y, lineIdx == m_highlightRow);
			auto ge = m_geTitle[i];
			SDL_Rect quad = { ge->GetRect().x + m_area.x + settings->textXMargin, ge->GetRect().y + y + settings->textYMargin, ge->GetRect().w, ge->GetRect().h };
			SDL_RenderCopy(gApp->GetRenderer(), ge->GetTexture(), NULL, &quad);
		}
		y += settings->lineHeight;

		lineIdx++;

		if (m_groupOpen[i])
		{
			for (int ii = 0; ii < gc->Size(); ii++)
			{
				if (lineIdx >= startLine && lineIdx < endLine)
				{
					DrawLine(lineIdx, y, lineIdx == m_highlightRow);
					gc->DrawElemAt(ii, m_area.x + settings->textXMargin, y + settings->textYMargin);
				}
				y += settings->lineHeight;
				lineIdx++;
			}
		}
	}

	// draw scroll bar
	int barY1, barY2;
	CalcScrollBar(barY1, barY2);

	SDL_Rect BarBack = { m_area.x + m_area.w - settings->scrollBarWidth, m_area.y, settings->scrollBarWidth, m_area.h };
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
	int y = m_area.y + settings->textYMargin;
	for (int i = 0; i < LF_MAX; i++)
	{
		if (m_gc[i]->Size() > 1)
		{
			y += settings->lineHeight * m_gc[i]->Size() + settings->lineHeight;
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
	int viewEnd = m_scroll + m_area.h;
	float relStart = SDL_clamp((float)viewStart / (float)fileHeight, 0.0f, 1.0f);
	float relEnd = SDL_clamp((float)viewEnd / (float)fileHeight, 0.0f, 1.0f);
	start = (int)(m_area.y + relStart * m_area.h);
	end = (int)(m_area.y + relEnd * m_area.h);

	return relStart > 0.0f || relEnd < 1.0f;
}
bool LogWindow::FindLogLineAt(int y, int& line)
{
	int group, item;
	if (FindLogItemAt(y, group, item))
	{
		if (item != -1)
		{
			int line = m_gcLines[group][item];
			return line != -1;
		}
	}
	return false;
}

bool LogWindow::FindLogItemAt(int y, int& group, int& item)
{
	auto settings = gApp->GetSettings();
	int row = (y - m_area.y + m_scroll) / settings->lineHeight ;
	for (int i = 0; i < LF_MAX && row >= 0; i++)
	{
		if (m_groupOpen[i])
		{
			if (row < m_gcLines[i].size()+1)
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
			row -= (int)m_gcLines[i].size() + 1;
		}
		else
		{
			if (row == 0)
			{
				group = i;
				item = -1;
				return true;
			}
			row--;
		}
	}
	return false;
}

void LogWindow::SnapScrollBarToMouseY(int y)
{
	float rel = (float)(y - m_area.y) / (float)m_area.h;
	int fileHeight = CalcLogHeight();
	m_targetScroll = fileHeight * rel - m_area.h * 0.5f;
	ClampTargetScroll();
}

void LogWindow::LogTextArray(LogFilter filter, const char** textArray)
{
	const char** t = textArray;
	int colIdx = 0;
	while (*t)
	{
		LogText(filter, string(*t), -1, colIdx);
		colIdx = 1;
		t++;
	}
}

void LogWindow::OnMouseDown(SDL_Event* event)
{
	int group, item;
	if (FindLogItemAt(event->button.y, group, item))
	{
		if (item == -1)
		{
			m_groupOpen[group] = !m_groupOpen[group];
			if (m_geTitle[group])
			{
				delete m_geTitle[group];
				m_geTitle[group] = 0;
			}
		}
		else
		{
			int jumpTo = m_gcLines[group][item];
			if (jumpTo != -1)
				gApp->GetEditWindow()->GotoLineCol(jumpTo, 0, MARK_None, true);
		}
	}
}

void LogWindow::OnMouseMotion(SDL_Event* event)
{
	auto settings = gApp->GetSettings();
	int x = event->motion.x;
	int y = event->motion.y;

	if (x < m_area.x || y >= m_area.x + m_area.w)
		m_highlightRow = -1;
	else
		m_highlightRow = (y - m_area.y + m_scroll) / settings->lineHeight;
}


void LogWindow::OnMouseWheel(SDL_Event* e)
{
	m_targetScroll += (e->wheel.preciseY * -80.0f);
	ClampTargetScroll();
}

void LogWindow::ClampTargetScroll()
{
	int fileHeight = CalcLogHeight();
	int maxScroll = max(0, fileHeight - m_area.h);
	m_targetScroll = SDL_clamp(m_targetScroll, 0.0f, (float)maxScroll);
}

void LogWindow::OnMouseUp(SDL_Event* e)
{
	m_autoScroll = 0;
}

