#include "common.h"
#include "LogWindow.h"

static const char* s_titles[] = { "Warnings", "Labels", "Instruction Details" };

LogWindow::LogWindow()
{
	for (int i = 0; i < LF_MAX; i++)
	{
		m_gc[i] = new GraphicChunk();
	}
}

LogWindow::~LogWindow()
{
	for (int i = 0; i < LF_MAX; i++)
	{
		delete m_gc[i];
	}
}

void LogWindow::LogText(LogFilter filter, string text, int lineNmbr)
{
	auto settings = gApp->GetSettings();
	m_gc[filter]->Add(GraphicElement::CreateFromText(gApp->GetFont(), text.c_str(), settings->commentColor, 0, (m_gc[filter]->Size()) * settings->lineHeight));
	m_gcLines[filter].push_back(lineNmbr);
}

void LogWindow::ClearLog(LogFilter filter)
{
	auto settings = gApp->GetSettings();
	m_gc[filter]->Clear();
	m_gc[filter]->Add(GraphicElement::CreateFromText(gApp->GetFont(), s_titles[filter], settings->opCodeColor, 0, 0));
	m_gcLines[filter].clear();
	m_gcLines[filter].push_back(-1);
}

void LogWindow::Draw()
{
	auto settings = gApp->GetSettings();
	int y = m_area.y + settings->textYMargin;
	for (int i = 0; i < LF_MAX; i++)
	{
		if (m_gc[i]->Size() > 1)
		{
			m_gc[i]->DrawAt(m_area.x + settings->textXMargin, y);
			y += settings->lineHeight * m_gc[i]->Size() + settings->lineHeight;
		}
	}
}

void LogWindow::OnMouseDown(SDL_Event* event)
{
	auto settings = gApp->GetSettings();
	int row = (event->button.y - m_area.y) / settings->lineHeight;
	for (int i = 0; i < LF_MAX && row > 0; i++)
	{
		if (row < m_gcLines[i].size())
		{
			int jumpTo = (m_gcLines[i][row]);
			if (jumpTo != -1)
			{
				gApp->GetEditWindow()->GotoLineCol(jumpTo, 0, MARK_None, true);
			}
			return;
		}
		row -= (int)m_gcLines[i].size();
	}

}

