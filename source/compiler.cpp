#include "common.h"
#include "compiler.h"
#include "graphicChunk.h"

CompilerLabel s_systemLabels[] =
{
    CompilerLabel("VIC_SPRITE0X", 0xd000),  CompilerLabel("VIC_SPRITE0Y", 0xd001),  CompilerLabel("VIC_SPRITE1X", 0xd002),  CompilerLabel("VIC_SPRITE1Y", 0xd003),
    CompilerLabel("VIC_SPRITE2X", 0xd004),  CompilerLabel("VIC_SPRITE2Y", 0xd005),  CompilerLabel("VIC_SPRITE3X", 0xd006),  CompilerLabel("VIC_SPRITE3Y", 0xd007),
    CompilerLabel("VIC_SPRITE4X", 0xd008),  CompilerLabel("VIC_SPRITE4Y", 0xd009),  CompilerLabel("VIC_SPRITE5X", 0xd00a),  CompilerLabel("VIC_SPRITE5Y", 0xd00b),
    CompilerLabel("VIC_SPRITE6X", 0xd00c),  CompilerLabel("VIC_SPRITE6Y", 0xd00d),  CompilerLabel("VIC_SPRITE7X", 0xd00e),  CompilerLabel("VIC_SPRITE7Y", 0xd00f),
    CompilerLabel("VIC_SPRITEMSB", 0xd010)
};

int gAddressingModeSize[] =
{
    1,  //    AM_Implied,              // operand
    2,  //    AM_Immediate,            // operand #value
    2,  //    AM_ZeroPage,             // operand value
    2,  //    AM_ZeroPageX,            // operand value,x
    3,  //    AM_ZeroPageY,            // operand value,y
    3,  //    AM_Absolute,             // operand value
    3,  //    AM_AbsoluteX,            // operand value,x
    3,  //    AM_AbsoluteY,            // operand value,y
    3,  //    AM_Indirect,             // operand (value)
    2,  //    AM_IndirectX,            // operand (value, x)
    2,  //    AM_IndirectY,            // operand (value), y
    2   //    AM_Relative              // operand value
};

