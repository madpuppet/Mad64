#include "common.h"
#include "compiler.h"
#include "graphicChunk.h"

CompilerLabel s_systemLabels[] =
{
    CompilerLabel("vic.sprite0X", 0xd000),  CompilerLabel("vic.sprite0Y", 0xd001),  CompilerLabel("vic.sprite1X", 0xd002),  CompilerLabel("vic.sprite1Y", 0xd003),
    CompilerLabel("vic.sprite2X", 0xd004),  CompilerLabel("vic.sprite2Y", 0xd005),  CompilerLabel("vic.sprite3X", 0xd006),  CompilerLabel("vic.sprite3Y", 0xd007),
    CompilerLabel("vic.sprite4X", 0xd008),  CompilerLabel("vic.sprite4Y", 0xd009),  CompilerLabel("vic.sprite5X", 0xd00a),  CompilerLabel("vic.sprite5Y", 0xd00b),
    CompilerLabel("vic.sprite6X", 0xd00c),  CompilerLabel("vic.sprite6Y", 0xd00d),  CompilerLabel("vic.sprite7X", 0xd00e),  CompilerLabel("vic.sprite7Y", 0xd00f),
    CompilerLabel("vic.spriteXMSB", 0xd010), CompilerLabel("vic.control1", 0xd011), CompilerLabel("vic.rasterCounter", 0xd012), CompilerLabel("vic.lightPenX", 0xd013),
    CompilerLabel("vic.lightPenY", 0xd014), CompilerLabel("vic.spriteEnable", 0xd015), CompilerLabel("vic.control2", 0xd016), CompilerLabel("vic.spriteYSize", 0xd017),
    CompilerLabel("vic.memoryPointer", 0xd018), CompilerLabel("vic.interruptRegister", 0xd019), CompilerLabel("vic.interruptEnable", 0xd01a), CompilerLabel("vic.spritePriority", 0xd01b),
    CompilerLabel("vic.spriteMulticolor", 0xd01c), CompilerLabel("vic.spriteXSize", 0xd01d), CompilerLabel("vic.spriteToSpriteCollision", 0xd01e), CompilerLabel("vic.spriteToDataCollision", 0xd01f),
    CompilerLabel("vic.borderColor", 0xd020), CompilerLabel("vic.backgroundColor0", 0xd021), CompilerLabel("vic.backgroundColor1", 0xd022), CompilerLabel("vic.backgroundColor2", 0xd023),
    CompilerLabel("vic.backgroundColor3", 0xd024), CompilerLabel("vic.spriteMulticolor0", 0xd025), CompilerLabel("vic.spriteMulticolor1", 0xd026), CompilerLabel("vic.sprite0Color", 0xd027),
    CompilerLabel("vic.sprite1Color", 0xd028), CompilerLabel("vic.sprite2Color", 0xd029), CompilerLabel("vic.sprite3Color", 0xd02a), CompilerLabel("vic.sprite4Color", 0xd02b),
    CompilerLabel("vic.sprite5Color", 0xd02c), CompilerLabel("vic.sprite6Color", 0xd02d), CompilerLabel("vic.sprite7Color", 0xd02e), CompilerLabel("vic.colorMemory", 0xd800),

    CompilerLabel("cia1.dataPortA", 0xdc00), CompilerLabel("cia1.dataPortB", 0xdc01), CompilerLabel("cia1.dataDirectionA", 0xdc02), CompilerLabel("cia1.dataDirectionB", 0xdc03),
    CompilerLabel("cia1.timerALow", 0xdc04), CompilerLabel("cia1.timerAHigh", 0xdc05), CompilerLabel("cia1.timerBLow", 0xdc06), CompilerLabel("cia1.timerBHigh", 0xdc07),
    CompilerLabel("cia1.clockTenths", 0xdc08), CompilerLabel("cia1.clockSeconds", 0xdc09), CompilerLabel("cia1.clockMinutes", 0xdc0a), CompilerLabel("cia1.clockHours", 0xdc0b),
    CompilerLabel("cia1.serialShift", 0xdc0c), CompilerLabel("cia1.interruptControl", 0xdc0d), CompilerLabel("cia1.controlTimerA", 0xdc0e), CompilerLabel("cia1.controlTimerB", 0xdc0f),

    CompilerLabel("cia2.dataPortA", 0xdd00), CompilerLabel("cia2.dataPortB", 0xdd01), CompilerLabel("cia2.dataDirectionA", 0xdd02), CompilerLabel("cia2.dataDirectionB", 0xdd03),
    CompilerLabel("cia2.timerALow", 0xdd04), CompilerLabel("cia2.timerAHigh", 0xdd05), CompilerLabel("cia2.timerBLow", 0xdd06), CompilerLabel("cia2.timerBHigh", 0xdd07),
    CompilerLabel("cia2.clockTenths", 0xdd08), CompilerLabel("cia2.clockSeconds", 0xdd09), CompilerLabel("cia2.clockMinutes", 0xdd0a), CompilerLabel("cia2.clockHours", 0xdd0b),
    CompilerLabel("cia2.serialShift", 0xdd0c), CompilerLabel("cia2.interruptControl", 0xdd0d), CompilerLabel("cia2.controlTimerA", 0xdd0e), CompilerLabel("cia2.controlTimerB", 0xdd0f),

    CompilerLabel("rom.SETLFS", 0xffba), CompilerLabel("rom.SETNAM", 0xffbd), CompilerLabel("rom.LOAD", 0xffd5),

    CompilerLabel("sid.v1_freqLow", 0xd400), CompilerLabel("sid.v1_freqHi", 0xd401), CompilerLabel("sid.v1_pulseWaveformWidthLo", 0xd402), CompilerLabel("sid.v1_pulseWaveformWidthHi", 0xd403),
    CompilerLabel("sid.b1_Control", 0xd404), CompilerLabel("sid.v1_envelope_ad", 0xd405), CompilerLabel("sid.v1_envelope_sr", 0xd406), CompilerLabel("sid.v2_freqLow", 0xd407),
    CompilerLabel("sid.v2_freqHi", 0xd408), CompilerLabel("sid.v2_pulseWaveformWidthLo", 0xd409), CompilerLabel("sid.v2_pulseWaveformWidthHi", 0xd40a), CompilerLabel("sid.v2_control", 0xd40b),
    CompilerLabel("sid.v2_envelope_ad", 0xd40c), CompilerLabel("sid.v2_envelope_sr", 0xd40d), CompilerLabel("sid.v3_freqLow", 0xd40e), CompilerLabel("sid.v3_freqHi", 0xd40f),
    CompilerLabel("sid.v3_pulseWaveformWidthLo", 0xd410), CompilerLabel("sid.v3_pulseWaveformWidthHi", 0xd411), CompilerLabel("sid.v3_control", 0xd412), CompilerLabel("sid.v3_envelope_ad", 0xd413),
    CompilerLabel("sid.v3_envelope_sr", 0xd414), CompilerLabel("sid.filterCutoffFreqLo", 0xd415), CompilerLabel("sid.filterCutoffFreqHi", 0xd416), CompilerLabel("sid.filterResonance", 0xd417),
    CompilerLabel("sid.filterModeAndVolume", 0xd418), CompilerLabel("sid.analogDigitalConvert1", 0xd419), CompilerLabel("sid.analogDigitalConvert2", 0xd41a), CompilerLabel("sid.oscillator3Random", 0xd41b),
    CompilerLabel("sid.envelope3Output", 0xd41c)
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

    {"LSR", AM_Implied,   0x4A, 2},
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

bool DecimalToNumber(string& token, int& value)
{
    value = 0;
    char ch = token[0];
    int idx = 0;
    int sign = 1;
    if (ch == '-')
    {
        sign = -1;
        idx = 1;
        ch = token[1];
    }

    for (int i = idx; i < token.size(); i++)
    {
        ch = token[i];
        if (ch < '0' || ch > '9')
            return false;
        value = value * 10 + ch - '0';
    }
    return true;
}

bool HexToNumber(string& token, int& value)
{
    value = 0;
    for (int i = 0; i < (int)token.size(); i++)
    {
        char ch = toupper(token[i]);
        if ((ch < 'A' && ch > 'F') || (ch < '0' && ch > '9'))
            return false;

        value = value * 16;
        value += (ch >= 'A' && ch <= 'F') ? ch - 'A' + 10 : ch - '0';
    }
    return true;
}

bool BinaryToNumber(string& token, int& value)
{
    value = 0;
    for (int i = 0; i < (int)token.size(); i++)
    {
        char ch = toupper(token[i]);
        if (ch != '0' && ch != '1')
            return false;

        value = value * 2;
        value += ch == '0' ? 0 : 1;
    }
    return true;
}

bool ConvertToNumber(TokenFifo& fifo, int& value)
{
    int mark = fifo.Mark();
    bool success;

    if (fifo.Peek(0) == "$")
        success = HexToNumber(fifo.Pop(1), value);
    else if (fifo.Peek(0) == "%")
        success = BinaryToNumber(fifo.Pop(1), value);
    else
        success = DecimalToNumber(fifo.Pop(), value);

    if (!success)
        fifo.ReturnToMark(mark);

    return success;
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

CompilerLabel* Compiler::FindLabel(CompilerSourceInfo* si, string& name, LabelResolve resolve, u32 resolveStartAddr)
{
    if (resolve == LabelResolve_Global)
    {
        for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
        {
            if (StrEqual(s_systemLabels[i].m_name, name))
            {
                return &s_systemLabels[i];
            }
        }
    }

    if (resolve == LabelResolve_Global)
    {
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                return l;
            }
        }
    }
    else if (resolve == LabelResolve_Backwards)
    {
        CompilerLabel* best = nullptr;

        // find largest label that is smaller than the start addr
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                if (!best || (l->m_addr > best->m_addr))
                {
                    if (l->m_addr < resolveStartAddr)
                    {
                        best = l;
                    }
                }
            }
        }
        return best;
    }
    else
    {
        CompilerLabel* best = nullptr;

        // find smallest label that is larger than the start addr
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                if (!best || (l->m_addr < best->m_addr))
                {
                    if (l->m_addr >= resolveStartAddr)
                    {
                        best = l;
                    }
                }
            }
        }
        return best;
    }
    return nullptr;
}

