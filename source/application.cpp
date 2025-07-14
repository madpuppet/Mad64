#include "common.h"
#include "application.h"
#include "tinyfiledialogs.h"
#include "dockableManager.h"
#include "dockableWindow_log.h"
#include "dockableWindow_emulatorScreen.h"
#include "dockableWindow_searchAndReplace.h"
#include "dockableWindow_memoryDump.h"
#include "dockableWindow_memoryImage.h"

// todo - cross platform way to launch the emulator
#if defined(_WIN32)
#include <Windows.h>
#endif

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

Application *gApp;

#define APP_TITLE "MAD64"
#define VERSION "2022.09"

Application::Application()
{
    m_quit = false;
    m_latchDoubleClick = 0;
    m_clickX = 0;
    m_clickY = 0;
    m_clickTime = 0;
    m_focusedWindowID = -1;
    m_flashScreenRed = 0.0f;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0)
    {
        Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        exit(0);
    }

    LogStart();
    gApp = this;
    m_fullscreen = false;

    //Create window
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
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

        m_fontC64 = TTF_OpenFont("fontc64.ttf", m_settings->fontSize);

        Log("Create Renderer");

#if defined(__APPLE__)
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);
#else
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
#endif

        if (m_renderer == NULL)
        {
            Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            m_quit = true;
            exit(0);
        }

        Log("Create Modules");
        m_cachedFontRenderer = new CachedFontRenderer();
        m_dockableMgr = new DockableManager();
        m_editWindow = new EditWindow();
        m_compiler = new Compiler();
        m_emulator = new EmulatorC64();

        m_windowCompiler = new DockableWindow_Log("Compiler Output");
        m_windowHelp = new DockableWindow_Log("Contextual Help");
        m_windowRegisters = new DockableWindow_Log("System Registers");
        m_windowLabels = new DockableWindow_Log("Labels");
        m_windowEmulatorScreen = new DockableWindow_EmulatorScreen("Emulator Screen");
        m_windowMemoryDump = new DockableWindow_MemoryDump("Memory Dump");
        m_windowSearchAndReplace = new DockableWindow_SearchAndReplace("Search and Replace");
        m_windowMemoryImage = new DockableWindow_MemoryImage("Memory Usage");

        m_dockableMgr->AddWindow(m_windowSearchAndReplace, "S&R", true, true);
        m_dockableMgr->AddWindow(m_windowCompiler, "COM", true, true);
        m_dockableMgr->AddWindow(m_windowHelp, "HLP", true, true);
        m_dockableMgr->AddWindow(m_windowLabels, "LAB", true, true);
        m_dockableMgr->AddWindow(m_windowRegisters, "REG", true, true);
        m_dockableMgr->AddWindow(m_windowMemoryImage, "MEM", true, true);
        m_dockableMgr->AddWindow(m_windowEmulatorScreen, "EMU", true, true);
        m_dockableMgr->AddWindow(m_windowMemoryDump, "DMP", true, true);
        m_dockableMgr->ParseSettings(m_settings->appFile);

        Log("Reload files");
        int loadIdx = 0;
        while (loadIdx < m_settings->loadedFilePaths.size())
        {
            if (!LoadFile(m_settings->loadedFilePaths[loadIdx].c_str()))
            {
                m_settings->loadedFilePaths.erase(m_settings->loadedFilePaths.begin() + loadIdx);
            }
            else
            {
                loadIdx++;
            }
        }
        m_editWindow->SetActiveFileIdx(m_settings->activeLoadedFilePath);

        m_mouseCapture = Capture_None;
        m_keyCapture = Capture_None;
    }

    m_joystick[0] = SDL_JoystickOpen(0);
    m_joystick[1] = SDL_JoystickOpen(1);

    SDL_StartTextInput();

    m_editWindow->CalcRects();
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

    TTF_CloseFont(m_fontC64);
    m_fontC64 = TTF_OpenFont("fontc64.ttf", m_settings->fontSize);

    m_editWindow->ClearVisuals();

    for (auto f : m_sourceFiles)
    {
        f->ClearAllVisuals();
    }

}

