#pragma once
#include "thread.h"
#include "contextualHelp.h"

// compiler label only get added to labels list once they are evaluated
class CompilerLabel
{
public:
    CompilerLabel(const string &name, double value, int lineNmbr = -1, bool isLocal=false) : m_name(name), m_value(value), m_lineNmbr(lineNmbr), m_isLocal(isLocal) {}
    string m_name;
    double m_value;
    int m_lineNmbr;
    bool m_isLocal;
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
    LT_DataWords,
    LT_DataText,
    LT_GenerateBytes,
    LT_GenerateWords,
    LT_Instruction,
    LT_BasicStartup
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
    LabelResolve_Done,
    LabelResolve_Global,
    LabelResolve_Backwards,
    LabelResolve_Forwards
};

enum CompilerExpressionOpcodeType
{
    CEOT_Prefix,
    CEOT_Binary,
    CEOT_Func,
    CEOT_Ternary
};

struct CompilerExpressionOpcode
{
    const char* text;
    int params;
    int priority;
    CompilerExpressionOpcodeType type;
    double(*Evaluate)(double a, double b, double c);
};

struct CompilerExpressionToken
{
    CompilerExpressionToken(double _value) : resolve(LabelResolve_Done), value(_value) {}
    CompilerExpressionToken(string _label, LabelResolve _resolve, u32 _lineAddr) : resolve(_resolve), label(_label), value(_lineAddr) {}
    CompilerExpressionToken(const CompilerExpressionToken& o) : resolve(o.resolve), value(o.value), label(o.label) {}

    LabelResolve resolve;
    double value;
    string label;
};

struct CompilerExpression
{
    CompilerExpression() {}
    CompilerExpression(double value)
    {
        m_tokens.push_back(new CompilerExpressionToken(value));
    }
    ~CompilerExpression()
    {
        for (auto t : m_tokens)
            delete t;
    }
    CompilerExpression* Clone()
    {
        CompilerExpression* c = new CompilerExpression();
        for (auto t : m_tokens)
            c->m_tokens.push_back(new CompilerExpressionToken(*t));
        c->m_operators = m_operators;
        c->m_operatorPri = m_operatorPri;
        return c;
    }

    vector<CompilerExpressionToken*> m_tokens;
    vector<CompilerExpressionOpcode*> m_operators;
    vector<int> m_operatorPri;
};

class CompilerLineInfo
{
public:
    CompilerLineInfo() : type(LT_Unknown), memAddr(0), opcode(0), operand(0), error(false), labelResolve(LabelResolve_Done)
    {
        gcMemAddr = new GraphicChunk();
        gcDecode = new GraphicChunk();
    }
    ~CompilerLineInfo()
    {
        delete gcMemAddr;
        delete gcDecode;
        for (auto e : dataExpr)
            delete e;
    }

    int lineNmbr;
    bool error;
    CompilerLineType type;
    u32 memAddr;
    int opcode;

    AddressingMode addressMode;
    int operandValue;

    double operand;
    string label;
    LabelResolve labelResolve;

    vector<CompilerExpression*> dataExpr;
    bool dataEvaluated;
    vector<double> cmdParams;

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

    string& Pop()
    {
        if (m_index < (int)m_tokens.size())
        {
            string& result = m_tokens[m_index];
            m_index++;
            return result;
        }
        m_index = (int)m_tokens.size();
        return m_end;
    }

    string& Peek()
    {
        if (m_index < (int)m_tokens.size())
            return m_tokens[m_index];
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

#define ERR(...)  { string error = FormatString(__VA_ARGS__); Error(error, li->lineNmbr); li->error = true; return; }
#define ERR_RF(...)  { string error = FormatString(__VA_ARGS__); Error(error, li->lineNmbr); li->error = true; return false; }
#define ERR_NOLINE(...)  { string error = FormatString(__VA_ARGS__); Error(error, 0); return; }
#define ERR_NORET(...)  { string error = FormatString(__VA_ARGS__); Error(error, li->lineNmbr); li->error = true; }

#if 1
class TokenisedLine
{
public:
    struct Token
    {
        const char* memory;
        int length;
    };

    int m_firstToken;
    int m_tokenCount;
};

class TokenisedFile
{
public:
    TokenisedFile(SourceFile* sf);
    ~TokenisedFile();

    TokenisedLine::Token* m_tokens;
    TokenisedLine* m_lines;

    int m_lineCount;
    char* m_memory;
};
#else
class TokenisedLine
{
public:
    vector<string> m_tokens;
};

class TokenisedFile
{
public:
    TokenisedFile(SourceFile* sf);
    ~TokenisedFile();

    vector<TokenisedLine*> m_lines;
};
#endif

class Compiler : public Thread
{
public:
	Compiler();

	// compile all lines
	void Compile(class SourceFile* file);

    // pass one builds up all the source code and labels,  but does not try to evaluate all expressions
    void CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine, u32 &currentMemAddr);

    // pass two attempts to resolve all labels/expressions but may take multiple passes if there are deep dependancies
    bool CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine);

	// return opcode index or -1 if not an opcode
	bool IsOpCode(const char* text);

    CompilerOpcode* FindOpcode(string &name, AddressingMode am);
    CompilerLabel* FindLabel(CompilerSourceInfo* si, string& name, LabelResolve resolve, u32 resolveStartAddr);

    GraphicChunk* GetMemAddrGC(class SourceFile* file, int line, int sourceVersion);
    GraphicChunk* GetDecodeGC(class SourceFile* file, int line, int sourceVersion);

	// 64k of ram
	u8 m_ram[65536];

    // expression opcodes that follow a value - ie.  a * b,  a + b
    CompilerExpressionOpcode *FindExprOpcode(string& token);

    // expression opcodes that precide a value - ie. !a,  sin(a)
    CompilerExpressionOpcode* FindPrefixExprOpcode(string& token);

    void PopExpressionValue(TokenFifo& fifo, CompilerLineInfo* li, CompilerExpression* expr, int priority);
    bool EvaluateExpression(CompilerSourceInfo* si, CompilerLineInfo* line, CompilerExpression* expr, double& value);
    bool ResolveExpressionToken(CompilerSourceInfo* si, CompilerExpressionToken* token);

    void CmdImport_Parse(TokenFifo& fifo, CompilerSourceInfo* si, CompilerLineInfo* li, u32& currentMemAddr);

    void AddLabelsContaining(CompilerSourceInfo* cs, vector<CompilerLabel*>& labels, const string& token);
    void AddCommandsContaining(vector<CommandHelp*>& commands, const string& token);


    struct ErrorItem
    {
        string text;
        int lineNmbr;
    };
    vector<ErrorItem*> m_errors;
    void Error(const string &text, int lineNmbr);
    void FlushErrors();

    void LogContextualHelp(SourceFile* sf, int line);

protected:
    Semaphore m_compileRequested;
    Semaphore m_compileCompleted;

    // compiler loop
    virtual int Go();

    TokenisedFile* m_fileCopy;
};


