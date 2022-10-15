#pragma once

#include "sourceFile.h"
#include "graphicChunk.h"
#include "editWindow.h"
#include "compiler.h"

struct AppSettings
{
    int fontSize;        // point size of font
    int lineHeight;      // pixel height of a line of text
    int whiteSpaceWidth; // pixel size of a space character
    int tabWidth;        // pixel size of a tab
    int textXMargin;     // amount to add to keep text away from edges
    int textYMargin;     // amount to add to keep text away from edges

    // editor split locations
    int xPosDecode;
    int xPosText;
    int xPosContextHelp;

    SDL_Color textColor;
    SDL_Color opCodeColor;
    SDL_Color commentColor;
};

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
