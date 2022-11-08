#include "common.h"
#include "application.h"
#include "tinyfiledialogs.h"

// todo - cross platform way to launch the emulator
#if defined(_WIN32)
#include <Windows.h>
#endif

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

Application *gApp;

#define APP_TITLE "MAD64"
#define VERSION "2022.07"

Application::Application()
{
    m_quit = false;
    m_latchDoubleClick = 0;
    m_clickX = 0;
    m_clickY = 0;
    m_clickTime = 0;

    LogStart();
    gApp = this;
    m_fullscreen = false;

    //Create window
    m_window = SDL_CreateWindow(APP_TITLE " v" VERSION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_window == NULL)
    {
        Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        m_quit = true;
        exit(0);
    }
    else
    {
        m_settings = new AppSettings();
        if (!m_settings->Load())
            m_settings->Save();

        // create cursors
        m_cursors[Cursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        m_cursors[Cursor_IBeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        m_cursors[Cursor_Horiz] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        m_cursors[Cursor_Vert] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        m_cursors[Cursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

        TTF_Init();
        Log("Create Font: %s", m_settings->fontPath.c_str());
        m_font = TTF_OpenFont(m_settings->fontPath.c_str(), m_settings->fontSize);
        if (m_font == nullptr)
        {
            m_font = TTF_OpenFont("font.otf", 16);
            m_settings->fontSize = 16;
        }

        TTF_GlyphMetrics(m_font, ' ', nullptr, nullptr, nullptr, nullptr, &m_whiteSpaceWidth);
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

        //Get window surface
        SDL_Surface *screenSurface = SDL_GetWindowSurface(m_window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x60));
        SDL_UpdateWindowSurface(m_window);

        m_logWindow = new LogWindow();
        m_editWindow = new EditWindow();
        m_compiler = new Compiler();
        m_emulator = new EmulatorC64();
//        m_emulator->ConvertSnapshot();

        for (auto& p : m_settings->loadedFilePaths)
        {
            LoadFile(p.c_str());
        }

        m_mouseCapture = Capture_None;
        m_keyCapture = Capture_None;
    }

    SDL_StartTextInput();
}

void Application::ReloadFont()
{
    TTF_Font* newFont = TTF_OpenFont(m_settings->fontPath.c_str(), m_settings->fontSize);
    if (newFont)
    {
        TTF_CloseFont(m_font);
        m_font = newFont;
        TTF_GlyphMetrics(m_font, ' ', nullptr, nullptr, nullptr, nullptr, &m_whiteSpaceWidth);
    }

    m_editWindow->ClearVisuals();

    for (auto f : m_sourceFiles)
    {
        f->ClearAllVisuals();
        if (f->GetCompileInfo())
            f->GetCompileInfo()->ClearVisuals();
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
    m_frameTick = 0;

    SDL_Event e;
    while (!m_quit)
    {
        if (SDL_WaitEventTimeout(&e,10))
            HandleEvent(&e);

        Update();
        Draw();

        u64 now = SDL_GetPerformanceCounter();
        if (m_frameTick == 0)
        {
            m_timeDelta = 1 / 60.0f;
        }
        else
        {
            m_timeDelta = (float)(now - m_frameTick) / (float)SDL_GetPerformanceFrequency();
        }
        m_frameTick = now;
    }

    for (auto file : m_sourceFiles)
        file->Save();

    m_settings->Save();

    delete m_editWindow;
    delete m_compiler;
    delete m_emulator;
    delete m_logWindow;
    delete m_settings;
    return 0;
}

void Application::Update()
{
    m_editWindow->Update();
    m_logWindow->Update();
    m_compiler->Update();
    m_emulator->Update();
}

void Application::Draw()
{
    SDL_SetRenderDrawColor(m_renderer, m_settings->backColor.r, m_settings->backColor.g, m_settings->backColor.b, 255);
    SDL_RenderFillRect(m_renderer, NULL);
    m_editWindow->Draw();
    SDL_RenderPresent(m_renderer);
}

void Application::HandleEvent(SDL_Event *e)
{
    switch (e->type)
    {
    case SDL_QUIT:
        Log("%s(%d): Got Quit Message!", __FILE__, __LINE__);
        m_quit = true;
        break;
    case SDL_MOUSEBUTTONDOWN:
        {
            u64 time = SDL_GetTicks();

            // we got a double click, so ignore further clicks until a delay
            if (m_latchDoubleClick && ((time - m_clickTime) < 400))
                return;

            if (!IsNear(e->button.x, m_clickX, 2) || !IsNear(e->button.y, m_clickY, 2) || ((time - m_clickTime) > 300))
            {
                m_clickX = e->button.x;
                m_clickY = e->button.y;
                m_clickTime = time;
                e->button.clicks = 1;
                m_latchDoubleClick = false;
            }
            else
            {
                e->button.clicks = 2;
                m_latchDoubleClick = true;
            }

            m_editWindow->OnMouseDown(e);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        m_editWindow->OnMouseUp(e);
        break;
    case SDL_MOUSEMOTION:
        if (!m_latchDoubleClick)
            m_editWindow->OnMouseMotion(e);
        break;
    case SDL_MOUSEWHEEL:
        m_editWindow->OnMouseWheel(e);
        break;
    case SDL_TEXTINPUT:
        m_editWindow->OnTextInput(e);
        break;
    case SDL_KEYDOWN:
        OnKeyDown(e);
        break;
    case SDL_KEYUP:
        OnKeyUp(e);
        break;
    case SDL_WINDOWEVENT:
        switch (e->window.event)
        {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                m_editWindow->OnResize();
                break;
        }
        break;
    }
}

void Application::LoadFile()
{
    const char* path = gApp->GetSettings()->activeFilePath.c_str();
    const char* patterns[3] = { "*.asm", "*.bas", "*.txt"};
    const char *file = tinyfd_openFileDialog("Load file", path, 3, patterns, nullptr, false);
    if (file)
    {
        Log("Load File: %s", file);
        LoadFile(file);
    }
}

SourceFile *Application::FindFile(const char* path)
{
    // check file isn't already loaded
    for (auto f : m_sourceFiles)
    {
        if (StrEqual(f->GetPath(), path))
        {
            return f;
        }
    }
    return nullptr;
}

void Application::LoadFile(const char* path)
{
    auto file = FindFile(path);
    if (file)
    {
        m_editWindow->SetActiveFile(file);
        return;
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

        if (HasExtension(path, ".asm"))
        {
            m_compiler->Compile(source);
            m_compiler->LogContextualHelp(source, m_editWindow->GetActiveLine());
        }
        else
        {
            m_logWindow->ClearAllLogs();
        }
        m_editWindow->CalcRects();
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
        m_sourceFiles.erase(std::remove(m_sourceFiles.begin(), m_sourceFiles.end(), activeFile));
        m_settings->loadedFilePaths.erase(std::remove(m_settings->loadedFilePaths.begin(), m_settings->loadedFilePaths.end(), string(activeFile->GetPath())));
        delete activeFile;
    }
}

void Application::CreateNewFile()
{
    Log("Create File");

    const char* path = gApp->GetSettings()->activeFilePath.c_str();
    const char* patterns[3] = { "*.asm", "*.bas", "*.*"};
    const char* file = tinyfd_saveFileDialog("Create new file", path, 3, patterns, "");
    if (file)
    {
        Log("New File: %s", file);

        string name = file;
        auto source = new SourceFile(name.c_str());
        m_sourceFiles.push_back(source);
        auto newLine = new SourceLine();
        newLine->Tokenize();
        source->GetLines().push_back(newLine);
        m_editWindow->OnFileLoaded(source);

        m_settings->loadedFilePaths.push_back(name);
        m_settings->Save();
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
            {
                SaveFile();
            }
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
    case SDLK_F1:
        {
            string path = m_settings->GetFilePath();
            auto file = FindFile(path.c_str());
            if (file)
            {
                m_editWindow->SetActiveFile(file);
            }
            else
            {
                m_settings->Save();
                LoadFile(path.c_str());
            }
            return;
        }
        break;

    case SDLK_F10:
        // single step
        while (!m_emulator->Step());
        return;

    case SDLK_F11:
        m_fullscreen = !m_fullscreen;
        if (m_fullscreen)
        {
            SDL_SetWindowFullscreen(m_window, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        {
            SDL_SetWindowFullscreen(m_window, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            
        }
        return;
    case SDLK_F5:
        {
            auto file = m_editWindow->GetActiveFile();
            if (file && HasExtension(file->GetName().c_str(), ".asm"))
            {
                m_settings->Save();
                file->Save();

                size_t lastindex = file->GetPath().find_last_of(".");
                string prgname = file->GetPath().substr(0, lastindex) + ".prg";

#if defined(_WIN32)
                STARTUPINFOA info = { sizeof(info) };
                PROCESS_INFORMATION processInfo;
                string path = "F:\\Emulators\\C64\\Vice3.6\\bin\\x64sc.exe";
                string cmdLine = "-autostartprgdiskimage " + prgname;
                if (CreateProcessA(path.c_str(), (char *)cmdLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
                {
                    CloseHandle(processInfo.hProcess);
                    CloseHandle(processInfo.hThread);
                }
#endif
            }
            return;
        }
        break;
    }

    m_editWindow->OnKeyDown(e);
}

void Application::OnKeyUp(SDL_Event* e)
{
    m_editWindow->OnKeyUp(e);
}

int Application::GetCurrentIndent(string& chars)
{
    int indent = 0;
    for (int i = 0; i<chars.size(); i++)
    {
        if (chars[i] == ' ')
            indent++;
        else if (chars[i] == '\t')
            indent += (m_settings->tabWidth - (indent % m_settings->tabWidth));
        else
            break;
    }
    return indent;
}

void Application::ReplaceIndent(string& chars, int newIndent)
{
    int lastIndent = 0;
    while (lastIndent < chars.size() && (chars[lastIndent] == ' ' || chars[lastIndent] == '\t'))
        lastIndent++;

    chars.erase(chars.begin(), chars.begin() + lastIndent);

    if (m_settings->tabsToSpaces)
    {
        for (int i=0; i<newIndent; i++)
            chars.insert(chars.begin(), ' ');
    }
    else
    {
        int tabs = newIndent / m_settings->tabWidth;
        for (int i = 0; i < tabs; i++)
            chars.insert(chars.begin(), '\t');
    }
}

void Application::Cmd_IndentLines(int startLine, int endLine)
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();
        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);
        for (int l = startLine; l <= endLine; l++)
        {
            auto line = file->GetLines()[l];
            auto& chars = line->GetChars();
            auto copy = chars;
            int indent = GetCurrentIndent(copy);
            indent = ((((indent + m_settings->tabWidth - 1) / m_settings->tabWidth) + 1) * m_settings->tabWidth);
            ReplaceIndent(copy, indent);
            cmd->PushReplace(l, copy);
        }

        bool isMarked;
        int markStartLine, markStartColumn, markEndLine, markEndColumn;
        m_editWindow->GetMarking(isMarked, markStartLine, markStartColumn, markEndLine, markEndColumn);
        cmd->SetPostMarking(markStartLine, markStartColumn, markEndLine, markEndColumn);

        file->GetCmdManager()->PushCmd(cmd);
        cmd->Do();
    }
}

void Application::Cmd_SearchAndReplace(const string& searchStr, const string& replaceStr, int startLine, int startColumn, int endLine, int endColumn)
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();
        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);

        int outLine = oldActiveLine;
        int outCol = oldActiveCol;

        if (startLine == -1) 
        {
            startLine = 0;
            endLine = (int)file->GetLines().size()-1;
            startColumn = 0;
            endColumn = (int)file->GetLines().back()->GetChars().size();
        }

        for (int lnIdx = startLine; lnIdx <= endLine; lnIdx++)
        {
            auto sl = file->GetLines()[lnIdx];
            auto& chars = sl->GetChars();

            size_t firstChar = 0;
            size_t lastChar = chars.size();

            if (lnIdx == startLine)
            {
                firstChar = startColumn;
            }
            if (lnIdx == endLine)
            {
                lastChar = endColumn;
            }

            size_t foundIdx = chars.find(searchStr, firstChar);
            if (foundIdx != string::npos)
            {
                string copy = chars;
                size_t searchLoc = firstChar;
                while (foundIdx != string::npos)
                {
                    copy.replace(foundIdx, searchStr.size(), replaceStr);
                    searchLoc = foundIdx + replaceStr.size();
                    foundIdx = copy.find(searchStr, searchLoc);

                    if (lnIdx > outLine)
                    {
                        outLine = lnIdx;
                        outCol = (int)searchLoc;
                    }
                }
                cmd->PushReplace(lnIdx, copy);
            }
        }
        cmd->SetNewActiveLineCol(outLine, outCol);
        file->GetCmdManager()->PushCmd(cmd);
        cmd->Do();
    }
}


void Application::Cmd_UndentLines(int startLine, int endLine)
{
    SourceFile* file = m_editWindow->GetActiveFile();
    if (file)
    {
        int oldActiveLine = m_editWindow->GetActiveLine();
        int oldActiveCol = m_editWindow->GetActiveCol();
        auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);
        for (int l = startLine; l <= endLine; l++)
        {
            auto line = file->GetLines()[l];
            auto& chars = line->GetChars();
            auto copy = chars;
            int indent = GetCurrentIndent(copy);
            int newIndent = max(0, ((((indent + m_settings->tabWidth - 1) / m_settings->tabWidth) - 1) * m_settings->tabWidth));
            if (indent != newIndent)
            {
                ReplaceIndent(copy, newIndent);
                cmd->PushReplace(l, copy);
            }
        }

        if (cmd->Size() > 0)
        {
            bool isMarked;
            int markStartLine, markStartColumn, markEndLine, markEndColumn;
            m_editWindow->GetMarking(isMarked, markStartLine, markStartColumn, markEndLine, markEndColumn);
            cmd->SetPostMarking(markStartLine, markStartColumn, markEndLine, markEndColumn);

            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
        else
            delete cmd;
    }
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

bool StrIsAllSpace(const string &str)
{
    for (auto c : str)
    {
        if (c != ' ')
            return false;
    }
    return true;
}

void Application::SetCursor( CursorType ct)
{
    SDL_SetCursor(m_cursors[ct]);
}

void Application::Cmd_BackspaceChar()
{
    auto settings = gApp->GetSettings();

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

            // if all previous characters are spaces,  and we are spaces AS tabs,  then remove spaces back to the last tab mark
            if (settings->tabsToSpaces && StrIsAllSpace(copy))
            {
                int effSize = oldActiveCol - 1;
                int prevTab = effSize - (effSize % settings->tabWidth);
                copy.resize(prevTab);
                cmd->SetNewActiveLineCol(oldActiveLine, prevTab);
            }
            else
            {
                copy.erase(copy.begin() + (oldActiveCol - 1));
                cmd->SetNewActiveLineCol(oldActiveLine, oldActiveCol - 1);
            }

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
        vector<string> copyBuffer;
        if (sl == el)
        {
            auto line = file->GetLines()[sl];
            copyBuffer.push_back(string(&line->GetChars()[sc], &line->GetChars()[ec]));
            CopyToClipboard(copyBuffer);
        }
        else
        {
            auto line = file->GetLines()[sl];
            copyBuffer.push_back(string(&line->GetChars()[sc], line->GetChars().size() - sc));

            for (int i = sl + 1; i < el; i++)
            {
                line = file->GetLines()[i];
                copyBuffer.push_back(string(&line->GetChars()[0], line->GetChars().size()));
            }

            line = file->GetLines()[el];
            copyBuffer.push_back(string(&line->GetChars()[0], ec));
            CopyToClipboard(copyBuffer);
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

    vector<string> copyBuffer;
    if (sl == el)
    {
        auto line = file->GetLines()[sl];
        copyBuffer.push_back(string(&line->GetChars()[sc], ec - sc));
        CopyToClipboard(copyBuffer);
    }
    else
    {
        auto line = file->GetLines()[sl];
        copyBuffer.push_back(string(&line->GetChars()[sc], line->GetChars().size() - sc));

        for (int i = sl + 1; i < el; i++)
        {
            line = file->GetLines()[i];
            copyBuffer.push_back(string(&line->GetChars()[0], line->GetChars().size()));
        }

        line = file->GetLines()[el];
        copyBuffer.push_back(string(&line->GetChars()[0], ec));
        CopyToClipboard(copyBuffer);
    }
}

void Application::Cmd_PasteArea(SourceFile* file)
{
    vector<string> copyBuffer;
    CopyFromClipboard(copyBuffer);
    if (!copyBuffer.empty())
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
            int endMarkingLine = oldActiveLine + (int)copyBuffer.size() - 1;
            int endMarkingCol = (int)copyBuffer.back().size();

            string out;
            auto cmd = new CmdChangeLines(file, oldActiveLine, oldActiveCol);
            for (int i = 0; i < copyBuffer.size(); i++)
            {
                auto& cbLine = copyBuffer[i];

                out.clear();
                if (i == 0)
                    out = copyStart;
                out.insert(out.end(), cbLine.begin(), cbLine.end());

                if (i == copyBuffer.size() - 1)
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
            file->GetCmdManager()->PushCmd(cmd);
            cmd->Do();
        }
    }
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
        if (settings->autoIndent && (m_editWindow->GetActiveCol() != 0))
        {
            // count indent in spaces...
            int countSpaces = GetCurrentIndent(chars);
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
