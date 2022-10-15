#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"
#include "editWindow.h"
#include "compiler.h"
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

protected:
    vector<SourceFile*> m_sourceFiles;

    // GENERAL METHODS
    void Update();
    void Draw();
    void LoadFile();
    void SaveFile();
    void CreateNewFile();

    // EVENTS
    void HandleEvent(SDL_Event* e);
    void OnKeyDown(SDL_Event* e);

    // RESOURCES
    SDL_Window* m_window;
    TTF_Font* m_font;
    SDL_Renderer* m_renderer;
    EditWindow* m_editWindow;
    Compiler* m_compiler;
    bool m_quit;
    bool m_repaint;
    AppSettings* m_settings;
};
extern Application *gApp;
