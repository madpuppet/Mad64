#include "common.h"
#include "appSettings.h"
#include <stdio.h>

bool ReadLine(char * &src, char *end, char* token, char* value)
{
    if (src >= end)
        return false;

    int tlen = 0;
    int vlen = 0;

    // skip white space
    while (src < end && (*src == 0xa || *src == 0xd || *src == ' ' || *src == '\t'))
        src++;

    if (*src == ';')
    {
        // skip comment
        while (src < end && *src != 0xa && *src != 0xd)
            src++;
        return (src != end);
    }

    // scan command token to the '=' sign
    while (src < end && *src != '=' && tlen < 255)
    {
        *token++ = *src++;
        tlen++;
    }
    src++;

    // scan in the value token
    while (src < end && *src != 0xa && *src != 0xd && *src != ';' && vlen < 255)
    {
        *value++ = *src++;
        vlen++;
    }
    while (src < end && (*src == 0xa || *src == 0xd || *src==' ' || *src == '\t'))
        src++;

    // trim off white space
    while (*(token - 1) == ' ' || *(token - 1) == '\t')
        token--;

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
    renderLineBackgrounds = true;
    lowCPUMode = false;
    vicePath = "F:\\Emulators\\C64\\Vice3.6\\bin\\x64sc.exe";
    openLogs = "CHLMRE";
    lineHeight = 24;
    fontPath = "font.ttf";
    fontSize = 16;
    tabWidth = 4;
    textXMargin = 8;
    textYMargin = 4;
    backColor = { 0, 0, 64, 255 };
    textColor = { 0, 255, 255, 255 };
    commentColor = { 0, 255, 64, 255 };
    numericColor = { 200, 255, 50, 255 };
    opCodeColor = { 255, 200, 50, 255 };
    helpGroupColor = { 255, 200, 50, 255 };
    helpTitleColor = { 160, 160, 50, 255 };
    helpBodyColor1 = { 128, 64, 255, 255 };
    helpBodyColor2 = { 128, 64, 128, 255 };
    xPosDecode = 57;
    xPosText = 185;
    xPosContextHelp = 1600;
    scrollBarWidth = 20;
    loadedFilePaths.push_back("readme.txt");
    activeLoadedFilePath = 0;
}

