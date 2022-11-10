#pragma once

class AppSettings
{
public:
    AppSettings();

    bool tabsToSpaces;    // convert tabs to spaces
    bool overwriteMode;  // overwrite or insert
    bool autoIndent;     // on return, do we indent the next line
    bool renderLineBackgrounds;   // turn this off for much faster rendering
    bool lowCPUMode;     // app will delay 10ms each cycle to free up CPU time to the system, even when focused

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
    string openLogs;

    bool Load();
    bool Save();
    string GetFilePath();
};


