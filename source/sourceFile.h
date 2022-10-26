#pragma once

class SourceLine
{
public:
    SourceLine();
    SourceLine(const char* start, const char* end);
    ~SourceLine();

    string &GetChars() { return m_chars; }
    vector<string>& GetTokens() { return m_tokens; }
    int GetLineWidth() { return m_charXOffset.back(); }
    void GetCharX(int column, int& xStart, int& xEnd);
    int GetColumnAtX(int x);

    GraphicChunk* GetGCText() { return m_gcText; }

    void Tokenize();
    void VisualizeText();

protected:
    GraphicChunk* m_gcText;         // graphic chunk for text

    string m_chars;
    vector<string> m_tokens;
    vector<int> m_charXOffset;
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
    vector<SourceLine*> &GetLines() { return m_lines; }

    void UpdateDirty(int amount) { m_dirtyCount += amount; m_sourceVersion++; }
    bool IsDirty() { return m_forceDirty || m_dirtyCount != 0; }
    void SetForceDirty(bool enable) { m_forceDirty = enable; }

    class CompilerSourceInfo* GetCompileInfo();
    void SetCompileInfo(class CompilerSourceInfo* info);
    int GetSourceVersion() { return m_sourceVersion; };

    void Visualize();

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
