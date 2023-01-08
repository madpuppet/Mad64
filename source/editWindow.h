#pragma once

enum MarkingType
{
	MARK_None,
	MARK_Key,
	MARK_Mouse
};

class EditWindow
{
public:
	EditWindow();

	void Update();
	void Draw();

	void OnResize();
	void OnEvent(SDL_Event* e);
	void OnKeyDown(SDL_Event* e);
	void OnKeyUp(SDL_Event* e);
	void OnTextInput(SDL_Event* e);

	// return TRUE to capture input to this window in future
	void OnMouseDown(SDL_Event* e);

	void OnMouseUp(SDL_Event* e);
	void OnMouseMotion(SDL_Event* e);
	void OnMouseWheel(int windowID, int mouseX, int mouseY, int wheelX, int wheelY);
	void UpdateCursor(int x, int y);

	void OnFileLoaded(SourceFile *file);
	void OnFileClosed(SourceFile* file);
	void SetActiveFile(SourceFile* file);
	void SetActiveFileIdx(int index);
	SourceFile* GetActiveFile() { return m_activeSourceFileItem ? m_activeSourceFileItem->file : nullptr;  }
	int GetActiveLine() { return m_activeSourceFileItem ? m_activeSourceFileItem->activeLine : 0; }
	int GetActiveCol() { return m_activeSourceFileItem ? m_activeSourceFileItem->activeColumn : 0; }

	void CursorUp(MarkingType marking);
	void CursorLeft(MarkingType marking);
	void CursorRight(MarkingType marking);
	void CursorDown(MarkingType marking);
	void CursorStart(MarkingType marking);
	void CursorEnd(MarkingType marking);
	void CursorTopOfFile(MarkingType marking);
	void CursorBottomOfFile(MarkingType marking);

	void GotoEmuPC();
	void GotoLineCol(int ln, int col, MarkingType mark, bool trackXPos);
	void ClearMarking() { m_marked = false; m_keyMarking = false; m_mouseMarking = false; }
	void SetMarking(int startLine, int startColumn, int endLine, int endColumn)
	{
		m_marked = true;
		m_markStartLine = startLine;
		m_markStartColumn = startColumn;
		m_markEndLine = endLine;
		m_markEndColumn = endColumn;
	}
	void GetMarking(bool &isMarked, int &startLine, int &startColumn, int &endLine, int &endColumn)
	{
		isMarked = m_marked;
		startLine = m_markStartLine;
		startColumn = m_markStartColumn;
		endLine = m_markEndLine;
		endColumn = m_markEndColumn;
	}
	void GetSortedMarking(bool& isMarked, int& startLine, int& startColumn, int& endLine, int& endColumn)
	{
		isMarked = m_marked;
		if ((m_markStartLine > m_markEndLine) || ((m_markStartLine == m_markEndLine) && (m_markStartColumn > m_markEndColumn)))
		{
			startLine = m_markEndLine;
			startColumn = m_markEndColumn;
			endLine = m_markStartLine;
			endColumn = m_markStartColumn;
		}
		else
		{
			startLine = m_markStartLine;
			startColumn = m_markStartColumn;
			endLine = m_markEndLine;
			endColumn = m_markEndColumn;
		}

	}

	// clear any visuals since font or font size has changed
	void ClearVisuals();
	void CalcRects();

	bool IsActiveAsmFile()
	{
		return m_activeSourceFileItem && HasExtension(m_activeSourceFileItem->file->GetPath().c_str(), ".asm");
	}

	bool IsAutoScrolling()
	{
		return m_autoScroll;
	}

protected:
	void OnMouseMotionCaptured(bool lostCapture, int x, int y);

	// calculate scroll bar top and bottom screenspace coords
	// returns true if scrollbar needs to render (ie. the start/end is not full screen)
	bool CalcVertScrollBar(int& start, int& end);
	bool CalcHorizScrollBar(int& start, int& end);
	void SelectCursor(int x, int y);
	void MakeActiveLineVisible();

	// check if line <lineNmbr> is marked
	// if it is, return the area of the line that is marked (from startCol to endCol)
	bool CheckLineMarked(int lineNmbr, int& startCol, int& endCol);

	// convert a mouse coord to a row/col in the edit source window
	bool MouseToRowCol(int x, int y, int& row, int& col);
	void ProcessMouseMarking(int x, int y);
	void SnapScrollBarToMouseY(int y);
	void SnapScrollBarToMouseX(int x);

	struct SourceFileItem
	{
		SourceFile* file;
		int vertScroll;
		int horizScroll;
		int activeLine, activeColumn, activeTargetX;
		bool modified;

		int editWindowHScrollWidth;    // locked scroll width = don't reduce unless the HorizScroll == 0
		int editWindowTextWidth;       // 

		SDL_Rect titleRect;
	};
	vector<SourceFileItem*> m_fileTabs;
	SourceFileItem* m_activeSourceFileItem;
	float m_cursorAnimTime;

	void UpdateContextualHelp();

	bool m_keyMarking;
	bool m_mouseMarking;
	bool m_marked;
	bool m_shiftDown;
	int m_markStartLine;
	int m_markStartColumn;
	int m_markEndLine;
	int m_markEndColumn;

	// auto scrolling
	int m_autoScroll;
	int m_autoScroll_mouseX;
	int m_autoScroll_mouseY;

	// track mouse move for wheel events
	int m_mouseX, m_mouseY;

	enum DragMode
	{
		DRAG_None,
		DRAG_DivideDecode,
		DRAG_DivideText,
		DRAG_DivideContext,
		DRAG_EditVertScroll,
		DRAG_EditHorizScroll,
		DRAG_LogVertScroll,
		DRAG_LogHorizScroll
	};
	DragMode m_dragMode;
	int m_dragOffset;

	void LayoutTabs();

	SDL_Rect m_allEditRect;
	SDL_Rect m_titleTabsRect;
	SDL_Rect m_sourceEditRect;
	SDL_Rect m_sourceEditVBarRect;
	SDL_Rect m_sourceEditHBarRect;

	SDL_Rect m_memAddrRect;
	SDL_Rect m_decodeRect;
	SDL_Rect m_contextHelpRect;
	SDL_Rect m_statusRect;

	// common resources
	SDL_Texture* m_tabActiveTex;
	SDL_Texture* m_tabInactiveTex;

	// status bar
	struct StatusInfo
	{
		float m_avgTimeDelta;
	} m_status;
	void InitStatus();
	void UpdateStatus();
	void DrawStatus();
	bool ScanTokenAt(int line, int col, int &startCol, int &endCol);

	int m_activeXPosDecode;
	int m_activeXPosText;
	int m_activeXPosContextHelp;

	void ClampActiveLine();

	// vscroll/hscroll
	int GetContentHeight();
	int GetContentWidth();
	void ClampTargetVertScroll();
	void ClampTargetHorizScroll();
	int m_vertScroll;
	int m_horizScroll;
	float m_targetVertScroll;
	float m_targetHorizScroll;
	SDL_Point m_dragMouseGrab;

	// scroll bar areas
	SDL_Rect m_contentArea;
	int m_renderedContentWidth;
	int m_renderedContentHeight;

	SDL_Rect m_vertBackArea;
	SDL_Rect m_vertBarFullArea;
	SDL_Rect m_vertBarArea;
	SDL_Rect m_horizBackArea;
	SDL_Rect m_horizBarFullArea;
	SDL_Rect m_horizBarArea;
	void CalcScrollBars();
};


