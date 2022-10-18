#include "common.h"
#include "application.h"

#include <windows.h>
#include <commdlg.h>

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

Application *gApp;

Application::Application()
{
    gApp = this;

    //Create window
    m_window = SDL_CreateWindow("MAD64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        m_settings = new AppSettings();
        if (!m_settings->Load())
            m_settings->Save();

        TTF_Init();
        m_font = TTF_OpenFont("data/font.ttf", m_settings->fontSize);
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetVSync(m_renderer, 1);

        //Get window surface
        SDL_Surface *screenSurface = SDL_GetWindowSurface(m_window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x60));
        SDL_UpdateWindowSurface(m_window);

        m_editWindow = new EditWindow();
        m_compiler = new Compiler();
        m_copyBuffer = new SourceCopyBuffer();

        for (auto& p : m_settings->loadedFilePaths)
        {
            LoadFile(p.c_str());
        }
    }
}

Application::~Application()
{
    //Destroy window
    SDL_DestroyWindow(m_window);

    //Quit SDL subsystems
    SDL_Quit();
}

int Application::MainLoop()
{
    SDL_Event e;
    while (!m_quit)
    {
        if (SDL_PollEvent(&e))
            HandleEvent(&e);

        Update();
        Draw();
    }

    for (auto file : m_sourceFiles)
        file->Save();

    return 0;
}

void Application::Update()
{
    m_editWindow->Update();
}

void Application::Draw()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
    SDL_RenderFillRect(m_renderer, NULL);
    m_editWindow->Draw();
    SDL_RenderPresent(m_renderer);
}

void Application::HandleEvent(SDL_Event *e)
{
    switch (e->type)
    {
    case SDL_QUIT:
        m_quit = true;
        break;
    case SDL_MOUSEBUTTONDOWN:
        m_editWindow->OnMouseDown(e);
        break;
    case SDL_MOUSEBUTTONUP:
        m_editWindow->OnMouseUp(e);
        break;
    case SDL_MOUSEMOTION:
        m_editWindow->OnMouseMotion(e);
        break;
    case SDL_MOUSEWHEEL:
        m_editWindow->OnMouseWheel(e);
        break;
    case SDL_KEYDOWN:
        OnKeyDown(e);
        break;
    case SDL_KEYUP:
        break;
    case SDL_WINDOWEVENT:
        m_editWindow->OnResize();
        break;
    }
}

void Application::LoadFile()
{
    OPENFILENAMEA ofn;
    char buffer[256];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = buffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(buffer);
    ofn.lpstrFilter = "All\0*.*\0Asm\0*.asm\0Basic\0*.bas";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn))
    {
        LoadFile(ofn.lpstrFile);
    }
}

void Application::LoadFile(const char* path)
{
    // check file isn't already loaded
    for (auto f : m_sourceFiles)
    {
        if (SDL_strcasecmp(path, f->GetPath())==0)
        {
            return;
        }
    }

    auto source = new SourceFile(path);
    if (source->Load())
    {
        m_sourceFiles.push_back(source);

        // alert renderer of update
        m_editWindow->OnFileLoaded(source);

        bool exists = false;
        for (auto& p : m_settings->loadedFilePaths)
        {
            if (SDL_strcasecmp(p.c_str(), path) == 0)
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            m_settings->loadedFilePaths.push_back(string(path));
            m_settings->Save();
        }
    }
    else
    {
        delete source;
    }
}

void Application::SaveFile()
{
    auto activeFile = m_editWindow->GetActiveFile();
    if (activeFile)
    {
        activeFile->Save();
    }
}

void Application::CloseFile()
{
    auto activeFile = m_editWindow->GetActiveFile();
    if (activeFile)
    {
        m_editWindow->OnFileClosed(activeFile);
        m_sourceFiles.erase(remove(m_sourceFiles.begin(), m_sourceFiles.end(), activeFile));
        m_settings->loadedFilePaths.erase(remove(m_settings->loadedFilePaths.begin(), m_settings->loadedFilePaths.end(), string(activeFile->GetPath())));
        delete activeFile;
    }
}

