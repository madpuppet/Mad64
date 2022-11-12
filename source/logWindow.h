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
		LF_Registers,
		LF_Emulator,
		LF_MAX
	};

	struct LogItem
	{
		LogItem() : line(-1), addr(0), ge(0) {}
		~LogItem()
		{
			delete ge;
		}

		GraphicElement* GetGE();

		string text;
		SDL_Color col;
		int line;
		u32 addr;

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

	struct MappedLogItem
	{
		SDL_Rect area;
		int group;
		int item;
	};

	void BuildIcons();
	void LayoutIcons();

	void LogText(LogFilter filter, string text, int lineNmbr = -1, int col = 0, u16 addr = 0);
	void LogTextArray(LogFilter filter, const char **textArray, int col);

	void ClearAllLogs();
	void ClearLog(LogFilter filter);
	void SetMemMap(u8* colorMap) { memcpy(m_memMap, colorMap, 65536); m_memMapDirty = true; }
	void Update();
	void Draw();
	bool CalcVertScrollBar(int& start, int& end);
	int CalcLogHeight();
	void DrawLine(int lineIdx, int y, bool highlight);
	void SnapScrollBarToMouseY(int y);
	void OnMouseDown(SDL_Event* event);
	void OnMouseMotion(SDL_Event* event);
	void OnMouseWheel(SDL_Event* event);

	// find which filter group toggle button is at x,y
	bool FindGroupItemAt(int x, int y, int& group);

	// find which log item is at an x,y location
	// for Memory, the item == memory address (0000..ffff)
	bool FindLogItemAt(int x, int y, int& group, int& item);

	// find the code line represented by log data at x,y
	bool FindLogLineAt(int x, int y, int& line);

	void ClampTargetScroll();
	void OnMouseUp(SDL_Event* e);
	void SelectCursor(int x, int y);
	void UpdateMemoryMap();

	string GetOpenLogs();
	void SetOpenLogs(const string& logs);

protected:
	SDL_Rect m_area;
	SDL_Rect m_titleArea;
	SDL_Rect m_logArea;

	LogGroup m_logGroups[LF_MAX];

	int m_autoScroll;
	int m_scroll;
	float m_targetScroll;
	int m_highlightRow;
	int m_emulatorZoom;

	enum DragMode
	{
		DRAG_None,
		DRAG_LogVertScroll
	};
	DragMode m_dragMode;
	int m_dragOffset;

	GraphicElement* GetGroupIconGE(LogFilter group);
	GraphicElement* GetGroupTitleGE(LogFilter group);

	// *** MEMORY SECTION ***
	u8* m_memMap;
	SDL_Texture* m_memMapTexture;
	bool m_memMapDirty;
	float m_markerAnim;

	vector<MappedLogItem> m_items;
};

