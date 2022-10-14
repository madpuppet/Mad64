#include "common.h"
#include "application.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

GraphicChunk* FontDemo(SDL_Renderer* renderer, TTF_Font* font, const char* msg)
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
            SDL_Point pos = { x * 200, y * 20 };
            gc->elements.push_back(GraphicElement::CreateFromText(renderer, font, msg, col, pos));
        }
    }
    return gc;
}


Application::Application()
{
    //Create window
    m_window = SDL_CreateWindow("MAD64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (m_window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        TTF_Init();
        m_font = TTF_OpenFont("C64_Pro-STYLE.ttf", 16);
        SDL_Renderer* renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetVSync(renderer, 1);

        //Get window surface
        SDL_Surface *screenSurface = SDL_GetWindowSurface(m_window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x60));
        SDL_UpdateWindowSurface(m_window);

        m_tempGC = FontDemo(renderer, m_font, "HELLO WORLD");

        SDL_Event e;
        SDL_Point pos = { 0,0 };
        float time = 0.0f;
        while (!m_quit)
        {
            time += 0.01f;
            SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
            SDL_RenderFillRect(renderer, NULL);
            pos.x = (int)(sinf(time) * 500.0f);
            m_tempGC->DrawAt(renderer, pos);
            SDL_RenderPresent(renderer);

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
    SDL_Point pos = { 0,0 };
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 100, 255);
    SDL_RenderFillRect(m_renderer, NULL);
    pos.x = (int)(sinf(m_time) * 500.0f);
    m_tempGC->DrawAt(m_renderer, pos);
    SDL_RenderPresent(m_renderer);
}

void Application::HandleEvent(SDL_Event *e)
{
    switch (e->type)
    {
    case SDL_QUIT:
        m_quit = true;
        break;
    }
}