CompilerOpcode s_opcodesRaw[] =
{
    {"ADC", AM_Immediate, 0x69, 2},
    {"ADC", AM_ZeroPage,  0x65, 3},
    {"ADC", AM_ZeroPageX, 0x75, 4},
    {"ADC", AM_Absolute,  0x6D, 4},
    {"ADC", AM_AbsoluteX, 0x7D, 4, true},
    {"ADC", AM_AbsoluteY, 0x79, 4, true},
    {"ADC", AM_IndirectX, 0x61, 6},
    {"ADC", AM_IndirectY, 0x71, 5, true},

    {"AND", AM_Immediate, 0x29, 2},
    {"AND", AM_ZeroPage,  0x25, 3},
    {"AND", AM_ZeroPageX, 0x35, 4},
    {"AND", AM_Absolute,  0x2D, 4},
    {"AND", AM_AbsoluteX, 0x3D, 4, true},
    {"AND", AM_AbsoluteY, 0x39, 4, true},
    {"AND", AM_IndirectX, 0x21, 6},
    {"AND", AM_IndirectY, 0x31, 5, true},

    {"ASL", AM_Implied,   0x0A, 2},
    {"ASL", AM_ZeroPage,  0x06, 5},
    {"ASL", AM_ZeroPageX, 0x16, 6},
    {"ASL", AM_Absolute,  0x0E, 6},
    {"ASL", AM_AbsoluteX, 0x1E, 7},

    {"BCC", AM_Relative,  0x90, 2, false, true},
    {"BCS", AM_Relative,  0xB0, 2, false, true},
    {"BEQ", AM_Relative,  0xF0, 2, false, true},

    {"BIT", AM_ZeroPage,  0x24, 3},
    {"BIT", AM_Absolute,  0x2C, 4},

    {"BMI", AM_Relative,  0x30, 2, false, true},
    {"BNE", AM_Relative,  0xD0, 2, false, true},
    {"BPL", AM_Relative,  0x10, 2, false, true},

    {"BRK", AM_Implied,   0x30, 7},

    {"BVC", AM_Relative,  0x50, 2, false, true},
    {"BVS", AM_Relative,  0x70, 2, false, true},

    {"CLC", AM_Implied,   0x18, 2},
    {"CLD", AM_Implied,   0xD8, 2},
    {"CLI", AM_Implied,   0x58, 2},
    {"CLV", AM_Implied,   0xB8, 2},

    {"CMP", AM_Immediate, 0xC9, 2},
    {"CMP", AM_ZeroPage,  0xC5, 3},
    {"CMP", AM_ZeroPageX, 0xD5, 4},
    {"CMP", AM_Absolute,  0xCD, 4},
    {"CMP", AM_AbsoluteX, 0xDD, 4, true},
    {"CMP", AM_AbsoluteY, 0xD9, 4, true},
    {"CMP", AM_IndirectX, 0xC1, 6},
    {"CMP", AM_IndirectY, 0xD1, 5, true},

    {"CPX", AM_Immediate, 0xE0, 2},
    {"CPX", AM_ZeroPage,  0xE4, 3},
    {"CPX", AM_Absolute,  0xEC, 4},

    {"CPY", AM_Immediate, 0xC0, 2},
    {"CPY", AM_ZeroPage,  0xC4, 3},
    {"CPY", AM_Absolute,  0xCC, 4},

    {"DEC", AM_ZeroPage,  0xC6, 5},
    {"DEC", AM_ZeroPageX, 0xD6, 6},
    {"DEC", AM_Absolute,  0xCE, 6},
    {"DEC", AM_AbsoluteX, 0xDE, 7},

    {"DEX", AM_Implied,   0xCA, 2},
    {"DEY", AM_Implied,   0x88, 2},

    {"EOR", AM_Immediate, 0x49, 2},
    {"EOR", AM_ZeroPage,  0x45, 3},
    {"EOR", AM_ZeroPageX, 0x55, 4},
    {"EOR", AM_Absolute,  0x4D, 4},
    {"EOR", AM_AbsoluteX, 0x5D, 4, true},
    {"EOR", AM_AbsoluteY, 0x59, 4, true},
    {"EOR", AM_IndirectX, 0x41, 6},
    {"EOR", AM_IndirectY, 0x51, 5, true},

    {"INC", AM_ZeroPage,  0xE6, 5},
    {"INC", AM_ZeroPageX, 0xF6, 6},
    {"INC", AM_Absolute,  0xEE, 6},
    {"INC", AM_AbsoluteX, 0xFE, 7},

    {"INX", AM_Implied,   0xCA, 2},
    {"INY", AM_Implied,   0x88, 2},

    {"JMP", AM_Absolute,  0x4C, 3},
    {"JMP", AM_Indirect,  0x6C, 5},

    {"JSR", AM_Absolute,  0x20, 6},

    {"LDA", AM_Immediate, 0xA9, 2},
    {"LDA", AM_ZeroPage,  0xA5, 3},
    {"LDA", AM_ZeroPageX, 0xB5, 4},
    {"LDA", AM_Absolute,  0xAD, 4},
    {"LDA", AM_AbsoluteX, 0xBD, 4, true},
    {"LDA", AM_AbsoluteY, 0xB9, 4, true},
    {"LDA", AM_IndirectX, 0xA1, 6},
    {"LDA", AM_IndirectY, 0xB1, 5, true},

    {"LDX", AM_Immediate, 0xA2, 2},
    {"LDX", AM_ZeroPage,  0xA6, 3},
    {"LDX", AM_ZeroPageY, 0xB6, 4},
    {"LDX", AM_Absolute,  0xAE, 4},
    {"LDX", AM_AbsoluteY, 0xBE, 4, true},

    {"LDY", AM_Immediate, 0xA0, 2},
    {"LDY", AM_ZeroPage,  0xA4, 3},
    {"LDY", AM_ZeroPageX, 0xB4, 4},
    {"LDY", AM_Absolute,  0xAC, 4},
    {"LDY", AM_AbsoluteX, 0xBC, 4, true},

    {"LSR", AM_Immediate, 0x4A, 2},
    {"LSR", AM_ZeroPage,  0x46, 5},
    {"LSR", AM_ZeroPageX, 0x56, 6},
    {"LSR", AM_Absolute,  0x4E, 6},
    {"LSR", AM_AbsoluteX, 0x5E, 7},

    {"NOP", AM_Implied,   0xEA, 2},

    {"ORA", AM_Immediate, 0x09, 2},
    {"ORA", AM_ZeroPage,  0x05, 3},
    {"ORA", AM_ZeroPageX, 0x15, 4},
    {"ORA", AM_Absolute,  0x0D, 4},
    {"ORA", AM_AbsoluteX, 0x1D, 4, true},
    {"ORA", AM_AbsoluteY, 0x19, 4, true},
    {"ORA", AM_IndirectX, 0x01, 6},
    {"ORA", AM_IndirectY, 0x11, 5, true},

    {"PHA", AM_Implied,   0x48, 3},
    {"PHP", AM_Implied,   0x08, 3},
    {"PLA", AM_Implied,   0x68, 4},
    {"PLP", AM_Implied,   0x28, 4},

    {"ROR", AM_Immediate, 0x6A, 2},
    {"ROR", AM_ZeroPage,  0x66, 5},
    {"ROR", AM_ZeroPageX, 0x76, 6},
    {"ROR", AM_Absolute,  0x4E, 6},
    {"ROR", AM_AbsoluteX, 0x7E, 7},

    {"RTI", AM_Implied,   0x40, 6},
    {"RTS", AM_Implied,   0x60, 6},

    { "SBC", AM_Immediate, 0xE9, 2},
    { "SBC", AM_ZeroPage,  0xE5, 3},
    { "SBC", AM_ZeroPageX, 0xF5, 4},
    { "SBC", AM_Absolute,  0xED, 4},
    { "SBC", AM_AbsoluteX, 0xFD, 4, true},
    { "SBC", AM_AbsoluteY, 0xF9, 4, true},
    { "SBC", AM_IndirectX, 0xE1, 6},
    { "SBC", AM_IndirectY, 0xF1, 5, true},

    { "SEC", AM_Implied,   0x38, 2},
    { "SED", AM_Implied,   0xF8, 2 },
    { "SEI", AM_Implied,   0x78, 2 },

    { "STA", AM_ZeroPage,  0x85, 3 },
    { "STA", AM_ZeroPageX, 0x95, 4 },
    { "STA", AM_Absolute,  0x8D, 4 },
    { "STA", AM_AbsoluteX, 0x9D, 5 },
    { "STA", AM_AbsoluteY, 0x99, 5 },
    { "STA", AM_IndirectX, 0x81, 6 },
    { "STA", AM_IndirectY, 0x91, 6 },

    { "STX", AM_ZeroPage,  0x86, 3 },
    { "STX", AM_ZeroPageY, 0x96, 4 },
    { "STX", AM_Absolute,  0x8E, 4 },

    { "STY", AM_ZeroPage,  0x84, 3 },
    { "STY", AM_ZeroPageX, 0x94, 4 },
    { "STY", AM_Absolute,  0x8C, 4 },

    { "TAX", AM_Implied,   0xAA, 3 },
    { "TAY", AM_Implied,   0xA8, 3 },
    { "TSX", AM_Implied,   0xBA, 3 },
    { "TXA", AM_Implied,   0x8A, 3 },
    { "TXS", AM_Implied,   0x9A, 3 },
    { "TYA", AM_Implied,   0x98, 3 }
};

