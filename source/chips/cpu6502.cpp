#include "common.h"
#include "cpu6502.h"

//=== Addressing mode decodes

u16 Cpu6502::Decode_ZeroX_Addr()
{
    return (m_regs.operand + m_regs.X) & 0xff;
}
u16 Cpu6502::Decode_ZeroY_Addr()
{
    return (m_regs.operand + m_regs.Y)&0xff;
}
u16 Cpu6502::Decode_Abs_Addr()
{
    return MemReadByte(m_regs.operand);
}
u16 Cpu6502::Decode_AbsX_Addr() 
{
    u16 addr = (m_regs.operand + m_regs.X) & 0xffff;
    m_regs.delayCycle = (addr & 0xff) < m_regs.X;
    return addr;
}
u16 Cpu6502::Decode_AbsY_Addr()
{
    u16 addr = (m_regs.operand + m_regs.Y) & 0xffff;
    m_regs.delayCycle = (addr & 0xff) < m_regs.Y;
    return addr;
}
u16 Cpu6502::Decode_Ind_Addr()
{
    return (u16)MemReadByte(m_regs.operand) | ((u16)MemReadByte((m_regs.operand + 1)&0xff) << 8);
}
u16 Cpu6502::Decode_IndX_Addr()
{
    u8 indAddr = (m_regs.operand + m_regs.X) & 0xff;
    return (u16)MemReadByte(indAddr) | ((u16)MemReadByte((indAddr + 1) & 0xff) << 8);
}
u16 Cpu6502::Decode_IndY_Addr()
{
    u8 indAddr = (u8)m_regs.operand;
    u16 addr = (((u16)MemReadByte(indAddr) | ((u16)MemReadByte((indAddr + 1) & 0xff) << 8)) + m_regs.Y) & 0xffff;
    m_regs.delayCycle = (addr & 0xff) < m_regs.Y;
    return MemReadByte(addr);
}

//=== Opcode decodes

void Cpu6502::Decode_SEC()
{
    m_regs.SR |= SR_Carry;
}

void Cpu6502::Decode_CLC()
{
    m_regs.SR &= ~SR_Carry;
}