bool Compiler::ParseImmediateOperand(TokenFifo &fifo, CompilerLineInfo *li)
{
    if (!ConvertToNumber(fifo, li->operand))
    {
        if (StrEqual(fifo.Peek(), "!"))
        {
            fifo.Pop();
        }
        li->label = fifo.Pop();

        if (StrEqual(fifo.Peek(), "+"))
        {
            li->labelResolve = LabelResolve_Forwards;
            fifo.Pop();
        }
        else if (StrEqual(fifo.Peek(), "-"))
        {
            li->labelResolve = LabelResolve_Backwards;
            fifo.Pop();
        }
        else
            li->labelResolve = LabelResolve_Global;
    }
    return true;
}

bool Compiler::ParseOperand(TokenFifo &fifo, AddressingMode amZeroPage, AddressingMode amAbsolute, CompilerSourceInfo* si, CompilerLineInfo *li)
{
    bool isLocal;
    if (ConvertToNumber(fifo, li->operand))
    {
        li->addressMode = (li->operand > 0xff) ? amAbsolute : amZeroPage;
    }
    else if (fifo.PopLabel(li->label, isLocal, li->labelResolve))
    {
        auto label = FindLabel(si, li->label, li->labelResolve, li->memAddr);
        if (label)
        {
            li->operand = label->m_addr;
            li->addressMode = (li->operand > 0xff) ? amAbsolute : amZeroPage;
            li->labelResolve = LabelResolve_Done;
        }
        else
        {
            if (StrEqual(fifo.Peek(), "!"))
                fifo.Pop();

            if (StrEqual(fifo.Peek(), "+"))
            {
                li->labelResolve = LabelResolve_Forwards;
                fifo.Pop();
            }
            else if (StrEqual(fifo.Peek(), "-"))
            {
                li->labelResolve = LabelResolve_Backwards;
                fifo.Pop();
            }
            else
                li->labelResolve = LabelResolve_Global;

            li->addressMode = amAbsolute;
        }
    }
    return true;
}

