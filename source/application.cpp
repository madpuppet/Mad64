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
        auto source = new SourceFile(ofn.lpstrFile);
        if (source->Load())
        {
            m_sourceFiles.push_back(source);

            // alert renderer of update
            m_editWindow->OnFileLoaded(source);
        }
        else
        {
            delete source;
        }
    }
}

void Application::SaveFile()
{

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
    }

    m_editWindow->OnKeyDown(e);
}