CompilerOpcode s_opcodes[256];

bool ConvertToNumber(string& token, int& value)
{
    value = 0;
    char ch = token[0];
    if (ch == '$')
    {
        // HEX
        for (int i = 1; i < (int)token.size(); i++)
        {
            value = value * 16;
            char ch = toupper(token[i]);
            value += (ch >= 'A' && ch <= 'F') ? ch - 'A' + 10 : ch - '0';
        }
        return true;
    }
    else
    {
        int idx = 0;
        int sign = 1;
        if (ch == '-')
        {
            sign = -1;
            idx = 1;
            ch = token[1];
        }

        // DEC
        for (int i = idx; i < token.size(); i++)
        {
            ch = token[i];
            if (ch < '0' || ch > '9')
                return false;
            value = value * 10 + ch - '0';
        }
        return true;
    }
    return false;
}

Compiler::Compiler()
{
    memset(m_ram, 0, sizeof(m_ram));
    memset(s_opcodes, 0, sizeof(s_opcodes));

    // copy opcode data to indexed table - we'll need this for fast emulation
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        s_opcodes[s_opcodesRaw[i].opc] = s_opcodesRaw[i];
    }
}

bool Compiler::IsOpCode(const char* text)
{
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        if (StrEqual(text, s_opcodesRaw[i].name))
        {
            return true;
        }
    }
    return false;
}