void Compiler::CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine *sourceLine, int& currentMemAddr)
{
    auto tokens = sourceLine->GetTokens();
    TokenFifo fifo(tokens);
    string &token = fifo.Pop();
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
    else if (StrEqual(token, ".import"))
    {
        
    }
    else if (StrEqual(token, "*"))
    {
        string& tok1 = fifo.Pop();
        string& tok2 = fifo.Pop();

        if (!StrEqual(tok1, "="))
        {
            ERR("Expected '=' after '*'");
        }
        int addr;
        if (!ConvertToNumber(fifo, addr))
        {
            ERR("Bad value '%s'", tok2.c_str());
        }

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

        if (fifo.Peek().empty())
        {
            // implied
            li->addressMode = AM_Implied;
        }
        else if (StrEqual(fifo.Peek(), "#"))
        {
            // immediate
            fifo.Pop();
            li->addressMode = AM_Immediate;
            if (!ParseImmediateOperand(fifo, li))
                return;
        }
        else if (StrEqual(fifo.Peek(), "("))
        {
            if (StrEqual(fifo.Peek(2), ")"))
            {
                if (StrEqual(fifo.Peek(3), ","))
                {
                    // indirectY
                    li->addressMode = AM_IndirectY;
                    fifo.Pop(1);
                    if (!ParseImmediateOperand(fifo, li))
                        return;
                }
                else
                {
                    // indirect
                    li->addressMode = AM_Indirect;
                    fifo.Pop(1);
                    if (!ParseImmediateOperand(fifo, li))
                        return;
                }
            }
            else
            {
                // indirectX
                li->addressMode = AM_IndirectX;
                fifo.Pop(1);
                if (!ParseImmediateOperand(fifo, li))
                    return;
            }
        }
        else if (StrEqual(fifo.Peek(1), ","))
        {
            if (StrEqual(fifo.Peek(2), "x"))
            {
                // zeroPageX, absoluteX
                if (!ParseOperand(fifo, AM_ZeroPageX, AM_AbsoluteX, si, li))
                    return;
            }
            else
            {
                // zeroPageY, absoluteY
                if (!ParseOperand(fifo, AM_ZeroPageY, AM_AbsoluteY, si, li))
                    return;
            }
        }
        else
        {
            // zeropage, absolute, relative
            if (FindOpcode(token, AM_Relative))
            {
                li->addressMode = AM_Relative;
                if (!ParseImmediateOperand(fifo, li))
                    return;
            }
            else if (!ParseOperand(fifo, AM_ZeroPage, AM_Absolute, si, li))
                return;
        }

        auto opcode = FindOpcode(token, li->addressMode);
        if (opcode)
        {
            li->opcode = opcode->opc;
        }
        else if (li->addressMode == AM_ZeroPage)
        {
            // try a full absolute..
            li->addressMode = AM_Absolute;
            opcode = FindOpcode(token, li->addressMode);
            if (!opcode)
                ERR("Cannot find version of '%s' that matches the addressing mode", token.c_str());
        }

        currentMemAddr += gAddressingModeSize[li->addressMode];
    }
    else if (token == "!")
    {
        // local address label
        string& tok1 = fifo.Pop();
        string& tok2 = fifo.Pop();
        if (tok2 != ":")
        {
            ERR("Expected ':' at end of label");
        }

        // address label
        li->type = LT_Label;
        li->label = tok1;
        li->memAddr = currentMemAddr;
        li->operand = li->memAddr;
        si->m_labels.push_back(new CompilerLabel(li->label.c_str(), currentMemAddr));
    }
    else if (StrEqual(token, "dc.b") || StrEqual(token, ".byte"))
    {
        li->type = LT_DataBytes;
        li->memAddr = currentMemAddr;

        while (!fifo.IsEmpty())
        {
            int dataval;
            if (!ConvertToNumber(fifo, dataval) || dataval < -128 || dataval > 255)
            {
                ERR("Data byte is malformed or outside 1 byte range");
            }
            li->data.push_back((u8)dataval);

            if (!fifo.IsEmpty())
            {
                // skip ','
                string& comma = fifo.Pop();
                if (comma != ",")
                    ERR("Expected comma in data bytes")
            }
        }
    }
    else if (StrEqual(token, ".text"))
    {
        li->type = LT_DataBytes;
        li->memAddr = currentMemAddr;

        string text = fifo.Pop();
        if (text.empty() || text.front() != '"' || text.back() != '"')
            ERR("Malformed text");
        for (auto c : text)
            li->data.push_back((u8)c);
    }
    else if (StrEqual(token, "dc.w") || StrEqual(token, ".word"))
    {
        li->type = LT_DataBytes;
        li->memAddr = currentMemAddr;

        while (!fifo.IsEmpty())
        {
            int dataval;
            if (!ConvertToNumber(fifo, dataval) || dataval < -32768 || dataval > 65535)
            {
                ERR("Data byte is malformed or outside 2 byte range");
            }
            li->data.push_back((u8)(dataval&0xff));
            li->data.push_back((u8)(dataval>>8));

            if (!fifo.IsEmpty())
            {
                // skip ','
                string& comma = fifo.Pop();
                if (comma != ",")
                    ERR("Expected comma in data bytes")
            }
        }
    }
    else
    {
        string& tok1 = fifo.Pop();
        if (tok1 == ":")
        {
            // address label
            li->type = LT_Label;
            li->memAddr = currentMemAddr;
            li->operand = li->memAddr;
            li->label = token;
            if (si->DoesLabelExist(li->label.c_str()))
            {
                ERR("Duplicate label declaration: %s", li->label.c_str());
            }
            si->m_labels.push_back(new CompilerLabel(li->label.c_str(), li->memAddr));
        }
        else if (StrEqual(tok1, "="))
        {
            // label assignment
            li->type = LT_Variable;
            li->label = token;
            int value;
            if (!ConvertToNumber(fifo, value))
            {
                ERR("Cannot parse var %s", token.c_str());
            }
            if (si->DoesLabelExist(li->label.c_str()))
            {
                ERR("Duplicate variable declaration: %s", li->label.c_str());
            }
            li->operand = value;
            li->memAddr = value;
            si->m_labels.push_back(new CompilerLabel(li->label.c_str(), value));
        }
        else
        {
            ERR("Unexpected token - not a label or an opcode - '%s'", token.c_str());
        }
    }
}

