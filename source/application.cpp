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
    m_window = SDL_CreateWindow("MAD64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
        copy.insert(copy.begin() + oldActiveCol, ch);
        cmd->SetNewActiveLineCol(oldActiveLine, oldActiveCol + 1);

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
void Application::Cmd_DeleteChar(SourceFile* file, int ln, int col)
{
    auto line = file->GetLines()[ln];
    if (col == line->GetChars().size())
    {
        // remove cr, so concat this line with prev
        if (ln < file->GetLines().size()-1)
        {
            auto line = file->GetLines()[ln];
            auto nextLine = file->GetLines()[ln+1];
            auto& chars = line->GetChars();
            auto& nextChars = nextLine->GetChars();

            // add line to previous line
            chars.insert(chars.end(), nextChars.begin(), nextChars.end());
            file->GetLines().erase(file->GetLines().begin() + ln + 1);
            delete nextLine;

            line->Tokenize();
            line->VisualizeText();
        }
    }
    else
    {
        auto line = file->GetLines()[ln];
        auto& chars = line->GetChars();
        chars.erase(chars.begin() + col);
        line->Tokenize();
        line->VisualizeText();
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

    if (sl == el)
    {
        auto line = file->GetLines()[sl];
        auto& chars = line->GetChars();
        chars.erase(chars.begin() + sc, chars.begin() + ec);
        line->Tokenize();
        line->VisualizeText();
    }
    else
    {
        // remove middle lines
        if ((startLine + 1) < endLine)
        {
            for (int ln = sl + 1; ln < el; ln++)
            {
                delete file->GetLines()[ln];
            }
            file->GetLines().erase(file->GetLines().begin() + sl + 1, file->GetLines().begin() + el);
        }

        // join end line to start line
        auto line = file->GetLines()[sl];
        auto nextLine = file->GetLines()[sl+1];
        auto& chars = line->GetChars();
        auto& nextChars = nextLine->GetChars();
        chars.erase(chars.begin() + sc, chars.end());
        chars.insert(chars.end(), nextChars.begin() + ec, nextChars.end());
        delete nextLine;
        file->GetLines().erase(file->GetLines().begin() + sl + 1);
        line->Tokenize();
        line->VisualizeText();
    }

    m_editWindow->ClearMarking();
    m_editWindow->GotoLineCol(sl, sc);
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

void Application::Cmd_PasteArea(SourceFile* file, int startLine, int startColumn)
{
    if (!m_copyBuffer->GetLines().empty())
    {
        if (m_copyBuffer->GetLines().size() == 1)
        {
            auto line = file->GetLines()[startLine];
            auto& chars = line->GetChars();
            auto cbLine = m_copyBuffer->GetLines()[0];
            auto& cbChars = cbLine->GetChars();
            chars.insert(chars.begin() + startColumn, cbChars.begin(), cbChars.end());
            m_editWindow->GotoLineCol(startLine, startColumn + (int)cbChars.size());
            line->Tokenize();
            line->VisualizeText();
        }
        else
        {

        }
    }
}

void Application::Cmd_OverwriteChar(SourceFile* file, int ln, int col, char ch)
{

}
void Application::Cmd_InsertNewLine(SourceFile* file, int ln, int col)
{
    auto line = file->GetLines()[ln];
    auto& chars = line->GetChars();

    auto newLine = new SourceLine();
    auto& newChars = newLine->GetChars();

    newChars.insert(newChars.begin(), chars.begin() + col, chars.end());
    chars.erase(chars.begin() + col, chars.end());

    file->GetLines().insert(file->GetLines().begin() + ln + 1, newLine);

    line->Tokenize();
    line->VisualizeText();

    newLine->Tokenize();
    newLine->VisualizeText();
    m_editWindow->GotoLineCol(ln + 1, 0);
}

void Cmd_CopyArea(SourceFile* file, int startLine, int startColumn, int endLine, int endColumn)
{

}

void Cmd_PasteArea(SourceFile* file, int startLine, int startColumn)
{

}
