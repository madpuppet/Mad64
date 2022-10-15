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

class SourceFile
{
public:
    SourceFile(const char* path);
    bool Load();
    const char* GetName() { return m_name.c_str(); }
    const char* GetPath() { return m_path.c_str(); }
    vector<SourceLine*> &GetLines() { return m_lines; }

protected:
    string m_name;
    string m_path;
    vector<SourceLine *> m_lines;
};
