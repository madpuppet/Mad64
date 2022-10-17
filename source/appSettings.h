#pragma once

class AppSettings
{
public:
    AppSettings();

    int fontSize;        // point size of font
    int lineHeight;      // pixel height of a line of text
    int whiteSpaceWidth; // pixel size of a space character
    int tabWidth;        // pixel size of a tab
    int textXMargin;     // amount to add to keep text away from edges
    int textYMargin;     // amount to add to keep text away from edges
    int scrollBarWidth;  // width of vertical scroll bars

    // editor split locations
    int xPosDecode;
    int xPosText;
    int xPosContextHelp;

    SDL_Color textColor;
    SDL_Color opCodeColor;
    SDL_Color commentColor;

    vector<string> loadedFilePaths;
    string activeFilePath;

    bool Load();
    bool Save();
};


