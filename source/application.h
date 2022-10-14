#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"

class Application
{
public:
    Application();
    ~Application();

    int MainLoop();

protected:
    vector<SourceFile*> m_sourceFiles;

    void HandleEvent(SDL_Event* e);
    void Draw();

    SDL_Window* m_window;
    TTF_Font* m_font;
    SDL_Renderer* m_renderer;
    bool m_quit;
    bool m_repaint;

    // general animation
    float m_time;

    // temp
    GraphicChunk* m_tempGC;
};