Application::~Application()
{
    SDL_JoystickClose(m_joystick[0]);
    SDL_JoystickClose(m_joystick[1]);

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
#if 0
        if (m_settings->lowCPUMode || m_focusedWindowID == -1)
        {
            int timeout = (m_runEmulation || m_editWindow->IsAutoScrolling()) ? 10 : 200;
            if (SDL_WaitEventTimeout(&e,timeout))
                HandleEvent(&e);
        }
        else
#endif
        {
            while (SDL_PollEvent(&e))
                HandleEvent(&e);
        }

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
    delete m_settings;
    return 0;
}

void Application::Update()
{
    m_editWindow->Update();
    m_compiler->Update();
    m_emulator->Update();

    if (m_runEmulation && m_editWindow->IsActiveAsmFile())
    {
//        m_timeDelta = SDL_min(m_timeDelta, 1 / 30.0f);

        int cycles = (int)(m_emulator->CyclesPerSecond() * m_timeDelta);
        bool complete = false;
        for (int i = 0; i < cycles; i++)
        {
            complete = m_emulator->Step();
            if (m_emulator->WasBreakpointHit())
            {
                m_emulator->ClearBreakpointHit();
                m_editWindow->GotoEmuPC();
                m_runEmulation = false;
                m_flashScreenRed = 1.0f;
                break;
            }
        }
        if (!complete)
            while (!m_emulator->Step());
    }

    m_flashScreenRed = max(0.0f, m_flashScreenRed - TIMEDELTA*5.0f);
}

float Application::CalcDPI()
{
    int windowWidth, windowHeight;
    int glWidth, glHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);
    SDL_GL_GetDrawableSize(m_window, &glWidth, &glHeight);
    float dpi = (float)glWidth / (float)windowWidth;
    return dpi;
}

void Application::Draw()
{
    {
        ClipRectScope crs(m_renderer, nullptr);
        SDL_SetRenderDrawColor(m_renderer, m_settings->backColor.r, m_settings->backColor.g, m_settings->backColor.b, 255);
        SDL_RenderFillRect(m_renderer, NULL);

        m_editWindow->Draw();
        m_dockableMgr->Draw();

        if (m_flashScreenRed)
        {
            SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, (int)(m_flashScreenRed * 255));
            SDL_RenderFillRect(m_renderer, NULL);
        }
    }

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
            float dpi = CalcDPI();
            e->button.x = e->button.x * dpi;
            e->button.y = e->button.y * dpi;

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

            SetCaptureTextInput(nullptr);
            SetCaptureKeyInput(nullptr);

            if (!m_dockableMgr->OnMouseDown(e) && (e->button.windowID == SDL_GetWindowID(m_window)))
            {
                m_editWindow->OnMouseDown(e);
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        {
            float dpi = CalcDPI();
            e->button.x = e->button.x * dpi;
            e->button.y = e->button.y * dpi;

            m_mouseMotionCapture = nullptr;
            m_dockableMgr->OnMouseUp(e);
            m_editWindow->OnMouseUp(e);
        }
        break;
    case SDL_MOUSEMOTION:
        {
            float dpi = CalcDPI();
            e->motion.x = e->motion.x * dpi;
            e->motion.y = e->motion.y * dpi;

            m_mouseWindowID = e->motion.windowID;

            if (!m_latchDoubleClick)
            {
                if (m_mouseMotionCapture != nullptr)
                    m_mouseMotionCapture(false, e->motion.x, e->motion.y);
                else
                {
                    gApp->SetCursor(Cursor_Arrow);
                    m_dockableMgr->UpdateCursor(e->motion.windowID, e->motion.x, e->motion.y);
                    if (!m_dockableMgr->OnMouseMotion(e) && (e->motion.windowID == SDL_GetWindowID(m_window)))
                    {
                        m_editWindow->UpdateCursor(e->motion.x, e->motion.y);
                        m_editWindow->OnMouseMotion(e);
                    }
                }
            }
        }
        break;
    case SDL_MOUSEWHEEL:
        if (!m_dockableMgr->OnMouseWheel(m_mouseWindowID, m_mouseX, m_mouseY, e->wheel.x, e->wheel.y) && (m_mouseWindowID == SDL_GetWindowID(m_window)))
        {
            m_editWindow->OnMouseWheel(m_mouseWindowID, m_mouseX, m_mouseY, e->wheel.x, e->wheel.y);
        }
        break;
    case SDL_TEXTINPUT:
        if (m_textInputCapture != nullptr)
        {
            m_textInputCapture(false, string(e->text.text));
        }
        else
            m_editWindow->OnTextInput(e);
        break;
    case SDL_KEYDOWN:
        OnKeyDown(e);
        break;
    case SDL_KEYUP:
        OnKeyUp(e);
        break;
    case SDL_WINDOWEVENT:
        if (e->window.windowID == SDL_GetWindowID(m_window))
        {
            switch (e->window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                    m_quit = true;
                    break;

                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    m_editWindow->OnResize();
                    break;
            }
        }

        switch (e->window.event)
        {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                if (e->window.windowID == SDL_GetWindowID(m_window) || m_dockableMgr->FindWindowByID(e->window.windowID) != nullptr)
                    m_focusedWindowID = e->window.windowID;
                else
                    m_focusedWindowID = -1;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                if (e->window.windowID == m_focusedWindowID)
                    m_focusedWindowID = -1;
                break;
        }
        break;
    case SDL_JOYBUTTONDOWN:
        m_emulator->OnJoystickButtonDown(e);
        return;
    case SDL_JOYBUTTONUP:
        m_emulator->OnJoystickButtonUp(e);
        return;
    case SDL_JOYAXISMOTION:
        m_emulator->OnJoystickAxisMotion(e);
        return;
    }
}

