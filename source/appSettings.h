#pragma once

struct AppFile
{
    ~AppFile()
    {
    }

    void Clear()
    {
        for (auto l : lines)
            delete l;
        lines.clear();
    }

    struct Line
    {
        string token;
        vector<string> params;
        bool GetBool(int p = 0)
        {
            return (p < params.size()) ? (params[p] == "1" || params[p] == "true") : false;
        }
        int GetInt(int p = 0)
        {
            return (p < params.size()) ? atoi(params[p].c_str()) : 0;
        }
        u8 GetInt8(int p = 0)
        {
            return (u8)((p < params.size()) ? atoi(params[p].c_str()) : 0);
        }
        float GetFloat(int p = 0)
        {
            return (p < params.size()) ? (float)atof(params[p].c_str()) : 0.0f;
        }
        string GetString(int p = 0)
        {
            return (p < params.size()) ? params[p] : "";
        }
        bool IsToken(const char* val)
        {
            return SDL_strcasecmp(token.c_str(), val) == 0;
        }
        SDL_Color GetColRGB(int p = 0)
        {
            return { GetInt8(p), GetInt8(p + 1), GetInt8(p + 2), 255 };
        }
    };
    vector<Line*> lines;
    int currentLine;
    void Parse(const char* mem, size_t size);
};


class AppSettings
{
public:
    AppSettings();

    bool tabsToSpaces;    // convert tabs to spaces
    bool overwriteMode;  // overwrite or insert
    bool autoIndent;     // on return, do we indent the next line
    bool renderLineBackgrounds;   // turn this off for much faster rendering
    bool lowCPUMode;     // app will delay 10ms each cycle to free up CPU time to the system, even when focused
    bool swapJoystickPorts;     // swap A & B joyports

    string vicePath;     // path to vice
    string fontPath;     // path to font
    int fontSize;        // point size of font
    int lineHeight;      // pixel height of a line of text
    int tabWidth;        // pixel size of a tab
    int textXMargin;     // amount to add to keep text away from edges
    int textYMargin;     // amount to add to keep text away from edges
    int scrollBarWidth;  // width of vertical scroll bars

    // editor split locations
    int xPosDecode;
    int xPosText;
    int xPosContextHelp;

    SDL_Color backColor;
    SDL_Color textColor;
    SDL_Color opCodeColor;
    SDL_Color commentColor;
    SDL_Color numericColor;
    SDL_Color helpGroupColor;
    SDL_Color helpTitleColor;
    SDL_Color helpBodyColor1;
    SDL_Color helpBodyColor2;

    vector<string> loadedFilePaths;
    string activeFilePath;
    int activeLoadedFilePath;

    AppFile appFile;

    bool Load();
    bool Save();
    string GetFilePath();
};