bool AppSettings::Load()
{
    char *pref = SDL_GetPrefPath("madpuppet", "mad64");
    size_t size;
    string path = pref;
    path = path + "settings.ini";
    activeLoadedFilePath = 0;

    Log("Load Settings file: %s", path.c_str());
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
                else if (SDL_strcasecmp(token, "renderLineBackgrounds") == 0)
                {
                    renderLineBackgrounds = val ? true : false;
                }
                else if (SDL_strcasecmp(token, "lowCPUMode") == 0)
                {
                    lowCPUMode = val ? true : false;
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
                    tabWidth = SDL_clamp(val, 1, 32);
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
                else if (SDL_strcasecmp(token, "backColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    backColor = { r, g, b, 255 };
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
                else if (SDL_strcasecmp(token, "helpGroupColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    helpGroupColor = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "helpTitleColor") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    helpTitleColor = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "helpBodyColor1") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    helpBodyColor1 = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "helpBodyColor2") == 0)
                {
                    u8 r, g, b;
                    ReadColor(value, r, g, b);
                    helpBodyColor2 = { r, g, b, 255 };
                }
                else if (SDL_strcasecmp(token, "vicePath") == 0)
                {
                    ReadString(value, vicePath);
                }
                else if (SDL_strcasecmp(token, "openLogs") == 0)
                {
                    ReadString(value, openLogs);
                }
                else if (SDL_strcasecmp(token, "loadedFilePaths") == 0)
                {
                    loadedFilePaths.clear();
                    ReadStringArray(value, loadedFilePaths);
                }
                else if (SDL_strcasecmp(token, "activeFilePath") == 0)
                {
                    activeFilePath = value;
                }
                else if (SDL_strcasecmp(token, "activeLoadedFilePath") == 0)
                {
                    activeLoadedFilePath = val;
                }
            }
        }
        SDL_free(data);
        return true;
    }
    return false;
}

#if defined(_WIN32)
#include <iostream>
#include <windows.h>
void CreateDir(const char* path)
{
    LPSECURITY_ATTRIBUTES attr;
    attr = NULL;
    CreateDirectoryA(path, attr);
}
#endif

string AppSettings::GetFilePath()
{
    string path = SDL_GetPrefPath("madpuppet", "mad64");
    path = path + "settings.ini";
    return path;
}

bool AppSettings::Save()
{
    char* pref = SDL_GetPrefPath("madpuppet", "mad64");

#if defined(_WIN32)
    // create all paths
    char* base = pref;
    while (base = strchr(base, '\\'))
    {
        base[0] = 0;
        CreateDir(pref);
        base[0] = '\\';
        base++;
    }
#endif

    string path = pref;
    path = path + "settings.ini";
    FILE* fh = fopen(path.c_str(), "w");
    if (fh)
    {
        fprintf(fh, "; spaces will be inserted when you hit TAB\n");
        fprintf(fh, "tabsToSpaces=%d\n\n", tabsToSpaces ? 1 : 0);
        fprintf(fh, "; characters are overwritten instead of inserted\n");
        fprintf(fh, "overwriteMode=%d\n\n", overwriteMode ? 1 : 0);
        fprintf(fh, "; on new line indent to the same level as the previous line\n");
        fprintf(fh, "autoIndent=%d\n\n", autoIndent ? 1 : 0);
        fprintf(fh, "; render alternating coloured lines behind the text\n");
        fprintf(fh, "renderLineBackgrounds=%d\n\n", renderLineBackgrounds ? 1 : 0);
        fprintf(fh, "; mad64 will delay 10ms each loop to give time back to the OS\n");
        fprintf(fh, "lowCPUMode=%d\n\n", lowCPUMode ? 1 : 0);
        fprintf(fh, "; path to font 'otf' or 'ttf' file \n");
        fprintf(fh, "fontPath=%s\n\n", fontPath.c_str());
        fprintf(fh, "; font point size to render with\n");
        fprintf(fh, "fontSize=%d\n\n", fontSize);
        fprintf(fh, "; nmbr of pixels each editor line should take\n");
        fprintf(fh, "lineHeight=%d\n\n", lineHeight);
        fprintf(fh, "; nmbr of whitespace characters between font columns\n");
        fprintf(fh, "tabWidth=%d\n\n", tabWidth);
        fprintf(fh, "; nmbr of pixels to leave blank from the left border\n");
        fprintf(fh, "textXMargin=%d\n\n", textXMargin);
        fprintf(fh, "; nmbr of pixels to leave blank at top of each line\n");
        fprintf(fh, "textYMargin=%d\n\n", textYMargin);
        fprintf(fh, "; width in pixels of vertical scroll bars\n");
        fprintf(fh, "scrollBarWidth=%d\n\n", scrollBarWidth);
        fprintf(fh, "; pixels from left border of decode split line\n");
        fprintf(fh, "xPosDecode=%d\n\n", xPosDecode);
        fprintf(fh, "; pixels from left border of text window split line\n");
        fprintf(fh, "xPosText=%d\n\n", xPosText);
        fprintf(fh, "; pixels from left border of the log window split line\n");
        fprintf(fh, "xPosContextHelp=%d\n\n", xPosContextHelp);
        fprintf(fh, "; rgb color for background\n");
        fprintf(fh, "backColor=%d,%d,%d\n\n", backColor.r, backColor.g, backColor.b);
        fprintf(fh, "; rgb color for general text \n");
        fprintf(fh, "textColor=%d,%d,%d\n\n", textColor.r, textColor.g, textColor.b);
        fprintf(fh, "; rgb color for assembly opcodes\n");
        fprintf(fh, "opCodeColor=%d,%d,%d\n\n", opCodeColor.r, opCodeColor.g, opCodeColor.b);
        fprintf(fh, "; rgb color for comments\n");
        fprintf(fh, "commentColor=%d,%d,%d\n\n", commentColor.r, commentColor.g, commentColor.b);
        fprintf(fh, "; rgb color for numbers (hex/dec/octal)\n");
        fprintf(fh, "numericColor=%d,%d,%d\n\n", numericColor.r, numericColor.g, numericColor.b);
        fprintf(fh, "; rgb color for help group title text in the log window\n");
        fprintf(fh, "helpGroupColor=%d,%d,%d\n\n", helpGroupColor.r, helpGroupColor.g, helpGroupColor.b);
        fprintf(fh, "; rgb color for help title icon text in the log window\n");
        fprintf(fh, "helpTitleColor=%d,%d,%d\n\n", helpTitleColor.r, helpTitleColor.g, helpTitleColor.b);
        fprintf(fh, "; rgb colors for alternating text lines in the log window\n");
        fprintf(fh, "helpBodyColor1=%d,%d,%d\n\n", helpBodyColor1.r, helpBodyColor1.g, helpBodyColor1.b);
        fprintf(fh, "; second alternating text line\n");
        fprintf(fh, "helpBodyColor2=%d,%d,%d\n\n", helpBodyColor2.r, helpBodyColor2.g, helpBodyColor2.b);
        if (!vicePath.empty())
        {
            fprintf(fh, "; path to vice emulator for launching with f5  \n");
            fprintf(fh, "vicePath=%s\n\n", vicePath.c_str());
        }

        openLogs = gApp->GetLogWindow()->GetOpenLogs();
        fprintf(fh, "; each letter indicates a log group that is open  \n");
        fprintf(fh, "openLogs=%s\n\n", openLogs.c_str());

        if (!loadedFilePaths.empty())
        {
            fprintf(fh, "; list of files to load on startup\n");
            fprintf(fh, "loadedFilePaths=%s", loadedFilePaths[0].c_str());
            for (int i = 1; i < loadedFilePaths.size(); i++)
                fprintf(fh, ",%s", loadedFilePaths[i].c_str());
            fprintf(fh, "\n\n");

            auto sf = gApp->GetEditWindow()->GetActiveFile();
            for (int i = 0; i < loadedFilePaths.size(); i++)
            {
                if (StrEqual(loadedFilePaths[i], sf->GetPath()))
                {
                    fprintf(fh, "; which of loadedFilePaths is active\n");
                    fprintf(fh, "activeLoadedFilePath=%d\n\n", i);
                    break;
                }
            }

        }

        if (!activeFilePath.empty())
        {
            fprintf(fh, "; where to look when opening a file requestor for new or existing files\n");
            fprintf(fh, "activeFilePath=%s\n\n", activeFilePath.c_str());
        }

        fclose(fh);
        return true;
    }
    return false;
}




