#pragma once

class Emulator;

namespace CPU6502
{
    enum AddressingMode
    {
        AM_Implied,              // operand
        AM_Immediate,            // operand #value
        AM_Relative,             // operand value
        AM_ZeroPage,             // operand value
        AM_ZeroPageX,            // operand value,x
        AM_ZeroPageY,            // operand value,y
        AM_IndirectX,            // operand (value, x)
        AM_IndirectY,            // operand (value), y
        AM_Absolute,             // operand value
        AM_AbsoluteX,            // operand value,x
        AM_AbsoluteY,            // operand value,y
        AM_Indirect              // operand (value)
    };
    extern int gAddressingModeSize[];
    extern const char* gAddressingModeName[];

    typedef bool (*Decode)(class Emulator*);
    struct Opcode
    {
        const char* name;
        AddressingMode addressMode;
        u8 opc;
        u8 cycles;

        bool extraCycleOnPageBoundary;
        bool extraCycleOnBranch;

        Decode m_decode;
    };

    enum CpuStatusRegisters
    {
        SR_Carry = 1,
        SR_Zero = 2,
        SR_Interrupt = 4,
        SR_Decimal = 8,
        SR_Break = 16,
        SR_Overflow = 64,
        SR_Negative = 128
    };

    struct CpuRegs
    {
        u16 PC;
        u8 A;
        u8 X;
        u8 Y;
        u8 SR;
        u8 SP;

        u16 operand;     // address mode applied
        u32 frameCycle;  // cycles this refresh
    };

    // array of 256 opcodes matching machine language value
    extern Opcode gOpcodes[256];

    // array of unique opcode names for quick lookup of string->opcode
    extern vector<Opcode*> gUniqueOpcodes;

    // initializes arrays such as gOpcodes6502
    extern void InitArrays();
    extern bool IsOpcode(const char* text);
    extern Opcode* FindOpcode(string& name, AddressingMode am);
}
