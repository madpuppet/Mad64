#include "common.h"
#include "application.h"

#include <windows.h>
#include <commdlg.h>

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

Application *gApp;

GraphicChunk* FontDemo(TTF_Font* font, const char* msg)
{
    GraphicChunk* gc = new GraphicChunk();
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 50; y++)
        {
            u8 r = (rand() & 127) + 128;
            u8 g = (rand() & 127) + 128;
            u8 b = (rand() & 127) + 128;
            SDL_Color col = { r,g,b,255 };
            gc->Add(GraphicElement::CreateFromText(font, msg, col, x * 200, y * 20));
        }
    }
    return gc;
}

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
        TTF_Init();
        m_font = TTF_OpenFont("data/C64_Pro-STYLE.ttf", 16);
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetVSync(m_renderer, 1);

        //Get window surface
        SDL_Surface *screenSurface = SDL_GetWindowSurface(m_window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x60));
        SDL_UpdateWindowSurface(m_window);

        m_ui = new UIManager();

        m_tempGC = FontDemo(m_font, "HELLO WORLD");

        SDL_Event e;
        float time = 0.0f;
        while (!m_quit)
        {
            time += 0.01f;

            SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
            SDL_RenderFillRect(m_renderer, NULL);

            m_tempGC->DrawAt((int)(sinf(time) * 500.0f), 100);
            SDL_RenderPresent(m_renderer);

            if (SDL_PollEvent(&e))
                HandleEvent(&e);
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

        m_time += 0.01f;
        Draw();
    }
    return 0;
}

void Application::Draw()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
    SDL_RenderFillRect(m_renderer, NULL);
    m_tempGC->DrawAt((int)(sinf(m_time) * 500.0f), 0);
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
        break;
    case SDL_MOUSEBUTTONUP:
        break;
    case SDL_MOUSEMOTION:
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
    char szFile[] = "pork";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Asm\0*.asm\0Basic\0*.bas";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn))
    {
        auto source = new SourceFile(ofn.lpstrFile);
        if (source->Load())
        {
            m_sourceFiles.push_back(source);
        }
        else
        {
            delete source;
        }
    }
}

void Application::CreateNewFile()
{
    OPENFILENAMEA ofn;
    char szFile[] = "pork";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Asm\0*.asm\0Basic\0*.bas";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = 0;
    if (GetOpenFileNameA(&ofn))
    {
        m_sourceFiles.push_back(new SourceFile(ofn.lpstrFile));
    }
}

void Application::OnKeyDown(SDL_Event* e)
{
    switch (e->key.keysym.sym)
    {
    case SDLK_l:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            LoadFile();
        }
        break;
    case SDLK_n:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            // CREATE A NEW FILE
            CreateNewFile();
        }
        break;
    case SDLK_s:
        if (e->key.keysym.mod & KMOD_CTRL)
        {
            // SAVE A FILE
        }
        break;
    }
}

