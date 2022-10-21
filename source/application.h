#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"
#include "editWindow.h"
#include "compiler.h"
#include "cmdManager.h"
#include "AppSettings.h"

class Application
{
public:
    Application();
    ~Application();

    int MainLoop();

    TTF_Font* GetFont() { return m_font; }
    SDL_Renderer* GetRenderer() { return m_renderer; }
    SDL_Window* GetWindow() { return m_window; }
    AppSettings* GetSettings() { return m_settings; }
    Compiler* GetCompiler() { return m_compiler; }
    EditWindow* GetEditWindow() { return m_editWindow; }
    int GetWhiteSpaceWidth() { return m_whiteSpaceWidth; }

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

    // helpers
    int GetCurrentIndent(string& chars);
    void ReplaceIndent(string& chars, int newIndent);
    SourceFile* FindFile(const char* path);
    void ReloadFont();

protected:
    vector<SourceFile*> m_sourceFiles;

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

    // RESOURCES
    SDL_Window* m_window;
    TTF_Font* m_font;
    int m_whiteSpaceWidth;
    SDL_Renderer* m_renderer;
    EditWindow* m_editWindow;
    Compiler* m_compiler;
    bool m_quit;
    bool m_repaint;
    AppSettings* m_settings;
};
extern Application *gApp;
