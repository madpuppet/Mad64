#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"
#include "editWindow.h"
#include "compiler.h"
#include "cmdManager.h"
#include "appSettings.h"
#include "logWindow.h"
#include "dockableManager.h"
#include "emulatorc64.h"

class DockableWindow_Log;
class DockableWindow_EmulatorScreen;
class DockableWindow_SearchAndReplace;
class DockableWindow_MemoryDump;
class DockableWindow_MemoryImage;

enum CursorType
{
    Cursor_Arrow,
    Cursor_IBeam,
    Cursor_Horiz,
    Cursor_Vert,
    Cursor_Hand,

    Cursor_MAX
};

class Application
{
public:
    Application();
    ~Application();

    int MainLoop();
    bool IsShuttingDown() { return m_quit; }
    bool IsEmulatorRunning() { return m_runEmulation; }

    TTF_Font* GetFont() { return m_font; }
    TTF_Font* GetFontC64() { return m_fontC64; }
    SDL_Renderer* GetRenderer() { return m_renderer; }
    SDL_Window* GetWindow() { return m_window; }
    AppSettings* GetSettings() { return m_settings; }
    Compiler* GetCompiler() { return m_compiler; }
    EmulatorC64* GetEmulator() { return m_emulator; }
    EditWindow* GetEditWindow() { return m_editWindow; }
    LogWindow* GetLogWindow() { return m_logWindow; }
    DockableManager* GetDockableMgr() { return m_dockableMgr; }

    DockableWindow_Log* GetWindowCompiler() { return m_windowCompiler; }
    DockableWindow_Log* GetWindowHelp() { return m_windowHelp; }
    DockableWindow_Log* GetWindowRegisters() { return m_windowRegisters; }
    DockableWindow_MemoryDump* GetWindowMemoryDump() { return m_windowMemoryDump; }
    DockableWindow_EmulatorScreen* GetWindowEmulatorScreen() { return m_windowEmulatorScreen; }
    DockableWindow_Log* GetWindowLabels() { return m_windowLabels; }
    DockableWindow_SearchAndReplace* GetWindowSearchAndReplace() { return m_windowSearchAndReplace; }
    DockableWindow_MemoryImage* GetWindowMemoryImage() { return m_windowMemoryImage; }

    int GetWhiteSpaceWidth() { return m_whiteSpaceWidth; }
    float GetTimeDelta() { return m_timeDelta; }
    void ApplyBreakpoints();
    void ToggleMemoryBreakpoint(u16 addr);
    void ClearAllMemoryBreakpoints();
    bool IsMemoryBreakpointInRange(u16 addrStart, u16 length);

    void SetCursor(CursorType ct);

    // COMMANDS - undo'able
    void Cmd_InsertChar(char ch);
    void Cmd_BackspaceChar();
    void Cmd_DeleteChar();
    void Cmd_InsertNewLine();
    void Cmd_InsertSpaces(int count);
    void Cmd_DeleteArea(SourceFile* file, int startLine, int startColumn, int endLine, int endColumn, bool toCopyBuffer);
    void Cmd_CopyArea(SourceFile* file, int startLine, int startColumn, int endLine, int endColumn);
    void Cmd_PasteArea(SourceFile* file);
    void Cmd_UndentLines(int startLine, int endLine);
    void Cmd_IndentLines(int startLine, int endLine);
    void Cmd_SearchAndReplace(const string& searchStr, const string& replaceStr, int startLine=-1, int startColumn=-1, int endLine=-1, int endColumn=-1);

    // helpers
    int GetCurrentIndent(string& chars);
    void ReplaceIndent(string& chars, int newIndent);
    SourceFile* FindFile(const char* path);
    void ReloadFont();
    void ResetAndStopEmulator();

    void SetCaptureMouseMotion(MouseMotionCaptureHook hook);
    void SetCaptureTextInput(TextCaptureHook hook);
    void SetCaptureKeyInput(KeyCaptureHook hook);

    void PushClippingRect(SDL_Renderer *r, SDL_Rect *rect);
    void PopClippingRect(SDL_Renderer* r);
    SDL_Rect GetActiveClipRect(SDL_Renderer* r);

    void DoEmuColdReset();
    void DoEmuTogglePlay();

protected:
    vector<SourceFile*> m_sourceFiles;
    vector<u16> m_memoryBreakpoints;

    // cursors
    SDL_Cursor* m_cursors[Cursor_MAX];

    MouseMotionCaptureHook m_mouseMotionCapture;
    TextCaptureHook m_textInputCapture;
    KeyCaptureHook m_keyInputCapture;

    // GENERAL METHODS
    void Update();
    void Draw();
    bool LoadFile();
    bool LoadFile(const char* path);
    void AddFile(SourceFile* sf);

    bool ImportFile();
    void SaveFile();
    void SaveFileAs();
    void CreateNewFile();
    void CloseFile();

    // EVENTS
    void HandleEvent(SDL_Event* e);
    void OnKeyDown(SDL_Event* e);
    void OnKeyUp(SDL_Event* e);

    // RESOURCES
    SDL_Window* m_window;
    TTF_Font* m_font;
    TTF_Font* m_fontC64;
    int m_whiteSpaceWidth;
    SDL_Renderer* m_renderer;
    EditWindow* m_editWindow;
    LogWindow* m_logWindow;
    Compiler* m_compiler;
    EmulatorC64* m_emulator;
    DockableManager* m_dockableMgr;
    bool m_quit;
    bool m_fullscreen;
    bool m_emulatorCaptureInput;
    AppSettings* m_settings;

    DockableWindow_Log* m_windowCompiler;
    DockableWindow_Log* m_windowHelp;
    DockableWindow_Log* m_windowRegisters;
    DockableWindow_MemoryDump* m_windowMemoryDump;
    DockableWindow_EmulatorScreen* m_windowEmulatorScreen;
    DockableWindow_Log* m_windowLabels;
    DockableWindow_SearchAndReplace* m_windowSearchAndReplace;
    DockableWindow_MemoryImage* m_windowMemoryImage;

    enum InputCapture
    {
        Capture_None,
        Capture_EditWindow,
        Capture_LogWindow
    };
    InputCapture m_mouseCapture;
    InputCapture m_keyCapture;

    // joystick
    SDL_Joystick* m_joystick[2];

    // double click
    bool m_latchDoubleClick;
    int m_clickX;
    int m_clickY;
    u64 m_clickTime;
    u64 m_frameTick;
    float m_timeDelta;
    int m_focusedWindowID;

    int m_mouseWindowID;
    int m_mouseX;
    int m_mouseY;

    // emulation
    bool m_runEmulation;
    float m_flashScreenRed;

    // clipping rects
    struct ClippingStack
    {
        SDL_Renderer* m_renderer;
        vector<SDL_Rect> m_rects;
    };
    vector<ClippingStack*> m_clippingStacks;
    ClippingStack* FindClippingStack(SDL_Renderer* r);
    void ApplyClippingStack(ClippingStack* stack);
};
extern Application *gApp;


struct ClipRectScope
{
    ClipRectScope(SDL_Renderer* r, SDL_Rect* rect)
    {
        gApp->PushClippingRect(r, rect);
        m_r = r;
    }
    ~ClipRectScope()
    {
        gApp->PopClippingRect(m_r);
    }
    SDL_Renderer* m_r;
};

