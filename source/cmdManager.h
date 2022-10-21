#pragma once

class CmdItem
{
public:
	virtual ~CmdItem() {}
	virtual void Do() = 0;
	virtual void Undo() = 0;
	virtual bool IsGroupStart() { return false; }
	virtual bool IsGroupEnd() { return false; }

	virtual void Dump() {}
};

class CmdItemGroupStart : public CmdItem
{
public:
	virtual void Do() {}
	virtual void Undo() {}
	virtual bool IsGroupStart() { return true; }
	virtual bool IsGroupEnd() { return false; }
};

class CmdItemGroupEnd : public CmdItem
{
public:
	virtual void Do() {}
	virtual void Undo() {}
	virtual bool IsGroupStart() { return false; }
	virtual bool IsGroupEnd() { return true; }
};

class CmdChangeLines : public CmdItem
{
public:
	CmdChangeLines(SourceFile* file, int oldLine, int oldColumn)
	{
		m_file = file;
		m_oldActiveLine = oldLine;
		m_oldActiveColumn = oldColumn;
		m_newActiveLine = oldLine;
		m_newActiveColumn = oldColumn;
		m_changeMarking = false;
	}

	~CmdChangeLines()
	{
		for (auto c : m_changes)
			delete c;
	}

	void Do();
	void Undo();
	void Dump();

	void SetFile(class SourceFile* file)
	{
		m_file = file;
	}
	void SetNewActiveLineCol(int line, int column)
	{
		m_newActiveLine = line;
		m_newActiveColumn = column;
	}
	void PushAdd(int line, string& chars);
	void PushRemove(int line);
	void PushReplace(int line, string& chars);
	int Size() { return (int)m_changes.size(); }

	void SetPostMarking(int startLine, int startColumn, int endLine, int endColumn)
	{
		m_changeMarking = true;
		m_markStartLine = startLine;
		m_markStartColumn = startColumn;
		m_markEndLine = endLine;
		m_markEndColumn = endColumn;
	}

protected:
	struct Change
	{
		enum Type
		{
			CHANGE_Remove,
			CHANGE_Add,
			CHANGE_Replace
		} type;
		int line;

		string oldLine;
		string newLine;
	};
	vector<Change*> m_changes;
	
	class SourceFile* m_file;
	int m_oldActiveLine;
	int m_oldActiveColumn;
	int m_newActiveLine;
	int m_newActiveColumn;

	bool m_changeMarking;
	int m_markStartLine;
	int m_markStartColumn;
	int m_markEndLine;
	int m_markEndColumn;
};

class CmdManager
{
public:
	CmdManager(class SourceFile *file) : m_current(0), m_file(file) {}
	~CmdManager();

	void Clear();
	void PushCmd(CmdItem *item);
	void Undo();
	void Redo();

	void Dump();

	int GetTotalCmds() { return (int)m_items.size(); }
	int GetCurrentCmdIndex() { return m_current; }

	class SourceFile* GetFile() { return m_file; }

protected:
	class SourceFile* m_file;
	vector<CmdItem*> m_items;
	u32 m_current;
};