CompilerOpcode* Compiler::FindOpcode(string &name, AddressingMode am)
{
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        if (StrEqual(name, s_opcodesRaw[i].name) && s_opcodesRaw[i].addressMode == am)
        {
            return &s_opcodesRaw[i];
        }
    }
    return nullptr;;
}

CompilerLabel* Compiler::FindLabel(CompilerSourceInfo *si, string& name)
{
    for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
    {
        if (StrEqual(s_systemLabels[i].m_name, name))
        {
            return &s_systemLabels[i];
        }
    }

    for (auto l : si->m_labels)
    {
        if (StrEqual(l->m_name, name))
        {
            return l;
        }
    }
    return nullptr;
}


struct TokenFifo
{
    TokenFifo(vector<string>& tokens) : m_tokens(tokens), m_end(""), m_index(0)
    {
        SkipSpacesAndComments();
    }

    string& Pop()
    {
        int result = m_index++;
        SkipSpacesAndComments();
        return (result < (int)m_tokens.size()) ? m_tokens[result] : m_end;
    }

    void SkipSpacesAndComments()
    {
        while (m_index < (int)m_tokens.size() && (m_tokens[m_index][0] == ' ' || m_tokens[m_index][0] == '\t' || m_tokens[m_index][0] == ';'))
            m_index++;
    }

    bool IsEmpty() { return m_index == m_tokens.size(); }

    int m_index;
    vector<string>& m_tokens;
    string m_end;
};

bool Compiler::ParseImmediateOperand(string &token, CompilerLineInfo *li)
{
    if (!ConvertToNumber(token, li->operand))
    {
        li->label = token;
        li->labelNeedsResolve = true;
    }
    return true;
}

bool Compiler::ParseOperand(string &token, AddressingMode amZeroPage, AddressingMode amAbsolute, CompilerSourceInfo* si, CompilerLineInfo *li)
{
    if (ConvertToNumber(token, li->operand))
    {
        li->addressMode = (li->operand > 0xff) ? amAbsolute : amZeroPage;
    }
    else
    {
        li->label = token;
        auto label = FindLabel(si, token);
        if (label)
        {
            li->operand = label->m_addr;
            li->addressMode = (li->operand > 0xff) ? amAbsolute : amZeroPage;
        }
        else
        {
            li->labelNeedsResolve = true;
            li->addressMode = amAbsolute;
        }
    }
    return true;
}