void Compiler::CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine)
{
    if (li->labelResolve == LabelResolve_Global || li->labelResolve == LabelResolve_Forwards || li->labelResolve == LabelResolve_Backwards)
    {
        auto label = FindLabel(si, li->label, li->labelResolve, li->memAddr);
        if (label)
        {
            if (li->addressMode == AM_Relative)
            {
                li->operand = (int)label->m_addr - (int)(li->memAddr+2);
                if (li->operand < -128 || li->operand > 127)
                {
                    ERR("Relative branch too large", li->lineNmbr); li->error = true;
                }
            }
            else if (li->addressMode == AM_ZeroPage || li->addressMode == AM_ZeroPageX || li->addressMode == AM_ZeroPageY || li->addressMode == AM_Indirect || li->addressMode == AM_IndirectX || li->addressMode == AM_IndirectY )
            {
                li->operand = label->m_addr;
                if (li->operand > 0xff)
                {
                    ERR("Zero page operand too large");
                }
            }
            else
            {
                li->operand = label->m_addr;
            }
        }
        else
        {
            ERR("Unable to resolve label '%s'", li->label.c_str());
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
                if (sl->error)
                {
                    SDL_Color addrCol = { 255, 0, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetFont(), "****", addrCol, 0, 0));
                }
                else
                {
                    char buffer[16];
                    SDL_snprintf(buffer, 16, "%04x", sl->memAddr);
                    SDL_Color addrCol = { 255, 255, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, addrCol, 0, 0));
                }
            }
            return sl->gcMemAddr;
        }
    }
    return nullptr;
}

