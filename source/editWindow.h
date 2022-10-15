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

protected:
	void ClampTargetScroll();
	void CalcScrollBar(int& start, int& end);
	void CursorUp(bool marking);
	void CursorLeft(bool marking);
	void CursorRight(bool marking);
	void CursorDown(bool marking);
	void CursorStart(bool marking);
	void CursorEnd(bool marking);
	void MakeActiveLineVisible();
	bool CheckLineMarked(int lineNmbr, int& startCol, int& endCol);

	struct SourceFileItem
	{
		SourceFile* file;
		GraphicElement* geText;
		int scroll;
		int targetScroll;
		int activeLine, activeColumn, activeTargetX;
	};
	vector<SourceFileItem*> m_fileTabs;
	SourceFileItem* m_activeSourceFileItem;
	float m_cursorAnimTime;
	bool m_overwriteMode;

	bool m_marking;
	int m_markStartLine;
	int m_markStartColumn;
	int m_markEndLine;
	int m_markEndColumn;

	enum DragMode
	{
		DRAG_None,
		DRAG_DivideDecode,
		DRAG_DivideText,
		DRAG_DivideContext
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

	// common resources
	SDL_Texture* m_tabActiveTex;
	SDL_Texture* m_tabInactiveTex;
};