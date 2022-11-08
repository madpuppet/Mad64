#pragma once

class Cpu6502
{
public:
    Cpu6502();

    typedef DMFastDelegate::FastDelegate0<> Decode;

    enum AddressingMode
    {
        AM_Imp,              // operand
        AM_Imm,             // operand #value
        AM_Rel,             // operand value
        AM_Zero,            // operand value
        AM_ZeroX,           // operand value,x
        AM_ZeroY,           // operand value,y
        AM_IndX,            // operand (value, x)
        AM_IndY,            // operand (value), y
        AM_Abs,             // operand value
        AM_AbsX,            // operand value,x
        AM_AbsY,            // operand value,y
        AM_Ind              // operand (value)
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

    struct Opcode
    {
        const char* name;
        AddressingMode addressMode;
        u8 opc;
        u8 cycles;
        Decode decode;
    };

    struct Registers
    {
        u16 PC;
        u8 A;
        u8 X;
        u8 Y;
        u8 SR;
        u8 SP;

        Opcode* op;      // opcode fetched
        u16 operand;            // opcode data fetched
        u32 frameCycle;         // cycles this refresh
        int decodeCycle;        // current decode cycle
        int delayCycles;        // some instructions may invoke 1 or 2 delay cycles before they complete
        int opcodeCycleCount;   // how many cycles (+delayCycles) does this opcode take

        // helpers
        void SetNZ(u8 val)
        {
            u8 N = val & SR_Negative;
            u8 Z = (val & 0xff) ? 0 : SR_Zero;
            SR = (SR & ~(SR_Negative | SR_Zero)) | N | Z;
        }

        void SetNZC(u8 val, u8 C)
        {
            u8 N = val & SR_Negative;
            u8 Z = (val & 0xff) ? 0 : SR_Zero;
            SR = (SR & ~(SR_Negative | SR_Zero | SR_Carry)) | N | Z | C;
        }
    };

    void Reset(u16 cpuStart);

    Registers& Regs() { return m_regs; }

    bool IsOpcode(const char* text);
    Opcode* FindOpcode(const string& name, AddressingMode am);
    Opcode* GetOpcode(u8 opcode) { return &m_opcodes[opcode]; }
    int GetAddressingModeSize(AddressingMode am);
    const char* GetAddressingModeName(AddressingMode am);

    // return true when an opcode has completed
    bool Step();

    void SetMemReadByte(const ReadByteHook& hook) { MemReadByte = hook; }
    void SetMemWriteByte(const WriteByteHook& hook) { MemWriteByte = hook; }

private:
    // HOOKS
    ReadByteHook MemReadByte;
    WriteByteHook MemWriteByte;

    // REGS
    Registers m_regs;

    // array of 256 opcodes matching machine language value
    Opcode m_opcodes[256];

    // array of unique opcode names for quick lookup of string->opcode
    vector<string> m_uniqueOpcodes;

    // addressing mode helpers
    u16 Decode_Zero_Addr();
    u16 Decode_ZeroX_Addr();
    u16 Decode_ZeroY_Addr();
    u16 Decode_Abs_Addr();
    u16 Decode_AbsX_Addr();
    u16 Decode_AbsY_Addr();
    u16 Decode_Ind_Addr();
    u16 Decode_IndX_Addr();
    u16 Decode_IndY_Addr();
    void Decode_Branch();

    // opcode decodes
    void Decode_Unknown();
    void Decode_BRK_Imp();
    void Decode_CLC_Imp();
    void Decode_CLD_Imp();
    void Decode_CLI_Imp();
    void Decode_CLV_Imp();
    void Decode_DEX_Imp();
    void Decode_DEY_Imp();
    void Decode_INX_Imp();
    void Decode_INY_Imp();
    void Decode_NOP_Imp();
    void Decode_PHA_Imp();
    void Decode_PHP_Imp();
    void Decode_PLA_Imp();
    void Decode_PLP_Imp();
    void Decode_SEC_Imp();
    void Decode_SED_Imp();
    void Decode_SEI_Imp();
    void Decode_TAX_Imp();
    void Decode_TAY_Imp();
    void Decode_TSX_Imp();
    void Decode_TXA_Imp();
    void Decode_TXS_Imp();
    void Decode_TYA_Imp();
    void Decode_RTI_Imp();
    void Decode_RTS_Imp();

    void Decode_BCC_Rel();
    void Decode_BCS_Rel();
    void Decode_BEQ_Rel();
    void Decode_BNE_Rel();
    void Decode_BMI_Rel();
    void Decode_BPL_Rel();
    void Decode_BVC_Rel();
    void Decode_BVS_Rel();

    void Decode_ADC(u8 M);
    void Decode_ADC_Imm();
    void Decode_ADC_Zero();
    void Decode_ADC_ZeroX();
    void Decode_ADC_Abs();
    void Decode_ADC_AbsX();
    void Decode_ADC_AbsY();
    void Decode_ADC_IndX();
    void Decode_ADC_IndY();

