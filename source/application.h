#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"

class Application
{
public:
    Application();
    ~Application();

    int MainLoop();

    TTF_Font* GetFont() { return m_font; }
    SDL_Renderer* GetRenderer() { return m_renderer; }
    SDL_Window* GetWindow() { return m_window; }
    UIManager* GetUI() { return m_ui; }

protected:
    vector<SourceFile*> m_sourceFiles;

    // GENERAL METHODS
    void Draw();
    void LoadFile();
    void CreateNewFile();

    // EVENTS
    void HandleEvent(SDL_Event* e);
    void OnKeyDown(SDL_Event* e);

    // RESOURCES
    SDL_Window* m_window;
    TTF_Font* m_font;
    SDL_Renderer* m_renderer;
    UIManager* m_ui;
    bool m_quit;
    bool m_repaint;
    float m_time;

    // temp
    GraphicChunk* m_tempGC;
};
extern Application *gApp;