void Application::CreateNewFile()
{
    OPENFILENAMEA ofn;
    char buffer[256];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = buffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(buffer);
    ofn.lpstrFilter = "All\0*.*\0Asm\0*.asm\0Basic\0*.bas";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = 0;
    if (GetSaveFileNameA(&ofn))
    {
        auto source = new SourceFile(ofn.lpstrFile);
        m_sourceFiles.push_back(source);

        m_editWindow->OnFileLoaded(source);
    }
}

void Application::OnKeyDown(SDL_Event* e)
{
    switch (e->key.keysym.sym)
    {
    case SDLK_l:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            if (!e->key.repeat)
                LoadFile();
            return;
        }
        break;
    case SDLK_n:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            if (!e->key.repeat)
                CreateNewFile();
            return;
        }
        break;
    case SDLK_s:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            if (!e->key.repeat)
                SaveFile();
            return;
        }
        break;
    case SDLK_q:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            if (!e->key.repeat)
                CloseFile();
            return;
        }
        break;
    }

    m_editWindow->OnKeyDown(e);
}

void Application::Cmd_InsertChar(char ch)
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();
        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

        auto line = file->GetLines()[oldActiveLine];
        auto& chars = line->GetChars();

        auto copy = chars;

        if (m_settings->overwriteMode && oldActiveCol < copy.size())
            copy[oldActiveCol] = ch;
        else
            copy.insert(copy.begin() + oldActiveCol, ch);

        cmd->SetNewActiveLineCol(oldActiveLine, oldActiveCol + 1);

        cmd->PushReplace(oldActiveLine, copy);
        file->GetCmdManager()->PushCmd(cmd);
        cmd->Do();
    }
}

void Application::Cmd_InsertSpaces(int count)
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();
        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

        auto line = file->GetLines()[oldActiveLine];
        auto& chars = line->GetChars();

        auto copy = chars;
        if (m_settings->overwriteMode)
        {
            for (int i = 0; i < count; i++)
            {
                if ((oldActiveCol + i) < copy.size())
                    copy[oldActiveCol + i] = ' ';
                else
                    copy.push_back(' ');
            }
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                copy.insert(copy.begin() + oldActiveCol, ' ');
            }
        }
        cmd->SetNewActiveLineCol(oldActiveLine, oldActiveCol + count);

        cmd->PushReplace(oldActiveLine, copy);
        file->GetCmdManager()->PushCmd(cmd);
        cmd->Do();
    }
}
void Application::Cmd_BackspaceChar()
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();

        // do nothing if at top of file
        if (oldActiveLine == 0 && oldActiveCol == 0)
            return;

        if (oldActiveCol > 0)
        {
            // just remove previous character
            auto line = file->GetLines()[oldActiveLine];
            auto& chars = line->GetChars();
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

            auto copy = chars;
            copy.erase(copy.begin() + (oldActiveCol - 1));

            int newActiveLine = oldActiveLine;
            int newActiveColumn = oldActiveCol - 1;
            cmd->SetNewActiveLineCol(newActiveLine, newActiveColumn);

            cmd->PushReplace(oldActiveLine, copy);
            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
        else
        {
            // need to remove CR, so concat this line onto the prev and delete this line
            auto line = file->GetLines()[oldActiveLine];
            auto prevline = file->GetLines()[oldActiveLine - 1];
            auto& chars = line->GetChars();
            auto& prevChars = prevline->GetChars();
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

            auto copy = prevChars;
            copy.insert(copy.end(), chars.begin(), chars.end());
            cmd->PushReplace(oldActiveLine - 1, copy);
            cmd->PushRemove(oldActiveLine);

            int newActiveLine = oldActiveLine - 1;
            int newActiveColumn = (int)prevChars.size();
            cmd->SetNewActiveLineCol(newActiveLine, newActiveColumn);
            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
    }
}

