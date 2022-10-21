#include "common.h"
#include "cmdManager.h"

void CmdManager::Clear()
{
	while (!m_items.empty())
	{
		delete m_items.back();
		m_items.pop_back();
	}
	m_current = 0;
}

CmdManager::~CmdManager()
{
	Clear();
}

void CmdManager::PushCmd(CmdItem *item)
{
	// clip off the end of the list
	if ((u32)m_items.size() > m_current)
		m_file->SetForceDirty(true);

	while (m_items.size() > m_current)
	{
		delete m_items.back();
		m_items.pop_back();
	}

	m_items.push_back(item);
	m_current++;
}

void CmdManager::Undo()
{
	if (m_current > 0)
	{
		int stack = 0;
		do
		{
			--m_current;
			if (m_items[m_current]->IsGroupEnd())
				stack++;
			else if (m_items[m_current]->IsGroupStart())
				--stack;
			else
				m_items[m_current]->Undo();
		} while (m_current > 0 && stack);
	}
}

void CmdManager::Redo()
{
	if (m_current < m_items.size())
	{
		int stack = 0;
		do
		{
			if (m_items[m_current]->IsGroupStart())
				stack++;
			else if (m_items[m_current]->IsGroupEnd())
				stack--;
			else
				m_items[m_current]->Do();
			m_current++;
		} while (m_current < m_items.size() && stack);
	}
}

void CmdManager::Dump()
{
	printf("CMD BUFFER:\n");
	for (auto cmd : m_items)
	{
		cmd->Dump();
	}
}

void CmdChangeLines::Do()
{
	gApp->GetEditWindow()->ClearMarking();
	for (auto c : m_changes)
	{
		switch (c->type)
		{
		case Change::CHANGE_Add:
			{
				auto line = new SourceLine();
				line->GetChars() = c->newLine;
				m_file->GetLines().insert(m_file->GetLines().begin() + c->line, line);
				line->Tokenize();
				line->VisualizeText();
			}
			break;
		case Change::CHANGE_Remove:
			{
				auto line = m_file->GetLines()[c->line];
				delete line;
				m_file->GetLines().erase(m_file->GetLines().begin() + c->line);
			}
			break;
		case Change::CHANGE_Replace:
			{
				auto line = m_file->GetLines()[c->line];
				line->GetChars() = c->newLine;
				line->Tokenize();
				line->VisualizeText();
			}
			break;
		}
	}

	gApp->GetEditWindow()->SetActiveFile(m_file);
	gApp->GetEditWindow()->GotoLineCol(m_newActiveLine, m_newActiveColumn, MARK_None, true);

	if (m_changeMarking)
	{
		gApp->GetEditWindow()->SetMarking(m_markStartLine, m_markStartColumn, m_markEndLine, m_markEndColumn);
	}

	m_file->UpdateDirty(1);
}

void CmdChangeLines::Undo()
{
	gApp->GetEditWindow()->ClearMarking();
	for (auto it = m_changes.rbegin(); it != m_changes.rend(); ++it)
	{
		auto c = (*it);
		switch (c->type)
		{
		case Change::CHANGE_Add:
			{
				// undo - remove the line we added
				auto line = m_file->GetLines()[c->line];
				delete line;
				m_file->GetLines().erase(m_file->GetLines().begin() + c->line);
			}
			break;

		case Change::CHANGE_Remove:
			{
				// add - add back in the line we removed
				auto line = new SourceLine();
				line->GetChars() = c->oldLine;
				m_file->GetLines().insert(m_file->GetLines().begin() + c->line, line);
				line->Tokenize();
				line->VisualizeText();
			}
			break;

		case Change::CHANGE_Replace:
			{
				auto line = m_file->GetLines()[c->line];
				line->GetChars() = c->oldLine;
				line->Tokenize();
				line->VisualizeText();
			}
			break;
		}
	}

	gApp->GetEditWindow()->SetActiveFile(m_file);
	gApp->GetEditWindow()->GotoLineCol(m_oldActiveLine, m_oldActiveColumn, MARK_None, true);

	m_file->UpdateDirty(-1);
}

void CmdChangeLines::PushAdd(int line, string& chars)
{
	auto change = new Change();
	change->type = Change::CHANGE_Add;
	change->line = line;
	change->newLine = chars;
	m_changes.push_back(change);
}

void CmdChangeLines::PushRemove(int line)
{
	auto change = new Change();
	change->type = Change::CHANGE_Remove;
	change->line = line;
	change->oldLine = m_file->GetLines()[line]->GetChars();
	m_changes.push_back(change);
}

void CmdChangeLines::PushReplace(int line, string& chars)
{
	auto change = new Change();
	change->type = Change::CHANGE_Replace;
	change->line = line;
	change->oldLine = m_file->GetLines()[line]->GetChars();
	change->newLine = chars;
	m_changes.push_back(change);
}

void CmdChangeLines::Dump()
{
	const char* cmdName[] = { "REMOVE", "ADD", "REPLACE" };
	printf("CMD: ChangeLines\n");
	for (auto c : m_changes)
	{
		string oldLine((char*)c->oldLine.data(), c->oldLine.size());
		string newLine((char*)c->newLine.data(), c->newLine.size());
		printf("   %s : %d |%s| => |%s|\n", cmdName[c->type], c->line, oldLine.c_str(), newLine.c_str());
	}
}



