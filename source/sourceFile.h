#pragma once

enum LineType
{
    LT_Unknown,
    LT_Comment,
    LT_Include,
    LT_Address,
    LT_Label,
    LT_DataBytes,
    LT_Instruction
};

struct CompileInfo
{
    LineType type;
    int memAddr;
    int memSize;
    std::vector<u8> data;
    GraphicChunk* addr;
};

class SourceLine
{
public:
    SourceLine(const char* start, const char* end)
    {
        m_compileInfo = nullptr;
        m_gcText = nullptr;
        m_gcMemAddr = nullptr;
        m_gcDecode = nullptr;

        while (start != end)
        {
            m_chars.push_back(*start++);
        }
    }
    SourceLine()
    {
        m_compileInfo = nullptr;
        m_gcText = nullptr;
        m_gcMemAddr = nullptr;
        m_gcDecode = nullptr;
    }

    vector<char>& GetChars() { return m_chars; }
    int GetLineWidth() { return m_charXOffset.back(); }
    CompileInfo* GetCompileInfo() { return m_compileInfo; }
    void GetCharX(int column, int& xStart, int& xEnd);
    int GetColumnAtX(int x);

    GraphicChunk* GetGCText() { return m_gcText; }
    GraphicChunk* GetGCMemAddr() { return m_gcMemAddr; }
    GraphicChunk* GetGCDecode() { return m_gcDecode; }

    void Tokenize();
    void VisualizeText();
    void Compile();
    void VisualizeMemAddr();
    void VisualizeDecode();

protected:
    GraphicChunk* m_gcText;         // graphic chunk for text
    GraphicChunk* m_gcMemAddr;      // graphic chunk for memory address
    GraphicChunk* m_gcDecode;       // graphic chunk for decode

    vector<char> m_chars;
    vector<char*> m_tokens;
    vector<int> m_charXOffset;

    CompileInfo* m_compileInfo;

    void FreeTokens();
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

    void UpdateDirty(int amount) { m_dirtyCount += amount; }
    bool IsDirty() { return m_dirtyCount != 0; }

protected:
    int m_dirtyCount;

    string m_name;
    string m_path;
    vector<SourceLine *> m_lines;
    class CmdManager* m_cmdManager;
};