void Cpu6502::Decode_TAX()
{
    m_regs.X = m_regs.A;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_TAY()
{
    m_regs.Y = m_regs.A;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_TSX()
{
    m_regs.X = m_regs.SP;
    m_regs.SetNZ(m_regs.SP);
}

void Cpu6502::Decode_TXA()
{
    m_regs.A = m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_TXS()
{
    m_regs.SP = m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_TYA()
{
    m_regs.SP = m_regs.Y;
    m_regs.SetNZ(m_regs.Y);
}

void Cpu6502::Decode_ADC(u8 M)
{
    u16 result = (u16)m_regs.A + M + (m_regs.SR & 1);
    u8 N = result & SR_Negative;
    u8 Z = (result & 0xff) ? 0 : SR_Zero;
    u8 V = ((m_regs.A ^ result) & (m_regs.operand ^ result)) & 0x80 ? SR_Overflow : 0;
    u8 C = result & 0x100 ? 1 : 0;
    m_regs.A = (u8)result;
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry | SR_Overflow)) | N | Z | V | C;
}

void Cpu6502::Decode_ADC_Imm()
{
    Decode_ADC(m_regs.operand);
}

void Cpu6502::Decode_ADC_Zero()
{
    Decode_ADC(MemReadByte(m_regs.operand));
}

void Cpu6502::Decode_ADC_ZeroX()
{
    Decode_ADC(MemReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_ADC_Abs()
{
    Decode_ADC(MemReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_ADC_AbsX()
{
    Decode_ADC(MemReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_ADC_AbsY()
{
    Decode_ADC(MemReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_ADC_IndX()
{
    Decode_ADC(MemReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_ADC_IndY()
{
    Decode_ADC(MemReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_AND(u8 M)
{
    m_regs.A &= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_AND_Imm()
{
    Decode_AND(m_regs.operand);
}

void Cpu6502::Decode_AND_Zero()
{
    Decode_AND(MemReadByte(m_regs.operand));
}

void Cpu6502::Decode_AND_ZeroX()
{
    Decode_AND(MemReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_AND_Abs()
{
    Decode_AND(MemReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_AND_AbsX()
{
    Decode_AND(MemReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_AND_AbsY()
{
    Decode_AND(MemReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_AND_IndX()
{
    Decode_AND(MemReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_AND_IndY()
{
    Decode_AND(MemReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_EOR(u8 M)
{
    m_regs.A ^= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_EOR_Imm()
{
    Decode_EOR(m_regs.operand);
}

void Cpu6502::Decode_EOR_Zero()
{
    Decode_EOR(MemReadByte(m_regs.operand));
}

void Cpu6502::Decode_EOR_ZeroX()
{
    Decode_EOR(MemReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_EOR_Abs()
{
    Decode_EOR(MemReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_EOR_AbsX()
{
    Decode_EOR(MemReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_EOR_AbsY()
{
    Decode_EOR(MemReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_EOR_IndX()
{
    Decode_EOR(MemReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_EOR_IndY()
{
    Decode_EOR(MemReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_ORA(u8 M)
{
    m_regs.A |= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_ORA_Imm()
{
    Decode_ORA(m_regs.operand);
}

void Cpu6502::Decode_ORA_Zero()
{
    Decode_ORA(MemReadByte(m_regs.operand));
}

void Cpu6502::Decode_ORA_ZeroX()
{
    Decode_ORA(MemReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_ORA_Abs()
{
    Decode_ORA(MemReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_ORA_AbsX()
{
    Decode_ORA(MemReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_ORA_AbsY()
{
    Decode_ORA(MemReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_ORA_IndX()
{
    Decode_ORA(MemReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_ORA_IndY()
{
    Decode_ORA(MemReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_LDA_Imm()
{
    m_regs.A = (u8)m_regs.operand;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_Zero()
{
    m_regs.A = MemReadByte(m_regs.operand);
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_ZeroX()
{
    m_regs.A = MemReadByte(Decode_ZeroX_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_Abs()
{
    m_regs.A = MemReadByte(Decode_Abs_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_AbsX()
{
    m_regs.A = MemReadByte(Decode_Abs_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_AbsY()
{
    m_regs.A = MemReadByte(Decode_AbsY_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_IndX()
{
    m_regs.A = MemReadByte(Decode_IndX_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_IndY()
{
    m_regs.A = MemReadByte(Decode_IndY_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_STA_Zero()
{
    MemWriteByte(m_regs.operand, m_regs.A);
}

void Cpu6502::Decode_STA_ZeroX()
{
    MemWriteByte(Decode_ZeroX_Addr()), m_regs.A);
}

void Cpu6502::Decode_STA_Abs()
{
    MemWriteByte(Decode_Abs_Addr()), m_regs.A);
}

void Cpu6502::Decode_STA_AbsX()
{
    MemWriteByte(Decode_AbsX_Addr()), m_regs.A);
}

void Cpu6502::Decode_STA_AbsY()
{
    MemWriteByte(Decode_AbsY_Addr()), m_regs.A);
}

void Cpu6502::Decode_STA_IndX()
{
    MemWriteByte(Decode_IndX_Addr()), m_regs.A);
}

void Cpu6502::Decode_STA_IndY()
{
    MemWriteByte(Decode_IndY_Addr()), m_regs.A);
}


void Cpu6502::Decode_LDX_Imm()
{
    m_regs.X = (u8)m_regs.operand;
    u8 N = (m_regs.operand & SR_Negative);
    u8 Z = (m_regs.operand ? 0 : SR_Zero);
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
    return false;
}
void Cpu6502::Decode_STX_Abs()
{
    u16 M = m_regs.operand;
    e->SetByte(M, m_regs.X);
    return false;
}
void Cpu6502::Decode_STX_Zero()
{
    u16 M = m_regs.operand;
    e->SetByte(M, m_regs.X);
    return false;
}
void Cpu6502::Decode_STX_ZeroY()
{
    u16 M = m_regs.operand + m_regs.Y;
    e->SetByte(M, m_regs.X);
    return false;
}

void Cpu6502::Decode_LDY_Imm()
{
    m_regs.Y = (u8)m_regs.operand;
    u8 N = (m_regs.operand & SR_Negative);
    u8 Z = (m_regs.operand ? 0 : SR_Zero);
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
    return false;
}
void Cpu6502::Decode_DEX()
{
    m_regs.X = m_regs.X - 1;
    u8 N = (m_regs.X & SR_Negative);
    u8 Z = (m_regs.X ? 0 : SR_Zero);
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
    return false;
}
void Cpu6502::Decode_DEY()
{
    m_regs.Y = m_regs.Y - 1;
    u8 N = (m_regs.Y & SR_Negative);
    u8 Z = (m_regs.Y ? 0 : SR_Zero);
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative)) | N | Z;
    return false;
}
void Cpu6502::Decode_BNE()
{
    if (m_regs.SR & SR_Zero)
    {
        m_regs.PC += (i8)m_regs.operand;
        return true;
    }
    return false;
}
void Cpu6502::Decode_RTS()
{
    u16 addr = e->GetByte(0x101 + m_regs.SP);
    addr = addr | (e->GetByte(0x102 + m_regs.SP) << 8);
    m_regs.SP += 2;
    m_regs.PC = addr + 1;
    return false;
}

Cpu6502::Opcode gOpcodesRaw[] =
{
    {"ADC", Cpu6502::AM_Immediate, 0x69, 2},
    {"ADC", Cpu6502::AM_ZeroPage,  0x65, 3},
    {"ADC", Cpu6502::AM_ZeroPageX, 0x75, 4},
    {"ADC", Cpu6502::AM_Absolute,  0x6D, 4},
    {"ADC", Cpu6502::AM_AbsoluteX, 0x7D, 4},
    {"ADC", Cpu6502::AM_AbsoluteY, 0x79, 4},
    {"ADC", Cpu6502::AM_IndirectX, 0x61, 6},
    {"ADC", Cpu6502::AM_IndirectY, 0x71, 5, true, false, &Decode_ADC_IndY},

    {"AND", Cpu6502::AM_Immediate, 0x29, 2, false, false, &Decode_AND_Imm},
    {"AND", Cpu6502::AM_ZeroPage,  0x25, 3, false, false, &Decode_AND_Zero},
    {"AND", Cpu6502::AM_ZeroPageX, 0x35, 4, false, false, &Decode_AND_ZeroX},
    {"AND", Cpu6502::AM_Absolute,  0x2D, 4, false, false, &Decode_AND_Abs},
    {"AND", Cpu6502::AM_AbsoluteX, 0x3D, 4, true, false, &Decode_AND_AbsX},
    {"AND", Cpu6502::AM_AbsoluteY, 0x39, 4, true, false, &Decode_AND_AbsY},
    {"AND", Cpu6502::AM_IndirectX, 0x21, 6, false, false, &Decode_AND_IndX},
    {"AND", Cpu6502::AM_IndirectY, 0x31, 5, true, false, &Decode_AND_IndY},

    {"ASL", Cpu6502::AM_Implied,   0x0A, 2},
    {"ASL", Cpu6502::AM_ZeroPage,  0x06, 5},
    {"ASL", Cpu6502::AM_ZeroPageX, 0x16, 6},
    {"ASL", Cpu6502::AM_Absolute,  0x0E, 6},
    {"ASL", Cpu6502::AM_AbsoluteX, 0x1E, 7},

    {"BCC", Cpu6502::AM_Relative,  0x90, 2, false, true},
    {"BCS", Cpu6502::AM_Relative,  0xB0, 2, false, true},
    {"BEQ", Cpu6502::AM_Relative,  0xF0, 2, false, true},

    {"BIT", Cpu6502::AM_ZeroPage,  0x24, 3},
    {"BIT", Cpu6502::AM_Absolute,  0x2C, 4},

    {"BMI", Cpu6502::AM_Relative,  0x30, 2, false, true},
    {"BNE", Cpu6502::AM_Relative,  0xD0, 2, false, true, &Decode_BNE},
    {"BPL", Cpu6502::AM_Relative,  0x10, 2, false, true},

    {"BRK", Cpu6502::AM_Implied,   0x30, 7},

    {"BVC", Cpu6502::AM_Relative,  0x50, 2, false, true},
    {"BVS", Cpu6502::AM_Relative,  0x70, 2, false, true},

    {"CLC", Cpu6502::AM_Implied,   0x18, 2, false, false, &Decode_SEC},
    {"CLD", Cpu6502::AM_Implied,   0xD8, 2},
    {"CLI", Cpu6502::AM_Implied,   0x58, 2},
    {"CLV", Cpu6502::AM_Implied,   0xB8, 2},

    {"CMP", Cpu6502::AM_Immediate, 0xC9, 2},
    {"CMP", Cpu6502::AM_ZeroPage,  0xC5, 3},
    {"CMP", Cpu6502::AM_ZeroPageX, 0xD5, 4},
    {"CMP", Cpu6502::AM_Absolute,  0xCD, 4},
    {"CMP", Cpu6502::AM_AbsoluteX, 0xDD, 4, true},
    {"CMP", Cpu6502::AM_AbsoluteY, 0xD9, 4, true},
    {"CMP", Cpu6502::AM_IndirectX, 0xC1, 6},
    {"CMP", Cpu6502::AM_IndirectY, 0xD1, 5, true},

    {"CPX", Cpu6502::AM_Immediate, 0xE0, 2},
    {"CPX", Cpu6502::AM_ZeroPage,  0xE4, 3},
    {"CPX", Cpu6502::AM_Absolute,  0xEC, 4},

    {"CPY", Cpu6502::AM_Immediate, 0xC0, 2},
    {"CPY", Cpu6502::AM_ZeroPage,  0xC4, 3},
    {"CPY", Cpu6502::AM_Absolute,  0xCC, 4},

    {"DEC", Cpu6502::AM_ZeroPage,  0xC6, 5},
    {"DEC", Cpu6502::AM_ZeroPageX, 0xD6, 6},
    {"DEC", Cpu6502::AM_Absolute,  0xCE, 6},
    {"DEC", Cpu6502::AM_AbsoluteX, 0xDE, 7},

    {"DEX", Cpu6502::AM_Implied,   0xCA, 2, false, false, &Decode_DEX},
    {"DEY", Cpu6502::AM_Implied,   0x88, 2, false, false, &Decode_DEY},

    {"EOR", Cpu6502::AM_Immediate, 0x49, 2, false, false, &Decode_EOR_Imm},
    {"EOR", Cpu6502::AM_ZeroPage,  0x45, 3, false, false, &Decode_EOR_Zero},
    {"EOR", Cpu6502::AM_ZeroPageX, 0x55, 4, false, false, &Decode_EOR_ZeroX},
    {"EOR", Cpu6502::AM_Absolute,  0x4D, 4, false, false, &Decode_EOR_Abs},
    {"EOR", Cpu6502::AM_AbsoluteX, 0x5D, 4, true, false, &Decode_EOR_AbsX},
    {"EOR", Cpu6502::AM_AbsoluteY, 0x59, 4, true, false, &Decode_EOR_AbsY},
    {"EOR", Cpu6502::AM_IndirectX, 0x41, 6, false, false, &Decode_EOR_IndX},
    {"EOR", Cpu6502::AM_IndirectY, 0x51, 5, true, false, &Decode_EOR_IndY},

    {"INC", Cpu6502::AM_ZeroPage,  0xE6, 5},
    {"INC", Cpu6502::AM_ZeroPageX, 0xF6, 6},
    {"INC", Cpu6502::AM_Absolute,  0xEE, 6},
    {"INC", Cpu6502::AM_AbsoluteX, 0xFE, 7},

    {"INX", Cpu6502::AM_Implied,   0xE8, 2},
    {"INY", Cpu6502::AM_Implied,   0xC8, 2},

    {"JMP", Cpu6502::AM_Absolute,  0x4C, 3},
    {"JMP", Cpu6502::AM_Indirect,  0x6C, 5},

    {"JSR", Cpu6502::AM_Absolute,  0x20, 6},

    {"LDA", Cpu6502::AM_Immediate, 0xA9, 2, false, false, &Decode_LDA_Imm},
    {"LDA", Cpu6502::AM_ZeroPage,  0xA5, 3},
    {"LDA", Cpu6502::AM_ZeroPageX, 0xB5, 4},
    {"LDA", Cpu6502::AM_Absolute,  0xAD, 4},
    {"LDA", Cpu6502::AM_AbsoluteX, 0xBD, 4, true},
    {"LDA", Cpu6502::AM_AbsoluteY, 0xB9, 4, true},
    {"LDA", Cpu6502::AM_IndirectX, 0xA1, 6},
    {"LDA", Cpu6502::AM_IndirectY, 0xB1, 5, true},

    {"LDX", Cpu6502::AM_Immediate, 0xA2, 2, false, false, &Decode_LDX_Imm},
    {"LDX", Cpu6502::AM_ZeroPage,  0xA6, 3},
    {"LDX", Cpu6502::AM_ZeroPageY, 0xB6, 4},
    {"LDX", Cpu6502::AM_Absolute,  0xAE, 4},
    {"LDX", Cpu6502::AM_AbsoluteY, 0xBE, 4, true},

    {"LDY", Cpu6502::AM_Immediate, 0xA0, 2, false, false, &Decode_LDY_Imm },
    {"LDY", Cpu6502::AM_ZeroPage,  0xA4, 3},
    {"LDY", Cpu6502::AM_ZeroPageX, 0xB4, 4},
    {"LDY", Cpu6502::AM_Absolute,  0xAC, 4},
    {"LDY", Cpu6502::AM_AbsoluteX, 0xBC, 4, true},

    {"LSR", Cpu6502::AM_Implied,   0x4A, 2},
    {"LSR", Cpu6502::AM_ZeroPage,  0x46, 5},
    {"LSR", Cpu6502::AM_ZeroPageX, 0x56, 6},
    {"LSR", Cpu6502::AM_Absolute,  0x4E, 6},
    {"LSR", Cpu6502::AM_AbsoluteX, 0x5E, 7},

    {"NOP", Cpu6502::AM_Implied,   0xEA, 2},

    {"ORA", Cpu6502::AM_Immediate, 0x09, 2},
    {"ORA", Cpu6502::AM_ZeroPage,  0x05, 3},
    {"ORA", Cpu6502::AM_ZeroPageX, 0x15, 4},
    {"ORA", Cpu6502::AM_Absolute,  0x0D, 4},
    {"ORA", Cpu6502::AM_AbsoluteX, 0x1D, 4, true},
    {"ORA", Cpu6502::AM_AbsoluteY, 0x19, 4, true},
    {"ORA", Cpu6502::AM_IndirectX, 0x01, 6},
    {"ORA", Cpu6502::AM_IndirectY, 0x11, 5, true},

    {"PHA", Cpu6502::AM_Implied,   0x48, 3},
    {"PHP", Cpu6502::AM_Implied,   0x08, 3},
    {"PLA", Cpu6502::AM_Implied,   0x68, 4},
    {"PLP", Cpu6502::AM_Implied,   0x28, 4},

    {"ROR", Cpu6502::AM_Immediate, 0x6A, 2},
    {"ROR", Cpu6502::AM_ZeroPage,  0x66, 5},
    {"ROR", Cpu6502::AM_ZeroPageX, 0x76, 6},
    {"ROR", Cpu6502::AM_Absolute,  0x4E, 6},
    {"ROR", Cpu6502::AM_AbsoluteX, 0x7E, 7},

    {"RTI", Cpu6502::AM_Implied,   0x40, 6},
    {"RTS", Cpu6502::AM_Implied,   0x60, 6, false, false, &Decode_RTS},

    { "SBC", Cpu6502::AM_Immediate, 0xE9, 2},
    { "SBC", Cpu6502::AM_ZeroPage,  0xE5, 3},
    { "SBC", Cpu6502::AM_ZeroPageX, 0xF5, 4},
    { "SBC", Cpu6502::AM_Absolute,  0xED, 4},
    { "SBC", Cpu6502::AM_AbsoluteX, 0xFD, 4, true},
    { "SBC", Cpu6502::AM_AbsoluteY, 0xF9, 4, true},
    { "SBC", Cpu6502::AM_IndirectX, 0xE1, 6},
    { "SBC", Cpu6502::AM_IndirectY, 0xF1, 5, true},

    { "SEC", Cpu6502::AM_Implied,   0x38, 2, false, false, &Decode_SEC},
    { "SED", Cpu6502::AM_Implied,   0xF8, 2 },
    { "SEI", Cpu6502::AM_Implied,   0x78, 2 },

    { "STA", Cpu6502::AM_ZeroPage,  0x85, 3, false, false, &Decode_STA_Zero},
    { "STA", Cpu6502::AM_ZeroPageX, 0x95, 4 },
    { "STA", Cpu6502::AM_Absolute,  0x8D, 4 },
    { "STA", Cpu6502::AM_AbsoluteX, 0x9D, 5 },
    { "STA", Cpu6502::AM_AbsoluteY, 0x99, 5 },
    { "STA", Cpu6502::AM_IndirectX, 0x81, 6 },
    { "STA", Cpu6502::AM_IndirectY, 0x91, 6 },

    { "STX", Cpu6502::AM_ZeroPage,  0x86, 3 , false, false, &Decode_STX_Zero },
    { "STX", Cpu6502::AM_ZeroPageY, 0x96, 4 , false, false, &Decode_STX_ZeroY },
    { "STX", Cpu6502::AM_Absolute,  0x8E, 4 , false, false, &Decode_STX_Abs },

    { "STY", Cpu6502::AM_ZeroPage,  0x84, 3 },
    { "STY", Cpu6502::AM_ZeroPageX, 0x94, 4 },
    { "STY", Cpu6502::AM_Absolute,  0x8C, 4 },

    { "TAX", Cpu6502::AM_Implied,   0xAA, 3, false, false, &Decode_TAX},
    { "TAY", Cpu6502::AM_Implied,   0xA8, 3, false, false, &Decode_TAY},
    { "TSX", Cpu6502::AM_Implied,   0xBA, 3, false, false, &Decode_TSX},
    { "TXA", Cpu6502::AM_Implied,   0x8A, 3, false, false, &Decode_TXA},
    { "TXS", Cpu6502::AM_Implied,   0x9A, 3, false, false, &Decode_TXS},
    { "TYA", Cpu6502::AM_Implied,   0x98, 3, false, false, &Decode_TYA}
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

Cpu6502::Cpu6502()
{
    memset(m_opcodes, 0, sizeof(m_opcodes));

    // copy opcode data to indexed table - we'll need this for fast emulation
    int cnt = sizeof(gOpcodesRaw) / sizeof(Opcode);
    for (int i = 0; i < cnt; i++)
    {
        m_opcodes[gOpcodesRaw[i].opc] = gOpcodesRaw[i];

        if ((i == 0) || (gOpcodesRaw[i - 1].name != gOpcodesRaw[i].name))
            m_uniqueOpcodes.push_back(&gOpcodesRaw[i]);
    }
}

void Cpu6502::Reset(u16 cpuStart)
{
    memset(&m_regs, 0, sizeof(m_regs));
    m_regs.PC = cpuStart;
    m_regs.SP = 0xff;
}

bool Cpu6502::IsOpcode(const char* text)
{
    for (auto opcode : m_uniqueOpcodes)
    {
        if (StrEqual(text, opcode->name))
        {
            return true;
        }
    }
    return false;
}

Cpu6502::Opcode* Cpu6502::FindOpcode(const string& name, AddressingMode am)
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

bool Cpu6502::Step()
{
    m_regs.frameCycle++;
    if (m_regs.delayCycle)
    {
        m_regs.delayCycle = false;
        return true;
    }
    else
    {
        if (m_regs.decodeCycle == 0)
        {
            u8 op = m_mem->ReadByte(m_regs.PC++);
            m_co = &gOpcodes[op];
            if (m_co && m_co->cycles > 0)
            {
                m_opcodeCycleCount = m_co->cycles;
            }
        }
        else if (m_decodeCycle == 1 && m_co->addressMode >= AM_Immediate)
        {
            m_regs.operand = GetByte(m_regs.PC++);
        }
        else if (m_decodeCycle == 2 && m_co->addressMode >= AM_Absolute)
        {
            m_regs.operand = m_regs.operand | ((u16)GetByte(m_regs.PC++) << 8);
        }

        if (++m_decodeCycle == m_opcodeCycleCount)
        {
            if (m_co->m_decode)
            {
                // execute the instruction,  some instructions can generate and extra delay cycle
                m_delayCycle = m_co->m_decode(this);
            }
            m_decodeCycle = 0;
        }
    }
}