void Compiler::CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, vector<string>& tokens, int& currentMemAddr)
{
    TokenFifo fifo(tokens);

    auto& token = fifo.Pop();
    if (token.empty())
    {
        li->type = LT_Comment;
        return;
    }
    else if (StrEqual(token, "#"))
    {
        // TODO: pragmas...  include/import
        li->type = LT_Comment;
        return;
    }
    else if (StrEqual(token, "*"))
    {
        string tok1 = fifo.Pop();
        string tok2 = fifo.Pop();
        if (!StrEqual(tok1, "="))
            ERR("Err(ln:%d): Expected '=' after '*'", li->lineNmbr);
        int addr;
        if (!ConvertToNumber(tok2, addr))
            ERR("Err(ln:%d): Bad value '%s'", li->lineNmbr, tok2.c_str());

        currentMemAddr = addr;
        li->type = LT_Address;
        li->memAddr = addr;
        return;
    }
    else if (IsOpCode(token.c_str()))
    {
        // decode the opcode
        li->type = LT_Instruction;
        li->memAddr = currentMemAddr;

        li->opcode = 0;
        li->operand = 0;
        li->addressMode = AM_Implied;
        li->label = "";
        li->labelNeedsResolve;

        string tok1 = fifo.Pop();
        string tok2 = fifo.Pop();
        string tok3 = fifo.Pop();
        string tok4 = fifo.Pop();
        string tok5 = fifo.Pop();
        string tok6 = fifo.Pop();

        if (tok1.empty())
        {
            // implied
            li->addressMode = AM_Implied;
        }
        else if (StrEqual(tok1, "#"))
        {
            // immediate
            li->addressMode = AM_Immediate;
            if (!ParseImmediateOperand(tok2, li))
                return;
        }
        else if (StrEqual(tok1, "("))
        {
            if (StrEqual(tok3, ")"))
            {
                if (StrEqual(tok4, ","))
                {
                    // indirectY
                    li->addressMode = AM_IndirectY;
                    if (!ParseImmediateOperand(tok2, li))
                        return;
                }
                else
                {
                    // indirect
                    li->addressMode = AM_Indirect;
                    if (!ParseImmediateOperand(tok2, li))
                        return;
                }
            }
            else
            {
                // indirectX
                li->addressMode = AM_IndirectX;
                if (!ParseImmediateOperand(tok2, li))
                    return;
            }
        }
        else if (StrEqual(tok2, ","))
        {
            if (StrEqual(tok3, "x"))
            {
                // zeroPageX, absoluteX
                if (!ParseOperand(tok1, AM_ZeroPageX, AM_AbsoluteX, si, li))
                    return;
            }
            else
            {
                // zeroPageY, absoluteY
                if (!ParseOperand(tok1, AM_ZeroPageY, AM_AbsoluteY, si, li))
                    return;
            }
        }
        else
        {
            // zeropage, absolute, relative
            if (FindOpcode(token, AM_Relative))
            {
                li->addressMode = AM_Relative;
                if (!ParseImmediateOperand(tok1, li))
                    return;
            }
            else if (!ParseOperand(tok1, AM_ZeroPage, AM_Absolute, si, li))
                return;
        }

        auto opcode = FindOpcode(token, li->addressMode);
        if (opcode)
        {
            li->opcode = opcode->opc;
        }
        else
        {
            ERR("Err(%d): Cannot find version of '%s' that matches the addressing mode", li->lineNmbr, token.c_str());
        }

        currentMemAddr += gAddressingModeSize[li->addressMode];
    }
    else if (token.back() == ':')
    {
        // its a label
        if (token.size() == 1)
        {
            Error("ERR Ln:%d) malformed label", li->lineNmbr);
            return;
        }
        else
        {
            // label
            li->type = LT_Label;
            li->memAddr = currentMemAddr;
            li->label = token.substr(0, token.size() - 1);

            if (si->DoesLabelExist(li->label.c_str()))
            {
                Error("ERR Ln:%d) Duplicate label declaration: %s", li->lineNmbr, li->label.c_str());
                return;
            }

            si->m_labels.push_back(new CompilerLabel(li->label.c_str(), li->memAddr));
        }
    }
    else
    {
        ERR("Err(%d): Unexpected token - not a label or an opcode - '%s'", li->lineNmbr, token.c_str());
    }
}

void Compiler::CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li)
{
    if (li->labelNeedsResolve)
    {
        auto label = FindLabel(si, li->label);
        if (label)
        {
            if (li->addressMode == AM_Relative)
            {
                li->operand = (int)label->m_addr - (int)(li->memAddr+2);
                if (li->operand < -128 || li->operand > 127)
                {
                    ERR("Err(ln:%d): Relative branch too large", li->lineNmbr);
                    return;
                }
            }
            else if (li->addressMode == AM_ZeroPage || li->addressMode == AM_ZeroPageX || li->addressMode == AM_ZeroPageY || li->addressMode == AM_Indirect || li->addressMode == AM_IndirectX || li->addressMode == AM_IndirectY )
            {
                li->operand = label->m_addr;
                if (li->operand > 0xff)
                {
                    ERR("Err(ln:%d): Zero page operand too large", li->lineNmbr);
                    return;
                }
            }
            else
            {
                li->operand = label->m_addr;
            }
        }
        else
        {
            ERR("Err(ln:%d): Unable to resolve label '%s'", li->lineNmbr, li->label.c_str());
            return;
        }
    }

    if (li->type == LT_Instruction || li->type == LT_DataBytes)
    {
        int instructionSize = gAddressingModeSize[li->addressMode];
        li->data.clear();
        li->data.push_back(li->opcode);
        if (instructionSize == 2)
        {
            li->data.push_back((u8)(li->operand & 0xff));
        }
        else if (instructionSize == 3)
        {
            li->data.push_back((u8)(li->operand & 0xff));
            li->data.push_back((u8)(li->operand >> 8));
        }
    }
}

