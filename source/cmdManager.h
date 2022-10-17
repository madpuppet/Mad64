#pragma once

class CmdItem
{
public:
	virtual ~CmdItem() {}
	virtual void Do() = 0;
	virtual void Undo() = 0;
	virtual bool IsGroupStart() { return false; }
	virtual bool IsGroupEnd() { return false; }
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
	}

	~CmdChangeLines()
	{
		for (auto c : m_changes)
			delete c;
	}

	void Do();
	void Undo();

	void SetFile(class SourceFile* file)
	{
		m_file = file;
	}
	void SetNewActiveLineCol(int line, int column)
	{
		m_newActiveLine = line;
		m_newActiveColumn = column;
	}
	void PushAdd(int line, vector<char>& chars);
	void PushRemove(int line);
	void PushReplace(int line, vector<char>& chars);

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

		vector<char> oldLine;
		vector<char> newLine;
	};
	vector<Change*> m_changes;
	
	class SourceFile* m_file;
	int m_oldActiveLine;
	int m_oldActiveColumn;
	int m_newActiveLine;
	int m_newActiveColumn;
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

	class SourceFile* GetFile() { return m_file; }

protected:
	class SourceFile* m_file;
	vector<CmdItem*> m_items;
	u32 m_current;
};
