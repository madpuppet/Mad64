// Using SDL and standard IO
#include "common.h"

int main(int argc, char *args[])
{
    auto app = new Application();
    app->MainLoop();
    delete app;
    return 0;
}

// helper functions
void CopyToClipboard(vector<string>& buffer)
{
    string text = "";
    for (int i = 0; i < buffer.size(); i++)
    {
        if (!buffer[i].empty())
            text += buffer[i];

        if (i != buffer.size() - 1)
            text += "\n";
    }
    SDL_SetClipboardText(text.c_str());
}

void CopyFromClipboard(vector<string>& buffer)
{
    char* clip = SDL_GetClipboardText();
    char* src = clip;
    string line;
    while (*src)
    {
        if (*src == 0xa)
        {
            buffer.push_back(line);
            line = "";
            src++;
            while (*src && *src == 0xd)
                src++;
        }
        else if (*src == 0xd)
        {
            buffer.push_back(line);
            line = "";
            src++;
            while (*src && *src == 0xa)
                src++;
        }
        else
        {
            line += *src++;
        }
    }
    if (!line.empty())
        buffer.push_back(line);
    SDL_free(clip);
}



