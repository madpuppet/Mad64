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
    struct GraphicChunk* addr;
};

struct SourceLine
{
    CompileInfo* compileInfo;
    struct GraphicChunk* line;
    vector<char> chars;
};

struct SourceFile
{
    string path;
    vector<SourceLine *> lines;
};