    void Decode_AND(u8 M);
    void Decode_AND_Imm();
    void Decode_AND_Zero();
    void Decode_AND_ZeroX();
    void Decode_AND_Abs();
    void Decode_AND_AbsX();
    void Decode_AND_AbsY();
    void Decode_AND_IndX();
    void Decode_AND_IndY();

    void Decode_CMP(u8 M);
    void Decode_CMP_Imm();
    void Decode_CMP_Zero();
    void Decode_CMP_ZeroX();
    void Decode_CMP_Abs();
    void Decode_CMP_AbsX();
    void Decode_CMP_AbsY();
    void Decode_CMP_IndX();
    void Decode_CMP_IndY();

    void Decode_CPX(u8 M);
    void Decode_CPX_Imm();
    void Decode_CPX_Zero();
    void Decode_CPX_Abs();

    void Decode_CPY(u8 M);
    void Decode_CPY_Imm();
    void Decode_CPY_Zero();
    void Decode_CPY_Abs();

    void Decode_ASL(u16 addr);
    void Decode_ASL_Imp();
    void Decode_ASL_Zero();
    void Decode_ASL_ZeroX();
    void Decode_ASL_Abs();
    void Decode_ASL_AbsX();

    void Decode_BIT(u16 addr);
    void Decode_BIT_Zero();
    void Decode_BIT_Abs();

    void Decode_DEC(u16 addr);
    void Decode_DEC_Zero();
    void Decode_DEC_ZeroX();
    void Decode_DEC_Abs();
    void Decode_DEC_AbsX();

    void Decode_EOR(u8 M);
    void Decode_EOR_Imm();
    void Decode_EOR_Zero();
    void Decode_EOR_ZeroX();
    void Decode_EOR_ZeroY();
    void Decode_EOR_Abs();
    void Decode_EOR_AbsX();
    void Decode_EOR_AbsY();
    void Decode_EOR_IndX();
    void Decode_EOR_IndY();

    void Decode_INC(u16 addr);
    void Decode_INC_Zero();
    void Decode_INC_ZeroX();
    void Decode_INC_Abs();
    void Decode_INC_AbsX();

    void Decode_JMP(u16 addr);
    void Decode_JMP_Abs();
    void Decode_JMP_Ind();

    void Decode_JSR_Abs();

    void Decode_LDA(u16 addr);
    void Decode_LDA_Imm();
    void Decode_LDA_Zero();
    void Decode_LDA_ZeroX();
    void Decode_LDA_Abs();
    void Decode_LDA_AbsX();
    void Decode_LDA_AbsY();
    void Decode_LDA_IndX();
    void Decode_LDA_IndY();

    void Decode_LDX(u16 addr);
    void Decode_LDX_Imm();
    void Decode_LDX_Zero();
    void Decode_LDX_ZeroY();
    void Decode_LDX_Abs();
    void Decode_LDX_AbsY();

    void Decode_LDY(u16 addr);
    void Decode_LDY_Imm();
    void Decode_LDY_Zero();
    void Decode_LDY_ZeroX();
    void Decode_LDY_Abs();
    void Decode_LDY_AbsX();

    void Decode_LSR(u16 addr);
    void Decode_LSR_Imp();
    void Decode_LSR_Zero();
    void Decode_LSR_ZeroX();
    void Decode_LSR_Abs();
    void Decode_LSR_AbsX();

    void Decode_ORA(u8 M);
    void Decode_ORA_Imm();
    void Decode_ORA_Zero();
    void Decode_ORA_ZeroX();
    void Decode_ORA_Abs();
    void Decode_ORA_AbsX();
    void Decode_ORA_AbsY();
    void Decode_ORA_IndX();
    void Decode_ORA_IndY();

    void Decode_ROL(u16 addr);
    void Decode_ROL_Imp();
    void Decode_ROL_Zero();
    void Decode_ROL_ZeroX();
    void Decode_ROL_Abs();
    void Decode_ROL_AbsX();

    void Decode_ROR(u16 addr);
    void Decode_ROR_Imp();
    void Decode_ROR_Zero();
    void Decode_ROR_ZeroX();
    void Decode_ROR_Abs();
    void Decode_ROR_AbsX();

    void Decode_SBC(u8 M);
    void Decode_SBC_Imm();
    void Decode_SBC_Zero();
    void Decode_SBC_ZeroX();
    void Decode_SBC_Abs();
    void Decode_SBC_AbsX();
    void Decode_SBC_AbsY();
    void Decode_SBC_IndX();
    void Decode_SBC_IndY();

    void Decode_STA_Zero();
    void Decode_STA_ZeroX();
    void Decode_STA_Abs();
    void Decode_STA_AbsX();
    void Decode_STA_AbsY();
    void Decode_STA_IndX();
    void Decode_STA_IndY();

    void Decode_STX_Zero();
    void Decode_STX_ZeroY();
    void Decode_STX_Abs();

    void Decode_STY_Zero();
    void Decode_STY_ZeroX();
    void Decode_STY_Abs();

    void Decode_DEY();
    void Decode_RTS();
};

