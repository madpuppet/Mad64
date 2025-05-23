#include "common.h"
#include "dockableWindow_searchAndReplace.h"
#include "uiItem_textBox.h"

void DockableWindow_SearchAndReplace::OnChildRendererChange()
{
}

void DockableWindow_SearchAndReplace::ClearAll()
{
    m_replaceBox->SetText("");
    m_searchBox->SetText("");
}

void DockableWindow_SearchAndReplace::Clear()
{
    m_items.clear();
    SetContentDirty();
}

void DockableWindow_SearchAndReplace::DrawChild()
{
    m_renderedWidth = 128;
    m_renderedItems.clear();

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();
    auto fr = gApp->GetFontRenderer();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    int activeLine = gApp->GetEditWindow()->GetActiveLine();
    int whiteSpace = gApp->GetWhiteSpaceWidth();

    for (auto& it : m_items)
    {
        if (it.lineNmbr == activeLine)
        {
            SDL_SetRenderDrawColor(r, 64, 64, 0, 255);
            SDL_Rect backRect = { m_contentArea.x, y, m_contentArea.w, settings->lineHeight };
            SDL_RenderFillRect(r, &backRect);
        }
        SDL_Color col = { 255, 255, 255, 255 };
        SDL_Rect rect;
        fr->RenderText(r, it.text, col, x, y, CachedFontRenderer::StandardFont, &rect, false);
        int dataX = rect.x + rect.w + whiteSpace;
        if (it.addr != -1)
        {
            auto emu = gApp->GetEmulator();
            for (int i = 0; i < 4; i++)
            {
                auto str = FormatString("%02x", emu->GetByte(it.addr + i));
                fr->RenderText(r, str, col, dataX, y, CachedFontRenderer::StandardFont, &rect, false);
                dataX += whiteSpace * 3;
            }
        }
        if (it.lineNmbr != -1)
        {
            RenderedItem item;
            item.area = { x, y, dataX - x, rect.h };
            item.lineNmbr = it.lineNmbr;
            m_renderedItems.push_back(item);
        }
        m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + (dataX - rect.x));
        y += settings->lineHeight;
    }
}

void DockableWindow_SearchAndReplace::CreateChildIcons()
{
    m_searchBox = new UIItem_TextBox("S", "<search>", 200, DELEGATE(DockableWindow_SearchAndReplace::OnSearchStringEnter), DELEGATE(DockableWindow_SearchAndReplace::OnSearchStringChange));
    m_searchBox->SetTabCallbacks(DELEGATE(DockableWindow_SearchAndReplace::ActivateReplace), DELEGATE(DockableWindow_SearchAndReplace::ActivateReplace));
    m_replaceBox = new UIItem_TextBox("R", "<replace>", 200, DELEGATE(DockableWindow_SearchAndReplace::OnReplaceStringEnter), DELEGATE(DockableWindow_SearchAndReplace::OnReplaceStringChange));
    m_replaceBox->SetTabCallbacks(DELEGATE(DockableWindow_SearchAndReplace::ActivateSearch), DELEGATE(DockableWindow_SearchAndReplace::ActivateSearch));
    m_titleIconsLeft.push_back(m_searchBox);
    m_titleIconsLeft.push_back(m_replaceBox);
}

void DockableWindow_SearchAndReplace::ActivateSearch()
{
    m_searchBox->SetSelected(true);
}
void DockableWindow_SearchAndReplace::ActivateReplace()
{
    m_replaceBox->SetSelected(true);
}


void DockableWindow_SearchAndReplace::LogText(const string& text, int lineNmbr, int color, int addr)
{
    auto settings = gApp->GetSettings();
    int y = 0;
    SDL_Color col = { 255,255,255,255 };
    LineItem item;
    item.text = text;
    item.y = y;
    item.lineNmbr = lineNmbr;
    item.colorIdx = color;
    item.addr = addr;
    y += settings->lineHeight;
    m_items.push_back(item);
    SetContentDirty();
}

int DockableWindow_SearchAndReplace::GetContentHeight()
{
    auto settings = gApp->GetSettings();
    return settings->lineHeight * (int)m_items.size();
}

int DockableWindow_SearchAndReplace::GetContentWidth()
{
    return m_renderedWidth;
}

void DockableWindow_SearchAndReplace::OnMouseButtonDown(int button, int x, int y)
{
    if (Contains(m_contentArea, x, y))
    {
        for (auto& item : m_renderedItems)
        {
            if (Contains(item.area, x, y))
            {
                gApp->GetEditWindow()->GotoLineCol(item.lineNmbr, 0, MARK_None, true);
                return;
            }
        }
    }
    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_SearchAndReplace::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    for (auto& item : m_renderedItems)
    {
        if (Contains(item.area, xAbs, yAbs))
        {
            gApp->SetCursor(Cursor_Hand);
        }
    }
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}