void AddDataLines(u8 *data, SourceFile *sf, int &baseIdx, int &idx)
{
    while (baseIdx < idx)
    {
        int bytes = SDL_min(16, idx - baseIdx);
        string line = "    dc.b ";
        for (int i = 0; i < bytes; i++)
        {
            if (i != 0)
                line += ", ";
            line += FormatString("$%02x", data[baseIdx + i]);
        }
        sf->GetLines().push_back(new SourceLine(line));
        baseIdx += bytes;
    }
}

bool Application::ImportFile()
{
    const char* path = gApp->GetSettings()->activeFilePath.c_str();
    const char* patterns[3] = { "*.prg", "*.d64", "*.crt" };
    const char* file = tinyfd_openFileDialog("Import binary", path, 3, patterns, nullptr, false);
    if (file)
    {
        Log("Load Binary: %s", file);
        if (HasExtension(file, ".prg"))
        {
            size_t size;
            u8* data = (u8*)SDL_LoadFile(file, &size);
            auto proc = m_emulator->GetCpu();
            string path = file;
            size_t lastindex = path.find_last_of(".");
            string asmname = path.substr(0, lastindex) + ".asm";

            auto sf = new SourceFile(asmname.c_str());
            if (data)
            {
                u16 addr = ((u16)data[1] << 8) | (u16)data[0];
                sf->GetLines().push_back(new SourceLine(FormatString("    * = $%04x", addr)));

                int idx = 2;
                int baseIdx = 2;
                Cpu6502::DisassembledLine dl;
                while (idx < size)
                {
                    int remain = SDL_min(3, (int)size - idx);
                    for (int i = 0; i < remain; i++)
                    {
                        dl.ram[i] = data[idx+i];
                    }
                    dl.size = remain;
                    dl.addr = addr + idx;

                    int dataBytes = idx - baseIdx;
                    if (dl.ram[0] != 0 && proc->Disassemble(dl, false))
                    {
                        AddDataLines(data, sf, baseIdx, idx);
                        sf->GetLines().push_back(new SourceLine(dl.text));
                        idx += dl.size;
                        baseIdx = idx;
                    }
                    else
                        idx++;
                }
                AddDataLines(data, sf, baseIdx, idx);
                AddFile(sf);
                SDL_free(data);
            }
        }
    }
    return false;
}

