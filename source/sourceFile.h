#pragma once

#include "graphicChunk.h"

class SourceLine
{
public:
    SourceLine();
    SourceLine(const char* start, const char* end);
    SourceLine(string line);
    ~SourceLine();

    struct RenderText
    {
        int x, y;
        string text;
        SDL_Color col;
    };

    string &GetChars() { return m_chars; }
    vector<string>& GetTokens() { return m_tokens; }
    int GetLineWidth() { LayoutRenderText(); return m_charXOffset.empty() ? 0 : m_charXOffset.back(); }
    void GetCharX(int column, int& xStart, int& xEnd);
    int GetColumnAtX(int x);
    vector<RenderText>& GetRenderText() { return m_renderText; }

    // scan the m_chars for tokens
    // this clears any previous visualization
    void Tokenize();

    // convert the tokens to visual elements if it hasn't already
    void RenderLine(int x, int y);
    void ClearAllVisuals();
    void LayoutRenderText();

    // Set Breakpoint (0 to clear, or use combination of BRK_xxxx flags)
    void SetBreakpoint(u8 breakpointType) { m_breakpoint = breakpointType; }
    u8 GetBreakpoint() { return m_breakpoint; }

protected:
    string m_chars;
    vector<string> m_tokens;
    vector<int> m_charXOffset;
    u8 m_breakpoint;
    vector<RenderText> m_renderText;
};

class SourceFile
{
public:
    SourceFile(const char* path);
    ~SourceFile();

    class CmdManager* GetCmdManager() { return m_cmdManager; }
    bool Load();
    bool Save();
    string &GetName() { return m_name; }
    string &GetPath() { return m_path; }
    void SetPath(const string& name);
    vector<SourceLine*> &GetLines() { return m_lines; }

    void UpdateDirty(int amount) { m_dirtyCount += amount; m_sourceVersion++; }
    bool IsDirty() { return m_forceDirty || m_dirtyCount != 0; }
    void SetForceDirty(bool enable) { m_forceDirty = enable; }

    class CompilerSourceInfo* GetCompileInfo();
    void SetCompileInfo(class CompilerSourceInfo* info);
    int GetSourceVersion() { return m_sourceVersion; };

    void ClearAllVisuals();

protected:
    bool SaveInternal();

    bool m_forceDirty;
    int m_dirtyCount;
    int m_sourceVersion;

    string m_name;
    string m_path;
    vector<SourceLine *> m_lines;
    class CmdManager* m_cmdManager;
    class CompilerSourceInfo* m_compileInfo;
};
