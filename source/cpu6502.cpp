#include "common.h"

namespace CPU6502
{
    bool Decode_LDA_Imm(Emulator* e)
    {
        e->m_regs.A = (u8)e->m_regs.operand;
        u8 N = (e->m_regs.operand & SR_Negative);
        u8 Z = (e->m_regs.operand ? SR_Zero : 0);
        e->m_regs.SR = (e->m_regs.SR & ~(SR_Zero|SR_Negative)) | N | Z;
        return false;
    }
    bool Decode_LDX_Imm(Emulator* e)
    {
        e->m_regs.X = (u8)e->m_regs.operand;
        u8 N = (e->m_regs.operand & SR_Negative);
        u8 Z = (e->m_regs.operand ? SR_Zero : 0);
        e->m_regs.SR = (e->m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
        return false;
    }
    bool Decode_STX_Abs(Emulator* e)
    {
        e->SetByte(e->m_regs.operand, e->m_regs.X);
        return false;
    }
    bool Decode_LDY_Imm(Emulator* e)
    {
        e->m_regs.Y = (u8)e->m_regs.operand;
        u8 N = (e->m_regs.operand & SR_Negative);
        u8 Z = (e->m_regs.operand ? SR_Zero : 0);
        e->m_regs.SR = (e->m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
        return false;
    }
    bool Decode_DEX(Emulator* e)
    {
        e->m_regs.X = e->m_regs.X - 1;
        u8 N = (e->m_regs.X & SR_Negative);
        u8 Z = (e->m_regs.X ? SR_Zero : 0);
        e->m_regs.SR = (e->m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
        return false;
    }
    bool Decode_DEY(Emulator* e)
    {
        e->m_regs.Y = e->m_regs.Y - 1;
        u8 N = (e->m_regs.Y & SR_Negative);
        u8 Z = (e->m_regs.Y ? SR_Zero : 0);
        e->m_regs.SR = (e->m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
        return false;
    }
    bool Decode_BNE(Emulator* e)
    {
        if (e->m_regs.SR & SR_Zero)
        {
            e->m_regs.PC += (i8)e->m_regs.operand;
            return true;
        }
        return false;
    }
    bool Decode_RTS(Emulator* e)
    {
        u16 addr = e->GetByte(0x101 + e->m_regs.SP);
        addr = addr | (e->GetByte(0x102 + e->m_regs.SP) << 8);
        e->m_regs.SP += 2;
        e->m_regs.PC = addr + 1;
        return false;
    }


    Opcode gOpcodesRaw[] =
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
        {"BNE", AM_Relative,  0xD0, 2, false, true, &Decode_BNE},
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

        {"DEX", AM_Implied,   0xCA, 2, false, false, &Decode_DEX},
        {"DEY", AM_Implied,   0x88, 2, false, false, &Decode_DEY},

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

        {"INX", AM_Implied,   0xE8, 2},
        {"INY", AM_Implied,   0xC8, 2},

        {"JMP", AM_Absolute,  0x4C, 3},
        {"JMP", AM_Indirect,  0x6C, 5},

        {"JSR", AM_Absolute,  0x20, 6},

        {"LDA", AM_Immediate, 0xA9, 2, false, false, &Decode_LDA_Imm},
        {"LDA", AM_ZeroPage,  0xA5, 3},
        {"LDA", AM_ZeroPageX, 0xB5, 4},
        {"LDA", AM_Absolute,  0xAD, 4},
        {"LDA", AM_AbsoluteX, 0xBD, 4, true},
        {"LDA", AM_AbsoluteY, 0xB9, 4, true},
        {"LDA", AM_IndirectX, 0xA1, 6},
        {"LDA", AM_IndirectY, 0xB1, 5, true},

        {"LDX", AM_Immediate, 0xA2, 2, false, false, &Decode_LDX_Imm},
        {"LDX", AM_ZeroPage,  0xA6, 3},
        {"LDX", AM_ZeroPageY, 0xB6, 4},
        {"LDX", AM_Absolute,  0xAE, 4},
        {"LDX", AM_AbsoluteY, 0xBE, 4, true},

        {"LDY", AM_Immediate, 0xA0, 2, false, false, &Decode_LDY_Imm },
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
        {"RTS", AM_Implied,   0x60, 6, false, false, &Decode_RTS},

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

        { "STY", AM_ZeroPage,  0x84, 3 , false, false, &Decode_STX_Abs},
        { "STY", AM_ZeroPageX, 0x94, 4 },
        { "STY", AM_Absolute,  0x8C, 4 },

        { "TAX", AM_Implied,   0xAA, 3 },
        { "TAY", AM_Implied,   0xA8, 3 },
        { "TSX", AM_Implied,   0xBA, 3 },
        { "TXA", AM_Implied,   0x8A, 3 },
        { "TXS", AM_Implied,   0x9A, 3 },
        { "TYA", AM_Implied,   0x98, 3 }
    };

    int gAddressingModeSize[] =
    {
        1,  //    AM_Implied,              // operand
        2,  //    AM_Immediate,            // operand #value
        2,  //    AM_Relative,             // operand value
        2,  //    AM_ZeroPage,             // operand value
        2,  //    AM_ZeroPageX,            // operand value,x
        2,  //    AM_ZeroPageY,            // operand value,y
        2,  //    AM_IndirectX,            // operand (value, x)
        2,  //    AM_IndirectY,            // operand (value), y
        3,  //    AM_Absolute,             // operand value
        3,  //    AM_AbsoluteX,            // operand value,x
        3,  //    AM_AbsoluteY,            // operand value,y
        3   //    AM_Indirect              // operand (value)
    };

    const char* gAddressingModeName[] =
    {
        "Implied", "Immediate", "Zero Page", "Zero Page X", "Zero Page Y", "Absolute", "Absolute X", "Absolute Y", "Indirect", "Indirect X", "Indirect Y", "Relative"
    };

    Opcode gOpcodes[256];
    vector<Opcode*> gUniqueOpcodes;

    void InitArrays()
    {
        memset(gOpcodes, 0, sizeof(gOpcodes));

        // copy opcode data to indexed table - we'll need this for fast emulation
        int cnt = sizeof(gOpcodesRaw) / sizeof(Opcode);
        for (int i = 0; i < cnt; i++)
        {
            gOpcodes[gOpcodesRaw[i].opc] = gOpcodesRaw[i];

            if ((i == 0) || (gOpcodesRaw[i - 1].name != gOpcodesRaw[i].name))
                gUniqueOpcodes.push_back(&gOpcodesRaw[i]);
        }
    }

    bool IsOpcode(const char* text)
    {
        for (auto opcode : gUniqueOpcodes)
        {
            if (StrEqual(text, opcode->name))
            {
                return true;
            }
        }
        return false;
    }

    Opcode* FindOpcode(string& name, AddressingMode am)
    {
        int cnt = sizeof(gOpcodesRaw) / sizeof(Opcode);
        for (int i = 0; i < cnt; i++)
        {
            if (StrEqual(name, gOpcodesRaw[i].name) && gOpcodesRaw[i].addressMode == am)
            {
                return &gOpcodesRaw[i];
            }
        }
        return nullptr;
    }

};


