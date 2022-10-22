#pragma once

class LogWindow
{
public:
	LogWindow();
	~LogWindow();

	void SetRect(const SDL_Rect &area) { m_area = area; }

	enum LogFilter
	{
		LF_CompilerWarning,
		LF_LabelHelp,
		LF_InstructionHelp,

		LF_MAX
	};

	void LogText(LogFilter filter, string text, int lineNmbr = -1);
	void ClearLog(LogFilter filter);
	void Draw();

	void OnMouseDown(SDL_Event* event);

protected:
	SDL_Rect m_area;
	GraphicChunk* m_gc[LF_MAX];
	vector<int> m_gcLines[LF_MAX];
};

