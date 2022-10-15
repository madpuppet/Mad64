#include "common.h"
#include "appSettings.h"
#include <stdio.h>

bool ReadLine(char * &src, char *end, char* token, char* value)
{
    if (src >= end)
        return false;

    int tlen = 0;
    int vlen = 0;

    while (src < end && (*src == 0xa || *src == 0xd || *src == ' ' || *src == '\t'))
        src++;
    while (src < end && *src != '=' && tlen < 255)
    {
        *token++ = *src++;
        tlen++;
    }
    src++;
    while (src < end && *src != 0xa && *src != 0xd && vlen < 255)
    {
        *value++ = *src++;
        vlen++;
    }
    while (src < end && (*src == 0xa || *src == 0xd || *src==' ' || *src == '\t'))
        src++;

    *token++ = 0;
    *value++ = 0;
    return true;
}
bool ReadColor(char* src, u8 &red, u8 &green, u8 &blue)
{
    char r[256];
    char g[256];
    char b[256];
    char* out;

    while (*src && (*src == 0xa || *src == 0xd || *src == ' ' || *src == '\t'))
        src++;

    out = r;
    while (*src && *src != ',')
        *out++ = *src++;
    *out++ = 0;
    if (*src)
        src++;

    out = g;
    while (*src && *src != ',')
        *out++ = *src++;
    *out++ = 0;
    if (*src)
        src++;

    out = b;
    while (*src)
        *out++ = *src++;
    *out++ = 0;

    red = (u8)atoi(r);
    green = (u8)atoi(g);
    blue = (u8)atoi(b);
    return r[0] && g[0] && b[0];
}

AppSettings::AppSettings()
{
    lineHeight = 24;
    fontSize = 16;
    whiteSpaceWidth = fontSize;
    tabWidth = whiteSpaceWidth * 4;
    textXMargin = 8;
    textYMargin = 4;
    textColor = { 0, 255, 255, 255 };
    commentColor = { 0, 255, 64, 255 };
    opCodeColor = { 255, 200, 50, 255 };
    xPosDecode = 100;
    xPosText = 300;
    xPosContextHelp = 1600;
}

bool AppSettings::Load()
{
    char *pref = SDL_GetPrefPath("madpuppet", "mad64");
    size_t size;
    string path = pref;
    path = path + "settings.ini";

    void *data = SDL_LoadFile(path.c_str(), &size);
    if (data)
    {
        char* src = (char*)data;
        char* end = src + size;

        char token[256];
        char value[256];
        while (ReadLine(src, end, token, value))
        {
            if (token[0] && value[0])
            {
                int val = atoi(value);
                if (SDL_strcasecmp(token, "FontSize") == 0)
                {
                    fontSize = val;
                }
                else if (SDL_strcasecmp(token, "LineHeight") == 0)
                {
                    lineHeight = val;
                }
                else if (SDL_strcasecmp(token, "whiteSpaceWidth") == 0)
                {
                    whiteSpaceWidth = val;
                }
                else if (SDL_strcasecmp(token, "tabWidth") == 0)
                {
                    tabWidth = val;
                }
                else if (SDL_strcasecmp(token, "textXMargin") == 0)
                {
                    textXMargin = val;
                }
                else if (SDL_strcasecmp(token, "textYMargin") == 0)
                {
                    textYMargin = val;
                }
                else if (SDL_strcasecmp(token, "xPosDecode") == 0)
                {
                    xPosDecode = val;
                }
                else if (SDL_strcasecmp(token, "xPosText") == 0)
                {
                    xPosText = val;
                }
                else if (SDL_strcasecmp(token, "xPosContextHelp") == 0)
                {
                    xPosContextHelp = val;
                }
                else if (SDL_strcasecmp(token, "textColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    textColor = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "opCodeColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    opCodeColor = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "commentColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    commentColor = { r, g, b, 255 };
                }
            }
        }
        SDL_free(data);
        return true;
    }
    return false;
}

#include <iostream>
#include <windows.h>
void CreateDir(const char* path)
{
    LPSECURITY_ATTRIBUTES attr;
    attr = NULL;
    CreateDirectoryA(path, attr);
}

bool AppSettings::Save()
{
    char* pref = SDL_GetPrefPath("madpuppet", "mad64");

    // create all paths
    char* base = pref;
    while (base = strchr(base, '\\'))
    {
        base[0] = 0;
        CreateDir(pref);
        base[0] = '\\';
        base++;
    }

    string path = pref;
    path = path + "settings.ini";
    FILE* fh = fopen(path.c_str(), "w");
    if (fh)
    {
        fprintf(fh, "fontSize=%d\n", fontSize);
        fprintf(fh, "lineHeight=%d\n", lineHeight);
        fprintf(fh, "whiteSpaceWidth=%d\n", whiteSpaceWidth);
        fprintf(fh, "tabWidth=%d\n", tabWidth);
        fprintf(fh, "textXMargin=%d\n", textXMargin);
        fprintf(fh, "textYMargin=%d\n", textYMargin);
        fprintf(fh, "xPosDecode=%d\n", xPosDecode);
        fprintf(fh, "xPosText=%d\n", xPosText);
        fprintf(fh, "xPosContextHelp=%d\n", xPosContextHelp);
        fprintf(fh, "textColor=%d,%d,%d\n", textColor.r, textColor.g, textColor.b);
        fprintf(fh, "opCodeColor=%d,%d,%d\n", opCodeColor.r, opCodeColor.g, opCodeColor.b);
        fprintf(fh, "commentColor=%d,%d,%d\n", commentColor.r, commentColor.g, commentColor.b);
        fclose(fh);
        return true;
    }
    return false;
}




