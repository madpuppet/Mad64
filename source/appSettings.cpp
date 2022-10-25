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
string MakeString(char* src, char* end)
{
    int len = (int)(end - src);
    char* temp = (char*)malloc(len + 1);
    memcpy(temp, src, len);
    temp[len] = 0;
    return string(temp);
}
void ReadString(char* src, string& str)
{
    str = string(src, strlen(src));
}
void ReadStringArray(char* src, vector<string>& arr)
{
    char* next;
    while (next = strchr(src, ','))
    {
        arr.push_back(MakeString(src, next));
        src = next + 1;
    }
    arr.push_back(string(src));
}
bool ReadColor(char* src, u8& red, u8& green, u8& blue)
{
    vector<string> cols;
    ReadStringArray(src, cols);
    red = cols.size() > 0 ? atoi(cols[0].c_str()) : 255;
    green = cols.size() > 1 ? atoi(cols[1].c_str()) : 255;
    blue = cols.size() > 2 ? atoi(cols[2].c_str()) : 255;
    return cols.size() == 2;
}

AppSettings::AppSettings()
{
    tabsToSpaces = true;
    overwriteMode = false;
    autoIndent = true;
    vicePath = "F:\\Emulators\\C64\\Vice3.6\\bin\\x64sc.exe";
    lineHeight = 24;
    fontPath = "data/CodeNewRoman.otf";
    fontSize = 16;
    tabWidth = 4;
    textXMargin = 8;
    textYMargin = 4;
    textColor = { 0, 255, 255, 255 };
    commentColor = { 0, 255, 64, 255 };
    numericColor = { 200, 255, 50, 255 };
    opCodeColor = { 255, 200, 50, 255 };
    xPosDecode = 57;
    xPosText = 185;
    xPosContextHelp = 1600;
    scrollBarWidth = 20;
    loadedFilePaths.push_back("readme.txt");
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
                if (SDL_strcasecmp(token, "tabsToSpaces") == 0)
                {
                    tabsToSpaces = val ? true : false;
                }
                else if (SDL_strcasecmp(token, "overwriteMode") == 0)
                {
                    overwriteMode = val ? true : false;
                }
                else if (SDL_strcasecmp(token, "autoIndent") == 0)
                {
                    autoIndent = val ? true : false;
                }
                else if (SDL_strcasecmp(token, "FontSize") == 0)
                {
                    fontSize = val;
                }
                else if (SDL_strcasecmp(token, "FontPath") == 0)
                {
                    fontPath = value;
                }
                else if (SDL_strcasecmp(token, "LineHeight") == 0)
                {
                    lineHeight = val;
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
                else if (SDL_strcasecmp(token, "scrollBarWidth") == 0)
                {
                    scrollBarWidth = val;
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
                else if (SDL_strcasecmp(token, "numericColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    numericColor = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "vicePath") == 0)
                {
                    ReadString(value, vicePath);
                }
                else if (SDL_strcasecmp(token, "loadedFilePaths") == 0)
                {
                    ReadStringArray(value, loadedFilePaths);
                }
                else if (SDL_strcasecmp(token, "activeFilePath") == 0)
                {
                    activeFilePath = value;
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

string AppSettings::GetFilePath()
{
    string path = SDL_GetPrefPath("madpuppet", "mad64");
    path = path + "settings.ini";
    return path;
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
        fprintf(fh, "tabsToSpaces=%d\n", tabsToSpaces ? 1 : 0);
        fprintf(fh, "overwriteMode=%d\n", overwriteMode ? 1 : 0);
        fprintf(fh, "autoIndent=%d\n", autoIndent ? 1 : 0);
        fprintf(fh, "fontPath=%s\n", fontPath.c_str());
        fprintf(fh, "fontSize=%d\n", fontSize);
        fprintf(fh, "lineHeight=%d\n", lineHeight);
        fprintf(fh, "tabWidth=%d\n", tabWidth);
        fprintf(fh, "textXMargin=%d\n", textXMargin);
        fprintf(fh, "textYMargin=%d\n", textYMargin);
        fprintf(fh, "scrollBarWidth=%d\n", scrollBarWidth);
        fprintf(fh, "xPosDecode=%d\n", xPosDecode);
        fprintf(fh, "xPosText=%d\n", xPosText);
        fprintf(fh, "xPosContextHelp=%d\n", xPosContextHelp);
        fprintf(fh, "textColor=%d,%d,%d\n", textColor.r, textColor.g, textColor.b);
        fprintf(fh, "opCodeColor=%d,%d,%d\n", opCodeColor.r, opCodeColor.g, opCodeColor.b);
        fprintf(fh, "commentColor=%d,%d,%d\n", commentColor.r, commentColor.g, commentColor.b);
        fprintf(fh, "numericColor=%d,%d,%d\n", numericColor.r, numericColor.g, numericColor.b);
        if (!vicePath.empty())
        {
            fprintf(fh, "vicePath=%s\n", vicePath.c_str());
        }

        if (!loadedFilePaths.empty())
        {
            fprintf(fh, "loadedFilePaths=%s", loadedFilePaths[0].c_str());
            for (int i=1; i<loadedFilePaths.size(); i++)
                fprintf(fh, ",%s", loadedFilePaths[i].c_str());
            fprintf(fh, "\n");
        }
        if (!activeFilePath.empty())
            fprintf(fh, "activeFilePath=%s\n", activeFilePath.c_str());

        fclose(fh);
        return true;
    }
    return false;
}