void Application::Cmd_DeleteChar()
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();

        // do nothing if at bottom of file
        if (oldActiveLine == (file->GetLines().size()-1) && (oldActiveCol == file->GetLines().back()->GetChars().size()))
            return;

        auto line = file->GetLines()[oldActiveLine];
        if (oldActiveCol < line->GetChars().size())
        {
            // just remove the next character
            auto& chars = line->GetChars();
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

            auto copy = chars;
            copy.erase(copy.begin() + oldActiveCol);

            cmd->PushReplace(oldActiveLine, copy);
            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
        else
        {
            // need to remove CR, so concat next line onto this one
            auto line = file->GetLines()[oldActiveLine];
            auto nextline = file->GetLines()[oldActiveLine + 1];
            auto& chars = line->GetChars();
            auto& nextChars = nextline->GetChars();
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

            auto copy = chars;
            copy.insert(copy.end(), nextChars.begin(), nextChars.end());
            cmd->PushReplace(oldActiveLine, copy);
            cmd->PushRemove(oldActiveLine+1);

            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
    }
}
void Application::Cmd_DeleteArea(SourceFile* file, int startLine, int startColumn, int endLine, int endColumn, bool toCopyBuffer)
{
    if (startLine == endLine && startColumn == endColumn)
        return;

    int sl = startLine;
    int sc = startColumn;
    int el = endLine;
    int ec = endColumn;
    if (startLine > endLine || (startLine == endLine && startColumn > endColumn))
    {
        sl = endLine;
        sc = endColumn;
        el = startLine;
        ec = startColumn;
    }

    // copy deleted section to the copy buffer
    if (toCopyBuffer)
    {
        m_copyBuffer->Clear();
        if (sl == el)
        {
            auto line = file->GetLines()[sl];
            auto cbLine = new SourceCopyBufferLine();
            cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin() + sc, line->GetChars().begin() + ec);
            m_copyBuffer->GetLines().push_back(cbLine);
        }
        else
        {
            auto line = file->GetLines()[sl];
            auto cbLine = new SourceCopyBufferLine();
            cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin() + sc, line->GetChars().end());
            m_copyBuffer->GetLines().push_back(cbLine);

            for (int i = sl + 1; i < el; i++)
            {
                line = file->GetLines()[i];
                cbLine = new SourceCopyBufferLine();
                cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin(), line->GetChars().end());
                m_copyBuffer->GetLines().push_back(cbLine);
            }

            line = file->GetLines()[el];
            cbLine = new SourceCopyBufferLine();
            cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin(), line->GetChars().begin() + ec);
            m_copyBuffer->GetLines().push_back(cbLine);
        }
    }

    int oldActiveLine = m_editWindow->GetActiveLine();
    int oldActiveCol = m_editWindow->GetActiveCol();
    auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

    if (sl == el)
    {
        auto line = file->GetLines()[sl];
        auto& chars = line->GetChars();

        auto copy = chars;
        copy.erase(copy.begin() + sc, copy.begin() + ec);
        cmd->PushReplace(sl, copy);
        cmd->SetNewActiveLineCol(sl, sc);
    }
    else
    {
        // need to apply bottom to top so that cmds record the correct information
        // any Remove puts everything below it out of sync
        // join end line to start line
        auto firstLine = file->GetLines()[sl];
        auto lastLine = file->GetLines()[el];
        auto& firstChars = firstLine->GetChars();
        auto& lastChars = lastLine->GetChars();
        auto copy = firstChars;
        copy.erase(copy.begin() + sc, copy.end());
        copy.insert(copy.end(), lastChars.begin() + ec, lastChars.end());

        // remove lines button up
        for (int ln = el; ln > sl; ln--)
        {
            cmd->PushRemove(ln);
        }

        cmd->PushReplace(sl, copy);
        cmd->SetNewActiveLineCol(sl, sc);
    }
    file->GetCmdManager()->PushCmd(cmd);
    cmd->Do();

    m_editWindow->ClearMarking();
}

void Application::Cmd_CopyArea(SourceFile* file, int startLine, int startColumn, int endLine, int endColumn)
{
    int sl = startLine;
    int sc = startColumn;
    int el = endLine;
    int ec = endColumn;
    if (startLine > endLine || (startLine == endLine && startColumn > endColumn))
    {
        sl = endLine;
        sc = endColumn;
        el = startLine;
        ec = startColumn;
    }

    m_copyBuffer->Clear();
    if (sl == el)
    {
        auto line = file->GetLines()[sl];
        auto cbLine = new SourceCopyBufferLine();
        cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin() + sc, line->GetChars().begin() + ec);
        m_copyBuffer->GetLines().push_back(cbLine);
    }
    else
    {
        auto line = file->GetLines()[sl];
        auto cbLine = new SourceCopyBufferLine();
        cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin() + sc, line->GetChars().end());
        m_copyBuffer->GetLines().push_back(cbLine);

        for (int i = sl + 1; i < el; i++)
        {
            line = file->GetLines()[i];
            cbLine = new SourceCopyBufferLine();
            cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin(), line->GetChars().end());
            m_copyBuffer->GetLines().push_back(cbLine);
        }

        line = file->GetLines()[el];
        cbLine = new SourceCopyBufferLine();
        cbLine->GetChars().insert(cbLine->GetChars().begin(), line->GetChars().begin(), line->GetChars().begin() + ec);
        m_copyBuffer->GetLines().push_back(cbLine);
    }
}

