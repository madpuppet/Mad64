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
        m_gcLine = nullptr;
        while (start != end)
        {
            m_chars.push_back(*start++);
        }
    }

    // create graphics chunk - break up line using syntax highlighting
    void Visualize()
    {
        // break into tokens

    }


protected:
    CompileInfo* m_compileInfo;
    GraphicChunk* m_gcLine;
    vector<char> m_chars;
};

class SourceFile
{
public:
    SourceFile(const char* path);
    bool Load();

protected:
    string m_name;
    string m_path;
    vector<SourceLine *> m_lines;
};
