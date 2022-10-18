#pragma once

class SourceLine
{
public:
    SourceLine(const char* start, const char* end)
    {
        m_gcText = new GraphicChunk();

        while (start != end)
        {
            m_chars.push_back(*start++);
        }
    }
    SourceLine()
    {
        delete m_gcText;
    }

    vector<char>& GetChars() { return m_chars; }
    vector<string>& GetTokens() { return m_tokens; }
    int GetLineWidth() { return m_charXOffset.back(); }
    void GetCharX(int column, int& xStart, int& xEnd);
    int GetColumnAtX(int x);

    GraphicChunk* GetGCText() { return m_gcText; }

    void Tokenize();
    void VisualizeText();

protected:
    GraphicChunk* m_gcText;         // graphic chunk for text

    vector<char> m_chars;
    vector<string> m_tokens;
    vector<int> m_charXOffset;
};

class SourceCopyBufferLine
{
public:
    vector<char>& GetChars() { return m_chars; }

protected:
    vector<char> m_chars;
};

class SourceCopyBuffer
{
public:
    void Clear();
    void Dump();
    vector<SourceCopyBufferLine*> &GetLines() { return m_lines; }

protected:
    vector<SourceCopyBufferLine*> m_lines;
};

class SourceFile
{
public:
    SourceFile(const char* path);
    ~SourceFile();

    class CmdManager* GetCmdManager() { return m_cmdManager; }
    bool Load();
    bool Save();
    const char* GetName() { return m_name.c_str(); }
    const char* GetPath() { return m_path.c_str(); }
    vector<SourceLine*> &GetLines() { return m_lines; }

    void UpdateDirty(int amount) { m_dirtyCount += amount; m_sourceVersion++; }
    bool IsDirty() { return m_forceDirty || m_dirtyCount != 0; }
    void SetForceDirty(bool enable) { m_forceDirty = enable; }

    class CompilerSourceInfo* GetCompileInfo();
    void SetCompileInfo(class CompilerSourceInfo* info);
    int GetSourceVersion() { return m_sourceVersion; };

protected:
    bool m_forceDirty;
    int m_dirtyCount;
    int m_sourceVersion;

    string m_name;
    string m_path;
    vector<SourceLine *> m_lines;
    class CmdManager* m_cmdManager;
    class CompilerSourceInfo* m_compileInfo;
};