GraphicChunk* Compiler::GetDecodeGC(class SourceFile* file, int line, int sourceVersion)
{
    SDL_Color dataCol = { 255, 64, 64, 255 };
    SDL_Color cycleCol = { 255, 255, 0, 255 };

    auto settings = gApp->GetSettings();
    auto ci = file->GetCompileInfo();
    if (ci && ci->m_sourceVersion == file->GetSourceVersion())
    {
        auto sl = ci->m_lines[line];
        if (sl->gcDecode->IsEmpty())
        {
            if (sl->type == LT_Instruction)
            {
                char buffer[16];
                auto opcode = &s_opcodes[sl->opcode];
                SDL_snprintf(buffer, 16, "%d", opcode->cycles);
                sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, cycleCol, 0, 0));
            }
            if (sl->type == LT_Instruction || sl->type == LT_DataBytes)
            {
                char buffer[16];
                for (int i = 0; i < min((int)sl->data.size(), 8); i++)
                {
                    SDL_snprintf(buffer, 16, "%02x", sl->data[i]);
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, dataCol, gApp->GetWhiteSpaceWidth() * (3 + i * 3), 0));
                }
                if (sl->data.size() > 8)
                {
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), "..", dataCol, gApp->GetWhiteSpaceWidth() * (3 + 8 * 3), 0));
                }
            }
        }
        return sl->gcDecode;
    }
    return nullptr;
}

void Compiler::Compile(SourceFile* file)
{
    Profile PF("COMPILE");

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
		CompileLinePass1(sourceInfo, lineInfo, line, currentMemAddr);
        sourceInfo->m_lines.push_back(lineInfo);
        lineNmbr++;
	}

    for (auto line : sourceInfo->m_lines)
    {
        CompileLinePass2(sourceInfo, line, file->GetLines()[line->lineNmbr]);
    }

    u64 timeEnd = SDL_GetTicks64();
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

