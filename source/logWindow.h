#pragma once

class LogWindow
{
public:
	LogWindow();
	~LogWindow();

	void SetRect(const SDL_Rect& area);

	enum LogFilter
	{
		LF_CompilerWarning,
		LF_InstructionHelp,
		LF_LabelHelp,
		LF_Memory,
		LF_MAX
	};

	struct LogItem
	{
		LogItem() : line(-1), ge(0) {}
		~LogItem()
		{
			delete ge;
		}

		GraphicElement* GetGE();

		string text;
		SDL_Color col;
		int line;

	protected:
		// created on demand
		GraphicElement* ge;
	};

	struct LogGroup
	{
		LogGroup() : m_geTitle(0), m_groupOpen(false) {}
		~LogGroup() 
		{
			delete m_geTitle;
			for (auto item : m_logLines)
				delete item;
		}

		GraphicElement* m_geTitle;
		GraphicElement* m_geIcon;

		bool m_groupOpen;
		vector<LogItem*> m_logLines;
	};

	void BuildIcons();
	void LayoutIcons();

	void LogText(LogFilter filter, string text, int lineNmbr = -1, int col = 0);
	void LogTextArray(LogFilter filter, const char **textArray, int col);

	void ClearAllLogs();
	void ClearLog(LogFilter filter);
	void SetMemMap(u8* colorMap) { memcpy(m_memMap, colorMap, 65536); m_memMapDirty = true; }
	void Update();
	void Draw();
	bool CalcScrollBar(int& start, int& end);
	int CalcLogHeight();
	void DrawLine(int lineIdx, int y, bool highlight);
	void SnapScrollBarToMouseY(int y);
	void OnMouseDown(SDL_Event* event);
	void OnMouseMotion(SDL_Event* event);
	void OnMouseWheel(SDL_Event* event);
	bool FindGroupItemAt(int x, int y, int& group);
	bool FindLogItemAt(int y, int& group, int& item);
	bool FindLogLineAt(int y, int& line);
	void ClampTargetScroll();
	void OnMouseUp(SDL_Event* e);
	void SelectCursor(int x, int y);
	void UpdateMemoryMap();

protected:
	SDL_Rect m_area;
	SDL_Rect m_titleArea;
	SDL_Rect m_logArea;

	LogGroup m_logGroups[LF_MAX];

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

	GraphicElement* GetGroupIconGE(LogFilter group);
	GraphicElement* GetGroupTitleGE(LogFilter group);

	u8* m_memMap;
	SDL_Texture* m_memMapTexture;
	bool m_memMapDirty;
};

