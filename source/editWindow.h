#pragma once

class EditWindow
{
public:
	EditWindow();

	void Update();
	void Draw();

	void OnResize();
	void OnEvent(SDL_Event* e);
	void OnKeyDown(SDL_Event* e);
	void OnMouseDown(SDL_Event* e);
	void OnMouseUp(SDL_Event* e);
	void OnMouseMotion(SDL_Event* e);
	void OnMouseWheel(SDL_Event* e);

	void OnFileLoaded(SourceFile *file);
	void OnFileClosed(SourceFile* file);
	void SetActiveFile(SourceFile* file);
	SourceFile* GetActiveFile() { return m_activeSourceFileItem ? m_activeSourceFileItem->file : nullptr;  }
	int GetActiveLine() { return m_activeSourceFileItem ? m_activeSourceFileItem->activeLine : 0; }
	int GetActiveCol() { return m_activeSourceFileItem ? m_activeSourceFileItem->activeColumn : 0; }

	void CursorUp(bool marking);
	void CursorLeft(bool marking);
	void CursorRight(bool marking);
	void CursorDown(bool marking);
	void GotoLineCol(int ln, int col);
	void ClearMarking() { m_marked = false; }

protected:
	void ClampTargetScroll();
	void CalcScrollBar(int& start, int& end);
	void CursorStart(bool marking);
	void CursorEnd(bool marking);
	void SelectCursor(int x, int y);
	void MakeActiveLineVisible();
	bool CheckLineMarked(int lineNmbr, int& startCol, int& endCol);
	bool MouseToRowCol(int x, int y, int& row, int& col);
	void ProcessMouseMarking(int x, int y);
	void SnapScrollBarToMouseY(int y);

	struct SourceFileItem
	{
		SourceFile* file;
		GraphicElement* geText;
		int scroll;
		float targetScroll;
		int activeLine, activeColumn, activeTargetX;
	};
	vector<SourceFileItem*> m_fileTabs;
	SourceFileItem* m_activeSourceFileItem;
	float m_cursorAnimTime;
	bool m_overwriteMode;

	bool m_keyMarking;
	bool m_mouseMarking;
	bool m_marked;
	int m_markStartLine;
	int m_markStartColumn;
	int m_markEndLine;
	int m_markEndColumn;

	// auto scrolling
	int m_autoScroll;
	int m_autoScroll_mouseX;
	int m_autoScroll_mouseY;

	enum DragMode
	{
		DRAG_None,
		DRAG_DivideDecode,
		DRAG_DivideText,
		DRAG_DivideContext,
		DRAG_EditVertScroll
	};
	DragMode m_dragMode;
	int m_dragOffset;

	void LayoutTabs();

	SDL_Rect m_titleTabsRect;
	SDL_Rect m_sourceEditRect;
	SDL_Rect m_memAddrRect;
	SDL_Rect m_decodeRect;
	SDL_Rect m_contextHelpRect;
	SDL_Rect m_statusRect;
	void CalcRects();

	SDL_Cursor* m_cursorArrow;
	SDL_Cursor* m_cursorIBeam;
	SDL_Cursor* m_cursorHoriz;
	SDL_Cursor* m_cursorVert;
	SDL_Cursor* m_cursorHand;

	// common resources
	SDL_Texture* m_tabActiveTex;
	SDL_Texture* m_tabInactiveTex;

	// status bar
	struct StatusInfo
	{
		bool overwriteMode;
		int line;
		int column;
		int totalLines;
		int totalColumns;

		GraphicElement* m_geOverwriteMode;
		GraphicElement* m_geLine;
		GraphicElement* m_geColumn;
	} m_status;
	void InitStatus();
	void UpdateStatus();
	void DrawStatus();
};