bool Application::LoadFile()
{
    const char* path = gApp->GetSettings()->activeFilePath.c_str();
    const char* patterns[3] = { "*.asm", "*.bas", "*.txt"};
    const char *file = tinyfd_openFileDialog("Load file", path, 3, patterns, nullptr, false);
    if (file)
    {
        Log("Load File: %s", file);
        return LoadFile(file);
    }
    return false;
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

void Application::AddFile(SourceFile* sf)
{
    // tokenize..
    for (auto line : sf->GetLines())
    {
        line->Tokenize();
    }

    // compile & vizualize compiled elements
    if (HasExtension(sf->GetPath().c_str(), ".asm"))
        gApp->GetCompiler()->Compile(sf);

    m_sourceFiles.push_back(sf);

    // alert renderer of update
    m_editWindow->OnFileLoaded(sf);
    m_dockableMgr->OnFileChange();

    bool exists = false;
    for (auto& p : m_settings->loadedFilePaths)
    {
        if (SDL_strcasecmp(p.c_str(), sf->GetPath().c_str()) == 0)
        {
            exists = true;
            break;
        }
    }
    if (!exists)
    {
        m_settings->loadedFilePaths.push_back(sf->GetPath());
        m_settings->Save();
    }

    if (HasExtension(sf->GetPath().c_str(), ".asm"))
    {
        m_compiler->Compile(sf);
        m_compiler->LogContextualHelp(sf, m_editWindow->GetActiveLine());
    }

    m_editWindow->CalcRects();
}

bool Application::LoadFile(const char* path)
{
    auto file = FindFile(path);
    if (file)
    {
        m_editWindow->SetActiveFile(file);
        return true;
    }

    auto source = new SourceFile(path);
    if (source->Load())
    {
        AddFile(source);
        return true;
    }
    else
    {
        delete source;
    }
    return false;
}

void Application::SaveFile()
{
    auto activeFile = m_editWindow->GetActiveFile();
    if (activeFile)
    {
        activeFile->Save();
    }
}

void Application::SaveFileAs()
{
    auto activeFile = m_editWindow->GetActiveFile();
    if (activeFile)
    {
        string filepath = activeFile->GetPath();
        const char* patterns[3] = { "*.asm", "*.bas", "*.*" };
        const char* filename = tinyfd_saveFileDialog("Save As", filepath.c_str(), 3, patterns, "");
        if (filename)
        {
            activeFile->SetPath(filename);
            activeFile->Save();
        }
    }
}

void Application::CloseFile()
{
    auto activeFile = m_editWindow->GetActiveFile();
    if (activeFile)
    {
        m_editWindow->OnFileClosed(activeFile);
        m_sourceFiles.erase(std::remove(m_sourceFiles.begin(), m_sourceFiles.end(), activeFile));

        auto fileIt = std::find(m_settings->loadedFilePaths.begin(), m_settings->loadedFilePaths.end(), string(activeFile->GetPath()));
        if (fileIt != m_settings->loadedFilePaths.end())
            m_settings->loadedFilePaths.erase(fileIt);
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

        m_settings->activeFilePath = GetPath(file);
        m_settings->loadedFilePaths.push_back(name);
        m_settings->Save();
    }
}

void Application::ClearAllMemoryBreakpoints()
{
    m_memoryBreakpoints.clear();
    ApplyBreakpoints();
}

void Application::ToggleMemoryBreakpoint(u16 addr)
{
    auto it = std::find(m_memoryBreakpoints.begin(), m_memoryBreakpoints.end(), addr);
    if (it == m_memoryBreakpoints.end())
        m_memoryBreakpoints.push_back(addr);
    else
        m_memoryBreakpoints.erase(it);
    ApplyBreakpoints();
}

void Application::ApplyBreakpoints()
{
    auto file = m_editWindow->GetActiveFile();
    auto csi = file->GetCompileInfo();
    if (csi)
    {
        auto lines = file->GetLines();
        auto clines = csi->m_lines;
        if (clines.size() == lines.size())
        {
            m_emulator->ClearAllBreakpoints();
            for (size_t i = 0; i < lines.size(); i++)
            {
                auto l = lines[i];
                auto cl = clines[i];
                u8 brk = l->GetBreakpoint();
                if (brk && cl->data.size() > 0)
                    m_emulator->AddBreakpoint((u16)cl->memAddr, (u16)cl->data.size(), brk);
            }
            for (auto addr : m_memoryBreakpoints)
            {
                m_emulator->AddBreakpoint(addr, 1, BRK_Read | BRK_Write);
            }
        }
    }
}

bool Application::IsMemoryBreakpointInRange(u16 addr, u16 length)
{
    for (auto bk : m_memoryBreakpoints)
    {
        if (bk >= addr && bk < addr+length)
            return true;
    }
    return false;
}

void Application::OnKeyDown(SDL_Event* e)
{
    if (m_keyInputCapture)
    {
        m_keyInputCapture(false, true, (u32)e->key.keysym.sym, (u32)e->key.keysym.mod);
    }

    if (!m_keyInputCapture || e->key.keysym.mod & KMOD_ALT)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_i:
                if (e->key.keysym.mod & KMOD_CTRL)
                {
                    if (!e->key.repeat)
                        ImportFile();
                    return;
                }
                break;

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
                        if (e->key.keysym.mod & KMOD_SHIFT)
                        {
                            SaveFileAs();
                        }
                        else
                        {
                            SaveFile();
                        }
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
            case SDLK_F9:
                {
                    if (m_editWindow->IsActiveAsmFile())
                    {
                        auto file = m_editWindow->GetActiveFile();
                        auto csi = file->GetCompileInfo();
                        {
                            int line = m_editWindow->GetActiveLine();
                            auto l = file->GetLines()[line];
                            auto cl = csi->m_lines[line];
                            if (l->GetBreakpoint())
                                l->SetBreakpoint(0);
                            else
                            {
                                l->SetBreakpoint(cl->type == LT_Instruction ? BRK_Execute : BRK_Read | BRK_Write);
                            }
                            gApp->ApplyBreakpoints();
                        }
                    }
                }
                return;
            case SDLK_F10:
                if (e->key.keysym.mod & KMOD_CTRL)
                {
                    DoEmuSingleFrame();
                }
                else if (e->key.keysym.mod & KMOD_SHIFT)
                {
                    DoEmuSingleRow();
                }
                else if (e->key.keysym.mod & KMOD_ALT)
                {
                    DoEmuSingleCycle();
                }
                else
                {
                    DoEmuSingleInstruction();
                }
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
            case SDLK_F6:
                {
                    DoEmuColdReset();
                }
                break;

            case SDLK_F5:
                {
                    if (m_editWindow->IsActiveAsmFile())
                    {
                        if ((e->key.keysym.mod & KMOD_CTRL) && (e->key.keysym.mod & KMOD_SHIFT))
                        {
                            // CTRL-SHIFT F5 - launch in vice
#if defined(_WIN32)
                            auto file = m_editWindow->GetActiveFile();
                            if (file && HasExtension(file->GetName().c_str(), ".asm"))
                            {
                                m_settings->Save();
                                file->Save();

                                size_t lastindex = file->GetPath().find_last_of(".");
                                string prgname = file->GetPath().substr(0, lastindex) + ".prg";

                                STARTUPINFOA info = { sizeof(info) };
                                PROCESS_INFORMATION processInfo;
                                string path = "G:\\Emulators\\C64\\Vice3.6\\bin\\x64sc.exe";
                                string cmdLine = "-autostartprgdiskimage \"" + prgname + "\"";
                                if (CreateProcessA(path.c_str(), (char*)cmdLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
                                {
                                    CloseHandle(processInfo.hProcess);
                                    CloseHandle(processInfo.hThread);
                                }
                            }
#endif
                        }
                        else if (e->key.keysym.mod & KMOD_CTRL)
                        {
                            DoEmuResetAndPlay();
                        }
                        else
                        {
                            // F5 - toggle emulator
                            m_runEmulation = !m_runEmulation;
                        }
                    }
                }
                break;
        }
        m_editWindow->OnKeyDown(e);
    }
}


void Application::DoEmuSingleCycle()
{
    // rasterline step
    m_runEmulation = false;
    m_emulator->Step();
    m_editWindow->GotoEmuPC();
}
void Application::DoEmuSingleInstruction()
{
    // single step
    m_runEmulation = false;
    while (!m_emulator->Step());
    m_editWindow->GotoEmuPC();
}
void Application::DoEmuSingleRow()
{
    // rasterline step
    m_runEmulation = false;
    int line = m_emulator->GetCurrentRasterline();
    while (line == m_emulator->GetCurrentRasterline())
        m_emulator->Step();
    m_editWindow->GotoEmuPC();
}
void Application::DoEmuSingleFrame()
{
    // frame step
    m_runEmulation = false;
    while (m_emulator->GetCurrentRasterline() == 0)
        m_emulator->Step();
    while (m_emulator->GetCurrentRasterline() != 0)
        m_emulator->Step();
    m_editWindow->GotoEmuPC();
}


void Application::DoEmuResetAndPlay()
{
    if (m_editWindow->IsActiveAsmFile())
    {
        auto file = m_editWindow->GetActiveFile();
        auto compiledFile = file->GetCompileInfo();
        if (compiledFile)
        {
            // CTRL F5 - restart emulator
            // reset emulator
            auto startLabel = gApp->GetCompiler()->FindMatchingLabel(compiledFile, "start");
            if (startLabel)
            {
                ApplyBreakpoints();
                m_emulator->Reset(compiledFile->m_ramDataMap, compiledFile->m_ramMask, (u16)startLabel->m_value);
                m_runEmulation = true;
            }
        }
    }
}

void Application::DoEmuTogglePlay()
{
    m_runEmulation = !m_runEmulation;
}

void Application::DoEmuColdReset()
{
    if (m_editWindow->IsActiveAsmFile())
    {
        auto file = m_editWindow->GetActiveFile();
        auto compiledFile = file->GetCompileInfo();
        ApplyBreakpoints();
        m_emulator->ColdReset(compiledFile->m_ramDataMap, compiledFile->m_ramMask);
        m_runEmulation = false;
    }
}

void Application::OnKeyUp(SDL_Event* e)
{
    if (m_keyInputCapture)
    {
        m_keyInputCapture(false, false, (u32)e->key.keysym.sym, (u32)e->key.keysym.mod);
    }
    else
    {
        m_editWindow->OnKeyUp(e);
    }
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

void Application::ResetAndStopEmulator()
{
    m_emulator->GetVic()->Reset();
}

void Application::SetCaptureMouseMotion(MouseMotionCaptureHook hook)
{
    if (m_mouseMotionCapture != nullptr)
        m_mouseMotionCapture(true, 0, 0);

    m_mouseMotionCapture = hook; 
}
void Application::SetCaptureTextInput(TextCaptureHook hook)
{
    if (m_textInputCapture != nullptr)
        m_textInputCapture(true, "");

    m_textInputCapture = hook;
}
void Application::SetCaptureKeyInput(KeyCaptureHook hook)
{
    if (m_keyInputCapture != nullptr)
        m_keyInputCapture(true, false, 0, 0);

    m_keyInputCapture = hook;
}

Application::ClippingStack* Application::FindClippingStack(SDL_Renderer* r)
{
    // find the appropriate stack for this renderer
    ClippingStack* stack = nullptr;
    for (auto item : m_clippingStacks)
    {
        if (item->m_renderer == r)
        {
            stack = item;
        }
    }
    return stack;
}
void Application::ApplyClippingStack(ClippingStack *stack)
{
    bool fullscreen = true;
    int x1, x2, y1, y2;
    for (auto& rect : stack->m_rects)
    {
        if (rect.w == -1)
        {
            fullscreen = true;
        }
        else if (fullscreen)
        {
            x1 = rect.x;
            y1 = rect.y;
            x2 = rect.x + rect.w;
            y2 = rect.y + rect.h;
            fullscreen = false;
        }
        else
        {
            x1 = SDL_max(x1, rect.x);
            y1 = SDL_max(y1, rect.y);
            x2 = SDL_min(x2, rect.x + rect.w);
            y2 = SDL_min(y2, rect.y + rect.h);
        }
    }
    if (fullscreen)
        SDL_RenderSetClipRect(stack->m_renderer, nullptr);
    else
    {
        SDL_Rect activeRect = { x1, y1, SDL_max(0,x2 - x1), SDL_max(0,y2 - y1) };
        SDL_RenderSetClipRect(stack->m_renderer, &activeRect);
    }
}
SDL_Rect Application::GetActiveClipRect(SDL_Renderer *r)
{
    SDL_Rect rect;
    SDL_RenderGetClipRect(r, &rect);
    return rect;
}

void Application::PushClippingRect(SDL_Renderer* r, SDL_Rect* rect)
{
    ClippingStack* stack = FindClippingStack(r);

    // create a new stack if we didn't find one
    if (!stack)
    {
        stack = new ClippingStack();
        stack->m_renderer = r;
        m_clippingStacks.push_back(stack);
    }

    // push this rect onto the stack
    SDL_Rect empty = { -1,-1,-1,-1 };
    SDL_Rect clip_rect = rect ? *rect : empty;
    stack->m_rects.push_back(clip_rect);

    // calculate active rect
    ApplyClippingStack(stack);
}
void Application::PopClippingRect(SDL_Renderer* r)
{
    ClippingStack* stack = FindClippingStack(r);

    // in case stack has been destroyed during the frame...
    // may only happen if we ever need to support destroying renderers
    if (stack)
    {
        // pop top rect from stack
        stack->m_rects.pop_back();

        // calculate active rect
        ApplyClippingStack(stack);
    }
}