void DockableWindow_SearchAndReplace::OnSearchStringEnter(const string& text)
{
    OnSearchStringChange(text);

    auto winEdit = gApp->GetEditWindow();
    auto file = winEdit->GetActiveFile();
    if (file)
    {
        // find all occurances...
        m_searchFoundLines.clear();
        auto& lines = file->GetLines();
        for (int ln = 0; ln < lines.size(); ln++)
        {
            if (StrFind(lines[ln]->GetChars(), text) != string::npos)
            {
                m_searchFoundLines.push_back(ln);
            }
        }

        int activeLine = winEdit->GetActiveLine();
        int startLine = (activeLine + 1) % lines.size();
        int ln = startLine;
        do
        {
            auto line = lines[ln];
            auto& chars = line->GetChars();
            int col = (int)StrFind(line->GetChars(), text);
            if (col != string::npos)
            {
//                m_searchBox->Flash(TextInput::MODE_Found);
                winEdit->GotoLineCol(ln, col, MARK_None, true);
                break;
            }
            ln = (ln + 1) % file->GetLines().size();
        } while (ln != activeLine);
//        m_searchBox->Flash(TextInput::MODE_NotFound);

        LogFoundItems();
    }
}

void DockableWindow_SearchAndReplace::LogFoundItems()
{
    Clear();
    auto winEdit = gApp->GetEditWindow();
    auto file = winEdit->GetActiveFile();
    if (file)
    {
        auto& lines = file->GetLines();
        for (auto ln : m_searchFoundLines)
        {
            if (lines.size() > ln)
            {
                LogText(FormatString("%05d %s", ln, lines[ln]->GetChars().c_str()), ln, ln % 1, -1);
            }
        }
    }
}

bool DockableWindow_SearchAndReplace::ContainsLine(int lineIdx)
{
    return std::find(m_searchFoundLines.begin(), m_searchFoundLines.end(), lineIdx) != m_searchFoundLines.end();
}

void DockableWindow_SearchAndReplace::OnSearchStringChange(const string& text)
{
    m_searchFoundLines.clear();
    Clear();

    auto winEdit = gApp->GetEditWindow();
    auto file = winEdit->GetActiveFile();
    if (file)
    {
        // find all occurances...
        m_searchFoundLines.clear();
        auto& lines = file->GetLines();
        for (int ln = 0; ln < lines.size(); ln++)
        {
            if (StrFind(lines[ln]->GetChars(), text) != string::npos)
            {
                m_searchFoundLines.push_back(ln);
            }
        }
        LogFoundItems();
    }
}

void DockableWindow_SearchAndReplace::ReSearch(const string& text, int minSize)
{
    m_searchFoundLines.clear();
    Clear();

    auto winEdit = gApp->GetEditWindow();
    auto file = winEdit->GetActiveFile();
    if (file)
    {
        // find all occurances...
        m_searchFoundLines.clear();
        auto& lines = file->GetLines();
        for (int ln = 0; ln < lines.size(); ln++)
        {
            if (StrFind(lines[ln]->GetChars(), text) != string::npos)
            {
                m_searchFoundLines.push_back(ln);
            }
        }
        LogFoundItems();
    }
}

void DockableWindow_SearchAndReplace::OnReplaceStringEnter(const string& text)
{
    auto winEdit = gApp->GetEditWindow();
    auto file = winEdit->GetActiveFile();
    if (file)
    {
        string searchText = m_searchBox->GetText();
        string replaceText = m_replaceBox->GetText();

        if (!searchText.empty())
        {
            bool isMarked;
            int startLine, startColumn, endLine, endColumn;
            winEdit->GetSortedMarking(isMarked, startLine, startColumn, endLine, endColumn);

            if (isMarked)
                gApp->Cmd_SearchAndReplace(searchText, replaceText, startLine, startColumn, endLine, endColumn);
            else
                gApp->Cmd_SearchAndReplace(searchText, replaceText);
        }
    }
}

void DockableWindow_SearchAndReplace::OnReplaceStringChange(const string& text)
{
}

void DockableWindow_SearchAndReplace::SetSearchText(const string& text)
{
    m_searchBox->SetText(text);
}

void DockableWindow_SearchAndReplace::SetReplaceText(const string& text)
{
    m_replaceBox->SetText(text);
}

void DockableWindow_SearchAndReplace::SelectSearchBox()
{
    m_searchBox->SetSelected(true);
}
void DockableWindow_SearchAndReplace::SelectReplaceBox()
{
    m_replaceBox->SetSelected(true);
}


