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
    LT_Variable,
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

enum LabelResolve
{
    LabelResolve_None,
    LabelResolve_Done,
    LabelResolve_Global,
    LabelResolve_Backwards,
    LabelResolve_Forwards
};

class CompilerLineInfo
{
public:
    CompilerLineInfo() : type(LT_Unknown), memAddr(0), opcode(0), operand(0), error(false), labelResolve(LabelResolve_None)
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
    LabelResolve labelResolve;
    bool error;
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

    string m_workingDir;
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

struct CompilerExpressionToken
{
    bool isLabel;
    int value;
    string label;
};

struct CompilerExpression
{
    enum Operation
    {
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_AND,
        OP_OR,
        OP_MODULO,
        OP_SHIFT_LEFT,
        OP_SHIFT_RIGHT
    };
    vector<CompilerExpressionToken*> m_tokens;
};

struct TokenFifo
{
    TokenFifo(vector<string>& tokens) : m_end(""), m_index(0)
    {
        // build tokens ignore white space tokens
        for (auto& t : tokens)
        {
            if (t[0] != ' ' && t[0] != '\t' && t[0] != ';')
                m_tokens.push_back(t);
        }
    }

    string& Pop(int ahead = 0)
    {
        if (m_index + ahead < (int)m_tokens.size())
        {
            string& result = m_tokens[m_index + ahead];
            m_index += ahead+1;
            return result;
        }
        m_index = (int)m_tokens.size();
        return m_end;
    }

    string& Peek(int ahead = 0)
    {
        if ((m_index + ahead) < (int)m_tokens.size())
            return m_tokens[m_index + ahead];
        return m_end;
    }

    bool PopLabel(string &label, bool &localLabel, LabelResolve &resolve)
    {
        if (Peek() == "!")
        {
            localLabel = true;
            Pop();
        }
        label = Pop();
        if (Peek() == "+")
        {
            resolve = LabelResolve_Forwards;
            Pop();
        }
        else if (Peek() == "-")
        {
            resolve = LabelResolve_Backwards;
            Pop();
        }
        else
        {
            resolve = LabelResolve_Global;
        }
        return !label.empty();
    }

    int Mark() { return m_index; }
    void ReturnToMark(int mark) { m_index = mark; }

    bool IsEmpty() { return m_index == m_tokens.size(); }

    int m_index;
    vector<string> m_tokens;
    string m_end;
};


#define ERR(...)  { string error = FormatString(__VA_ARGS__);  Error("Err ln %d:'%s' : %s",li->lineNmbr+1, sourceLine->GetChars().c_str(), error.c_str() ); li->error = true; return; }

class Compiler
{
public:
	Compiler();

	// compile all lines
	void Compile(class SourceFile* file);

    // compile a single line
    // pass one builds up all the source code and labels,  but leaves markers for forward referenced labels since we don't know where they are yet
    // pass two fills in label address operands
    void CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine, int &currentMemAddr);
    void CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine);

	// return opcode index or -1 if not an opcode
	bool IsOpCode(const char* text);

    CompilerOpcode* FindOpcode(string &name, AddressingMode am);
    CompilerLabel* FindLabel(CompilerSourceInfo* si, string& name, LabelResolve resolve, u32 resolveStartAddr);
    bool ParseImmediateOperand(TokenFifo& token, CompilerLineInfo* li);
    bool ParseOperand(TokenFifo& token, AddressingMode amZeroPage, AddressingMode amAbsolute, CompilerSourceInfo* si, CompilerLineInfo* li);

    GraphicChunk* GetMemAddrGC(class SourceFile* file, int line, int sourceVersion);
    GraphicChunk* GetDecodeGC(class SourceFile* file, int line, int sourceVersion);

	// 64k of ram
	u8 m_ram[65536];

    vector<string> m_errors;
    void Error(const char* pFormat, ...);
};