GraphicChunk* Compiler::GetMemAddrGC(class SourceFile* file, int line, int sourceVersion)
{
    auto settings = gApp->GetSettings();
    auto ci = file->GetCompileInfo();
    if (ci && ci->m_sourceVersion == file->GetSourceVersion())
    {
        auto sl = ci->m_lines[line];
        if (sl->type != LT_Unknown && sl->type != LT_Comment)
        {
            if (sl->gcMemAddr->IsEmpty())
            {
                char buffer[16];
                SDL_snprintf(buffer, 16, "%04x", sl->memAddr);
                SDL_Color addrCol = { 255, 255, 0, 255 };
                sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, addrCol, 0, 0));
            }
            return sl->gcMemAddr;
        }
    }
    return nullptr;
}

GraphicChunk* Compiler::GetDecodeGC(class SourceFile* file, int line, int sourceVersion)
{
    auto settings = gApp->GetSettings();
    auto ci = file->GetCompileInfo();
    if (ci && ci->m_sourceVersion == file->GetSourceVersion())
    {
        auto sl = ci->m_lines[line];
        if (sl->type == LT_Instruction)
        {
            if (sl->gcDecode->IsEmpty())
            {
                char buffer[16];
                auto opcode = &s_opcodes[sl->opcode];
                switch (gAddressingModeSize[sl->addressMode])
                {
                    case 1:
                        SDL_snprintf(buffer, 16, "%d %02x", opcode->cycles, sl->opcode);
                        break;
                    case 2:
                        SDL_snprintf(buffer, 16, "%d %02x %02x", opcode->cycles, sl->opcode, sl->operand & 0xff);
                        break;
                    case 3:
                        SDL_snprintf(buffer, 16, "%d %02x %02x %02x", opcode->cycles, sl->opcode, sl->operand & 0xff, (sl->operand >> 8) & 0xff);
                        break;
                }

                SDL_Color addrCol = { 255, 255, 0, 255 };
                sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, addrCol, 0, 0));
            }
            return sl->gcDecode;
        }
    }
    return nullptr;
}

void Compiler::Compile(SourceFile* file)
{
    int currentMemAddr = 0;

    m_errors.clear();

	auto sourceInfo = new CompilerSourceInfo();
    sourceInfo->m_sourceVersion = file->GetSourceVersion();
    file->SetCompileInfo(sourceInfo);

    int lineNmbr = 0;
    for (auto line : file->GetLines())
	{
		auto lineInfo = new CompilerLineInfo();
        lineInfo->lineNmbr = lineNmbr;
		CompileLinePass1(sourceInfo, lineInfo, line->GetTokens(), currentMemAddr);
        sourceInfo->m_lines.push_back(lineInfo);
        lineNmbr++;
	}

    for (auto line : sourceInfo->m_lines)
    {
        CompileLinePass2(sourceInfo, line);
    }
}

void Compiler::Error(const char* pFormat, ...)
{
    va_list va;
    va_start(va, pFormat);
    char buffer[1024];
    vsprintf(buffer, pFormat, va);
    string out(buffer, SDL_strlen(buffer));
    m_errors.push_back(out);
    printf("%s\n", out.c_str());
}

bool CompilerSourceInfo::DoesLabelExist(const char* label) 
{
    for (auto l : m_labels)
    {
        if (StrEqual(l->m_name.c_str(), label))
            return true;
    }
    return false;
}

void CompilerSourceInfo::ClearVisuals()
{
    for (auto l : m_lines)
    {
        l->gcDecode->Clear();
        l->gcMemAddr->Clear();
    }
}

void CompilerSourceInfo::SavePrg(const char* path)
{
    FILE* fh = fopen(path, "wb");
    if (fh)
    {
        u32 startAddr = 0xffff;
        u32 endAddr = 0;

        // pass one - find our memory range
        for (auto l : m_lines)
        {
            if (l->type == LT_Instruction || l->type == LT_DataBytes)
            {
                startAddr = min(l->memAddr, startAddr);
                endAddr = max(l->memAddr, endAddr);
            }
        }

        // allocate flat memory block with 2 byte header for address
        u32 memSize = (endAddr + 1) - startAddr + 2;
        u8* mem = (u8*)SDL_malloc((size_t)(memSize + 2));

        // write out lines to memory
        mem[0] = startAddr & 0xff;
        mem[1] = (startAddr >> 8) & 0xff;
        for (auto l : m_lines)
        {
            if (l->type == LT_Instruction || l->type == LT_DataBytes)
            {
                memcpy(mem + 2 + (l->memAddr - startAddr), l->data.data(), l->data.size());
            }
        }

        // save and close
        fwrite(mem, memSize, 1, fh);
        fclose(fh);
    }
}

