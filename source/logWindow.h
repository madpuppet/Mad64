#pragma once

class LogWindow
{
public:
	LogWindow();
	~LogWindow();

	void SetRect(const SDL_Rect& area) { m_area = area; }

	enum LogFilter
	{
		LF_CompilerWarning,
		LF_InstructionHelp,
		LF_LabelHelp,
		LF_Memory,
		LF_MAX
	};

	void LogText(LogFilter filter, string text, int lineNmbr = -1, int col = 0);
	void LogTextArray(LogFilter filter, const char **textArray, int col);

	void ClearLog(LogFilter filter);
	void Update();
	void Draw();
	bool CalcScrollBar(int& start, int& end);
	int CalcLogHeight();
	void DrawLine(int lineIdx, int y, bool highlight);
	void SnapScrollBarToMouseY(int y);
	void OnMouseDown(SDL_Event* event);
	void OnMouseMotion(SDL_Event* event);
	void OnMouseWheel(SDL_Event* event);
	bool FindLogItemAt(int y, int& group, int& item);
	bool FindLogLineAt(int y, int& line);
	void ClampTargetScroll();
	void OnMouseUp(SDL_Event* e);

protected:
	SDL_Rect m_area;
	GraphicElement* m_geTitle[LF_MAX];
	GraphicChunk* m_gc[LF_MAX];
	vector<int> m_gcLines[LF_MAX];
	bool m_groupOpen[LF_MAX];

	int m_autoScroll;
	int m_scroll;
	float m_targetScroll;
	int m_highlightRow;

	enum DragMode
	{
		DRAG_None,
		DRAG_LogVertScroll
	};
	DragMode m_dragMode;
	int m_dragOffset;
};