void Application::Cmd_PasteArea(SourceFile* file)
{
    if (!m_copyBuffer->GetLines().empty())
    {
        SourceFile* file = m_editWindow->GetActiveFile();
        if (file)
        {
            int oldActiveLine = m_editWindow->GetActiveLine();
            int oldActiveCol = m_editWindow->GetActiveCol();

            auto line = file->GetLines()[oldActiveLine];
            auto& chars = line->GetChars();

            auto copyStart = chars;
            copyStart.erase(copyStart.begin() + oldActiveCol, copyStart.end());
            auto copyEnd = chars;
            copyEnd.erase(copyEnd.begin(), copyEnd.begin() + oldActiveCol);

            int startMarkingCol = (int)copyStart.size();
            int startMarkingLine = oldActiveLine;
            int endMarkingLine = oldActiveLine + (int)m_copyBuffer->GetLines().size() - 1;
            int endMarkingCol = (int)m_copyBuffer->GetLines().back()->GetChars().size();

            vector<char> out;
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);
            for (int i = 0; i < m_copyBuffer->GetLines().size(); i++)
            {
                auto cbLine = m_copyBuffer->GetLines()[i];

                out.clear();
                if (i == 0)
                    out = copyStart;
                out.insert(out.end(), cbLine->GetChars().begin(), cbLine->GetChars().end());

                if (i == m_copyBuffer->GetLines().size() - 1)
                {
                    endMarkingCol = (int)out.size();
                    out.insert(out.end(), copyEnd.begin(), copyEnd.end());
                }

                if (i == 0)
                    cmd->PushReplace(oldActiveLine, out);
                else
                    cmd->PushAdd(oldActiveLine+i, out);
            }

            cmd->SetNewActiveLineCol(endMarkingLine, endMarkingCol);
            cmd->SetPostMarking(startMarkingLine, startMarkingCol, endMarkingLine, endMarkingCol);

            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
    }
}

void Application::Cmd_OverwriteChar(SourceFile* file, int ln, int col, char ch)
{

}

void Application::Cmd_InsertNewLine()
{
    auto settings = gApp->GetSettings();
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();

        auto line = file->GetLines()[oldActiveLine];
        auto& chars = line->GetChars();

        auto copy = chars;
        copy.erase(copy.begin() + oldActiveCol, copy.end());

        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);
        cmd->PushReplace(oldActiveLine, copy);

        copy = chars;
        copy.erase(copy.begin(), copy.begin() + oldActiveCol);

        int tab = 0;
        if (settings->autoIndent)
        {
            // count indent in spaces...
            int countSpaces = 0;
            for (int i = 0; i < chars.size() && (chars[i] == ' ' || chars[i] == '\t'); i++)
            {
                if (chars[i] == ' ')
                    countSpaces++;
                else if (chars[i] == '\t')
                    countSpaces += (settings->tabWidth - (countSpaces % settings->tabWidth));
            }

            if (countSpaces > 0)
            {
                // now insert either spaces or tabs
                if (settings->tabsToSpaces)
                {
                    for (int i = 0; i < countSpaces; i++)
                        copy.insert(copy.begin(), ' ');
                    tab = countSpaces;
                }
                else
                {
                    int insertTabs = max(1, countSpaces / settings->tabWidth);
                    for (int i=0; i < insertTabs; i++)
                        copy.insert(copy.begin(), '\t');
                    tab = insertTabs;
                }
            }
        }

        cmd->PushAdd(oldActiveLine + 1, copy);
        cmd->SetNewActiveLineCol(oldActiveLine + 1, tab);

        file->GetCmdManager()->PushCmd(cmd);
        cmd->Do();

        m_editWindow->ClearMarking();
    }
}
