#pragma once
#include "thread.h"
#include "contextualHelp.h"

struct Cpu6502State
{
    u16 regPC;
    u8 regA;
    u8 regX;
    u8 regY;
    u8 regSR;
    u8 regSP;
};

// compiler label only get added to labels list once they are evaluated
class CompilerLabel
{
public:
    CompilerLabel(const string &name, double value, int lineNmbr = -1, bool isLocal=false) : m_name(name), m_help(0), m_value(value), m_lineNmbr(lineNmbr), m_isLocal(isLocal) {}
    CompilerLabel(const string& name, const char *help[], double value) : m_name(name), m_help(help), m_value(value), m_lineNmbr(-1), m_isLocal(false) {}
    string m_name;
    const char** m_help;
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

    // get visuals on demand
    GraphicChunk* GetMemAddrGC(int line);
    GraphicChunk* GetDecodeGC(int line);

    // this much match current source stamp or compile is invalid
    int m_sourceVersion;

    // find line that matches an address
    int FindLineByAddress(u32 memAddress);

    string m_workingDir;
    vector<CompilerLabel*> m_labels;
    vector<CompilerLineInfo*> m_lines;

    // track how long it took to compile
    double m_compileTimeMS;

    // memory display mode
    bool m_displayLineNumbers;
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

#if 0
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

    SourceFile* m_sf;
    int m_sourceVersion;
    TokenisedLine::Token* m_tokens;

    TokenisedLine* m_lines;
    int m_lineCount;

    char* m_memory;
    double m_compileTimeMS;

    // where to find includes/imports
    string m_workingDir;
};
#else
class TokenisedLine
{
public:
    vector<string>& GetTokens() { return m_tokens; }

    vector<string> m_tokens;
};

class TokenisedFile
{
public:
    TokenisedFile(SourceFile* sf);
    ~TokenisedFile();

    SourceFile* m_sf;
    int m_sourceVersion;

    // copy of all the sourceFile data
    vector<TokenisedLine*> m_lines;

    // where to find includes/imports
    string m_workingDir;
};
#endif

class Compiler : public Thread
{
public:
	Compiler();
    ~Compiler();

    // push new compile through when old one is finished
    void Update();

	// compile all lines
	void Compile(class SourceFile* file);

    // do compile of tokenised file copy - this is done on a thread
    void DoCompile();

    // pass one builds up all the source code and labels,  but does not try to evaluate all expressions
    void CompileLinePass1(CompilerLineInfo* li, TokenisedLine* tokenLine, u32& currentMemAddr);

    // pass two attempts to resolve all labels/expressions but may take multiple passes if there are deep dependancies
    bool CompileLinePass2(CompilerLineInfo* li, TokenisedLine* tokenLine);

	// return opcode index or -1 if not an opcode
	bool IsOpCode(const char* text);

    CompilerOpcode* FindOpcode(string &name, AddressingMode am);
    CompilerLabel* FindLabel(CompilerSourceInfo* csi, string& name, LabelResolve resolve, u32 resolveStartAddr);

    // expression opcodes that follow a value - ie.  a * b,  a + b
    CompilerExpressionOpcode *FindExprOpcode(string& token);

    // expression opcodes that precide a value - ie. !a,  sin(a)
    CompilerExpressionOpcode* FindPrefixExprOpcode(string& token);

    void PopExpressionValue(TokenFifo& fifo, CompilerLineInfo* li, CompilerExpression* expr, int priority);
    bool EvaluateExpression(CompilerLineInfo* line, CompilerExpression* expr, double& value);
    bool ResolveExpressionToken(CompilerExpressionToken* token);

    void CmdImport_Parse(TokenFifo& fifo, CompilerSourceInfo* si, CompilerLineInfo* li, u32& currentMemAddr);

    void AddLabelsContaining(CompilerSourceInfo* cs, vector<CompilerLabel*>& labels, const string& token);
    void AddLabelsMatchingValue(CompilerSourceInfo* cs, vector<CompilerLabel*>& labels, double value);
    void AddCommandsContaining(vector<CommandHelp*>& commands, const string& token);
    CommandHelp *FindMatchingCommand(const string& token);
    CompilerLabel* FindMatchingLabel(CompilerSourceInfo* si, const string& token);

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
    // start compiling m_nextFile now
    void StartThreadedCompile();

    // secure access to 'compilationActive' and 'm_nextFile'
    Mutex m_compilationLock;

    // true if compilation is current active and m_activeFile is being processed
    bool m_compilationActive;

    // main thread signals when we have a file ready to compile
    Semaphore m_compileRequested;

    // file queued for compile - if NULL, no file is queued
    TokenisedFile* m_nextFile;

    // this file is being compiled on thread
    TokenisedFile* m_activeFile;

    // this file being compiled - can be collected if compilation is not active
    CompilerSourceInfo* m_compiledFile;

    // compiler loop
    virtual int Go();
};


