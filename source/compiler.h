#pragma once

class CompilerLabel
{
public:
    CompilerLabel(const char* name, u32 addr) : m_name(name), m_addr(addr) {}

    string m_name;
    u32 m_addr;
};

enum CompilerLineType
{
    LT_Unknown,
    LT_Comment,
    LT_Include,
    LT_Address,
    LT_Label,
    LT_DataBytes,
    LT_Instruction
};

// OPERANDS
enum AddressingMode
{
    AM_Implied,              // operand
    AM_Immediate,            // operand #value
    AM_ZeroPage,             // operand value
    AM_ZeroPageX,            // operand value,x
    AM_ZeroPageY,            // operand value,y
    AM_Absolute,             // operand value
    AM_AbsoluteX,            // operand value,x
    AM_AbsoluteY,            // operand value,y
    AM_Indirect,             // operand (value)
    AM_IndirectX,            // operand (value, x)
    AM_IndirectY,            // operand (value), y
    AM_Relative              // operand value
};
extern int gAddressingModeSize[];

class CompilerLineInfo
{
public:
    CompilerLineInfo() : type(LT_Unknown), memAddr(0), opcode(0), operand(0) 
    {
        gcMemAddr = new GraphicChunk();
        gcDecode = new GraphicChunk();
    }
    ~CompilerLineInfo()
    {
        delete gcMemAddr;
        delete gcDecode;
    }

    int lineNmbr;
    CompilerLineType type;
    u32 memAddr;
    int opcode;
    int operand;
    AddressingMode addressMode;
    string label;
    bool labelNeedsResolve;
    std::vector<u8> data;

    GraphicChunk* gcMemAddr;
    GraphicChunk* gcDecode;
};

class CompilerSourceInfo
{
public:
    bool DoesLabelExist(const char* label);

    // save as PRG
    void SavePrg(const char* path);

    // clear all visualizations
    void ClearVisuals();

    // this much match current source stamp or compile is invalid
    int m_sourceVersion;

    vector<CompilerLabel*> m_labels;
    vector<CompilerLineInfo*> m_lines;
};

struct CompilerOpcode
{
    const char* name;
    AddressingMode addressMode;
    u8 opc;
    u8 cycles;

    bool extraCycleOnPageBoundary;
    bool extraCycleOnBranch;
};

#define ERR(...)  { Error(__VA_ARGS__); return; }

class Compiler
{
public:
	Compiler();

	// compile all lines
	void Compile(class SourceFile* file);

    // compile a single line
    // pass one builds up all the source code and labels,  but leaves markers for forward referenced labels since we don't know where they are yet
    // pass two fills in label address operands
    void CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, vector<string>& tokens, int &currentMemAddr);
    void CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li);

	// return opcode index or -1 if not an opcode
	bool IsOpCode(const char* text);

    CompilerOpcode* FindOpcode(string &name, AddressingMode am);
    CompilerLabel* FindLabel(CompilerSourceInfo* si, string& name);
    bool ParseImmediateOperand(string& token, CompilerLineInfo* li);
    bool ParseOperand(string& token, AddressingMode amZeroPage, AddressingMode amAbsolute, CompilerSourceInfo* si, CompilerLineInfo* li);

    GraphicChunk* GetMemAddrGC(class SourceFile* file, int line, int sourceVersion);
    GraphicChunk* GetDecodeGC(class SourceFile* file, int line, int sourceVersion);

	// 64k of ram
	u8 m_ram[65536];

    vector<string> m_errors;
    void Error(const char* pFormat, ...);
};


