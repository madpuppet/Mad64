#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"
#include "editWindow.h"
#include "compiler.h"
#include "cmdManager.h"
#include "appSettings.h"
#include "logWindow.h"
#include "emulatorc64.h"

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

    TTF_Font* GetFont() { return m_font; }
    SDL_Renderer* GetRenderer() { return m_renderer; }
    SDL_Window* GetWindow() { return m_window; }
    AppSettings* GetSettings() { return m_settings; }
    Compiler* GetCompiler() { return m_compiler; }
    EmulatorC64* GetEmulator() { return m_emulator; }
    EditWindow* GetEditWindow() { return m_editWindow; }
    LogWindow* GetLogWindow() { return m_logWindow; }
    int GetWhiteSpaceWidth() { return m_whiteSpaceWidth; }
    float GetTimeDelta() { return m_timeDelta; }

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


protected:
    vector<SourceFile*> m_sourceFiles;

    // cursors
    SDL_Cursor* m_cursors[Cursor_MAX];

    // GENERAL METHODS
    void Update();
    void Draw();
    void LoadFile();
    void LoadFile(const char* path);
    void SaveFile();
    void CreateNewFile();
    void CloseFile();

    // EVENTS
    void HandleEvent(SDL_Event* e);
    void OnKeyDown(SDL_Event* e);
    void OnKeyUp(SDL_Event* e);

    // RESOURCES
    SDL_Window* m_window;
    TTF_Font* m_font;
    int m_whiteSpaceWidth;
    SDL_Renderer* m_renderer;
    EditWindow* m_editWindow;
    LogWindow* m_logWindow;
    Compiler* m_compiler;
    EmulatorC64* m_emulator;
    bool m_quit;
    bool m_fullscreen;
    AppSettings* m_settings;

    enum InputCapture
    {
        Capture_None,
        Capture_EditWindow,
        Capture_LogWindow
    };
    InputCapture m_mouseCapture;
    InputCapture m_keyCapture;

    // double click
    bool m_latchDoubleClick;
    int m_clickX;
    int m_clickY;
    u64 m_clickTime;
    u64 m_frameTick;
    float m_timeDelta;

    // emulation
    bool m_runEmulation;
};
extern Application *gApp;
