#include "common.h"
#include "cpu6502.h"

#define OPC( n, am, o, cyc ) m_opcodes[o].name = #n; m_opcodes[o].addressMode = AM_##am;\
     m_opcodes[o].opc = o; m_opcodes[o].cycles = cyc; m_opcodes[o].decode = DELEGATE(Cpu6502::Decode_##n##_##am);

//========================================
//=== Addressing mode decodes
//========================================

u16 Cpu6502::Decode_Zero_Addr()
{
    return m_regs.operand;
}

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
    return m_regs.operand;
}
u16 Cpu6502::Decode_AbsX_Addr() 
{
    u16 addr = (m_regs.operand + m_regs.X) & 0xffff;
    m_regs.delayCycles = ((addr & 0xff) < m_regs.X) ? 1 : 0;
    return addr;
}
u16 Cpu6502::Decode_AbsY_Addr()
{
    u16 addr = (m_regs.operand + m_regs.Y) & 0xffff;
    m_regs.delayCycles = ((addr & 0xff) < m_regs.Y) ? 1 : 0;
    return addr;
}
u16 Cpu6502::Decode_Ind_Addr()
{
    return (u16)ReadByte(m_regs.operand) | ((u16)ReadByte(m_regs.operand + 1) << 8);
}
u16 Cpu6502::Decode_IndX_Addr()
{
    u8 indAddr = (m_regs.operand + m_regs.X) & 0xff;
    return (u16)ReadByte(indAddr) | ((u16)ReadByte((indAddr + 1) & 0xff) << 8);
}
u16 Cpu6502::Decode_IndY_Addr()
{
    u8 indAddr = (u8)m_regs.operand;
    u16 addr = (((u16)ReadByte(indAddr) | ((u16)ReadByte((indAddr + 1) & 0xff) << 8)) + m_regs.Y) & 0xffff;
    m_regs.delayCycles = ((addr & 0xff) < m_regs.Y) ? 1 : 0;
    return addr;
}

//========================================
//=== Opcode decodes
//========================================

void Cpu6502::Decode_Unknown()
{
    // just do nothing and return
}

void Cpu6502::Decode_SEC_Imp()
{
    m_regs.SR |= SR_Carry;
}

void Cpu6502::Decode_CLC_Imp()
{
    m_regs.SR &= ~SR_Carry;
}

void Cpu6502::Decode_SEI_Imp()
{
    m_regs.SR |= SR_Interrupt;
}

void Cpu6502::Decode_CLI_Imp()
{
    m_regs.SR &= ~SR_Interrupt;
    m_interruptDelay = 2;
}

void Cpu6502::Decode_CLV_Imp()
{
    m_regs.SR &= ~SR_Overflow;
}

void Cpu6502::Decode_DEX_Imp()
{
    --m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_DEY_Imp()
{
    --m_regs.Y;
    m_regs.SetNZ(m_regs.Y);
}

void Cpu6502::Decode_INX_Imp()
{
    ++m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_INY_Imp()
{
    ++m_regs.Y;
    m_regs.SetNZ(m_regs.Y);
}

void Cpu6502::Decode_SED_Imp()
{
    m_regs.SR |= SR_Decimal;
}

void Cpu6502::Decode_CLD_Imp()
{
    m_regs.SR &= ~SR_Decimal;
}

void Cpu6502::Decode_NOP_Imp()
{
}

void Cpu6502::Decode_PHA_Imp()
{
    WriteByte(0x100 + m_regs.SP--, m_regs.A);
}

void Cpu6502::Decode_PHP_Imp()
{
    WriteByte(0x100 + m_regs.SP--, m_regs.SR);
}

void Cpu6502::Decode_PLA_Imp()
{
    m_regs.A = ReadByte(++m_regs.SP + 0x100);
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_PLP_Imp()
{
    m_regs.SR = ReadByte(++m_regs.SP + 0x100);
}

void Cpu6502::Decode_TAX_Imp()
{
    m_regs.X = m_regs.A;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_TAY_Imp()
{
    m_regs.Y = m_regs.A;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_TSX_Imp()
{
    m_regs.X = m_regs.SP;
    m_regs.SetNZ(m_regs.SP);
}

void Cpu6502::Decode_TXA_Imp()
{
    m_regs.A = m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_TXS_Imp()
{
    m_regs.SP = m_regs.X;
    m_regs.SetNZ(m_regs.X);
}

void Cpu6502::Decode_TYA_Imp()
{
    m_regs.A = m_regs.Y;
    m_regs.SetNZ(m_regs.Y);
}

void Cpu6502::Decode_BRK_Imp()
{
    u16 ret = (u16)(m_regs.PC+1);
    WriteByte(0x100 + m_regs.SP--, (u8)(ret>>8));
    WriteByte(0x100 + m_regs.SP--, (u8)ret);
    WriteByte(0x100 + m_regs.SP--, m_regs.SR|SR_Break);
    m_regs.SR |= SR_Interrupt;
    u16 addr = (u16)ReadByte(0xfffe) | ((u16)ReadByte(0xffff) << 8);
}

void Cpu6502::Decode_RTI_Imp()
{
    u8 sr = ReadByte(0x100 + ++m_regs.SP);
    u16 addr = ReadByte(0x100 + ++m_regs.SP) | (ReadByte(0x100 + ++m_regs.SP) << 8);
    m_regs.SR = (m_regs.SR & SR_Break) | (sr & ~SR_Break);
    m_regs.PC = addr;
}

void Cpu6502::Decode_RTS_Imp()
{
    u16 addr = ReadByte(0x101 + m_regs.SP);
    addr = addr | (ReadByte(0x102 + m_regs.SP) << 8);
    m_regs.SP += 2;
    m_regs.PC = addr + 1;
}

void Cpu6502::Decode_Branch()
{
    u16 srcPage = m_regs.PC & 0xff00;
    m_regs.PC = (u16)((int)m_regs.PC + (i8)m_regs.operand);
    m_regs.delayCycles = (srcPage != (m_regs.PC & 0xff00)) ? 2 : 1;
}

void Cpu6502::Decode_BCC_Rel()
{
    if (!(m_regs.SR & SR_Carry))
        Decode_Branch();
}
void Cpu6502::Decode_BCS_Rel()
{
    if (m_regs.SR & SR_Carry)
        Decode_Branch();
}
void Cpu6502::Decode_BEQ_Rel()
{
    if (m_regs.SR & SR_Zero)
        Decode_Branch();
}
void Cpu6502::Decode_BNE_Rel()
{
    if (!(m_regs.SR & SR_Zero))
        Decode_Branch();
}
void Cpu6502::Decode_BMI_Rel()
{
    if (m_regs.SR & SR_Negative)
        Decode_Branch();
}
void Cpu6502::Decode_BPL_Rel()
{
    if (!(m_regs.SR & SR_Negative))
        Decode_Branch();
}
void Cpu6502::Decode_BVC_Rel()
{
    if (!(m_regs.SR & SR_Overflow))
        Decode_Branch();
}
void Cpu6502::Decode_BVS_Rel()
{
    if (m_regs.SR & SR_Overflow)
        Decode_Branch();
}

void Cpu6502::Decode_ADC(u8 M)
{
    if (m_regs.SR & SR_Decimal)
    {
        u8 low1 = m_regs.A & 0xf;
        u8 high1 = (m_regs.A >> 4) & 0xf;
        u8 low2 = M & 0xf;
        u8 high2 = (M >> 4) & 0xf;
        u8 outLow = low1 + low2 + (m_regs.SR & 1);
        u8 outHi = high1 + high2;
        if (outLow >= 10)
        {
            outLow = outLow - 10;
            outHi++;
        }
        u8 C = 0;
        if (outHi >= 10)
        {
            outHi -= 10;
            C = SR_Carry;
        }
        u8 result = outLow | (outHi << 4);
        u8 N = 0;
        u8 Z = result ? 0 : SR_Zero;
        u8 V = 0;
        m_regs.A = result;
        m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry | SR_Overflow)) | N | Z | V | C;
    }
    else
    {
        u16 result = (u16)m_regs.A + M + (m_regs.SR & 1);
        u8 N = result & SR_Negative;
        u8 Z = (result & 0xff) ? 0 : SR_Zero;
        u8 C = result & 0x100 ? 1 : 0;
        u8 V = ((m_regs.A ^ result) & (m_regs.operand ^ result)) & 0x80 ? SR_Overflow : 0;
        m_regs.A = (u8)result;
        m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry | SR_Overflow)) | N | Z | V | C;
    }
}

void Cpu6502::Decode_ADC_Imm()
{
    Decode_ADC((u8)m_regs.operand);
}

void Cpu6502::Decode_ADC_Zero()
{
    Decode_ADC(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_ADC_ZeroX()
{
    Decode_ADC(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_ADC_Abs()
{
    Decode_ADC(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_ADC_AbsX()
{
    Decode_ADC(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_ADC_AbsY()
{
    Decode_ADC(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_ADC_IndX()
{
    Decode_ADC(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_ADC_IndY()
{
    Decode_ADC(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_CMP(u8 M)
{
    u16 result = (u16)m_regs.A - M;
    u8 N = result & SR_Negative;
    u8 Z = (result & 0xff) ? 0 : SR_Zero;
    u8 C = result & 0x8000 ? 0 : 1;
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry)) | N | Z | C;
}

void Cpu6502::Decode_CMP_Imm()
{
    Decode_CMP((u8)m_regs.operand);
}

void Cpu6502::Decode_CMP_Zero()
{
    Decode_CMP(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_CMP_ZeroX()
{
    Decode_CMP(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_CMP_Abs()
{
    Decode_CMP(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_CMP_AbsX()
{
    Decode_CMP(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_CMP_AbsY()
{
    Decode_CMP(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_CMP_IndX()
{
    Decode_CMP(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_CMP_IndY()
{
    Decode_CMP(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_CPX(u8 M)
{
    u16 result = (u16)m_regs.X - M;
    u8 N = result & SR_Negative;
    u8 Z = (result & 0xff) ? 0 : SR_Zero;
    u8 C = result & 0x8000 ? 0 : 1;
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry)) | N | Z | C;
}

void Cpu6502::Decode_CPX_Imm()
{
    Decode_CPX((u8)m_regs.operand);
}

void Cpu6502::Decode_CPX_Zero()
{
    Decode_CPX(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_CPX_Abs()
{
    Decode_CPX(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_CPY(u8 M)
{
    u16 result = (u16)m_regs.Y - M;
    u8 N = result & SR_Negative;
    u8 Z = (result & 0xff) ? 0 : SR_Zero;
    u8 C = result & 0x8000 ? 0 : 1;
    m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry)) | N | Z | C;
}

void Cpu6502::Decode_CPY_Imm()
{
    Decode_CPY((u8)m_regs.operand);
}

void Cpu6502::Decode_CPY_Zero()
{
    Decode_CPY(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_CPY_Abs()
{
    Decode_CPY(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_SBC(u8 M)
{
    if (m_regs.SR & SR_Decimal)
    {
        u8 low1 = m_regs.A & 0xf;
        u8 high1 = (m_regs.A >> 4) & 0xf;
        u8 low2 = M & 0xf;
        u8 high2 = (M >> 4) & 0xf;
        int outLow = low1 - low2 - (1 ^ (m_regs.SR & SR_Carry));
        int outHi = high1 - high2;
        if (outLow < 0)
        {
            outLow = outLow + 10;
            outHi--;
        }
        u8 C = 0;
        if (outHi < 0)
        {
            outHi += 10;
            C = SR_Carry;
        }
        u8 result = outLow | (outHi << 4);
        u8 N = 0;
        u8 Z = result ? 0 : SR_Zero;
        u8 V = 0;
        m_regs.A = result;
        m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry | SR_Overflow)) | N | Z | V | C;
    }
    else
    {
        u16 result = (u16)m_regs.A - M - (1^(m_regs.SR & SR_Carry));
        u8 N = result & SR_Negative;
        u8 Z = (result & 0xff) ? 0 : SR_Zero;
        u8 V = ((m_regs.A ^ result) & (m_regs.operand ^ result)) & 0x80 ? SR_Overflow : 0;
        u8 C = result & 0x100 ? 0 : 1;
        m_regs.A = (u8)result;
        m_regs.SR = (m_regs.SR & ~(SR_Zero | SR_Negative | SR_Carry | SR_Overflow)) | N | Z | V | C;
    }
}

void Cpu6502::Decode_SBC_Imm()
{
    Decode_SBC((u8)m_regs.operand);
}

void Cpu6502::Decode_SBC_Zero()
{
    Decode_SBC(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_SBC_ZeroX()
{
    Decode_SBC(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_SBC_Abs()
{
    Decode_SBC(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_SBC_AbsX()
{
    Decode_SBC(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_SBC_AbsY()
{
    Decode_SBC(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_SBC_IndX()
{
    Decode_SBC(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_SBC_IndY()
{
    Decode_SBC(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_AND(u8 M)
{
    m_regs.A &= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_AND_Imm()
{
    Decode_AND((u8)m_regs.operand);
}

void Cpu6502::Decode_AND_Zero()
{
    Decode_AND(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_AND_ZeroX()
{
    Decode_AND(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_AND_Abs()
{
    Decode_AND(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_AND_AbsX()
{
    Decode_AND(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_AND_AbsY()
{
    Decode_AND(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_AND_IndX()
{
    Decode_AND(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_AND_IndY()
{
    Decode_AND(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_ASL(u16 addr)
{
    u8 val = ReadByte(addr);
    u8 newCarry = (val & 0x80) ? SR_Carry : 0;
    val = (u8)((u16)val << 1);
    WriteByte(addr, val);
    m_regs.SetNZC(val, newCarry);
}

void Cpu6502::Decode_ASL_Imp()
{
    u8 newCarry = (m_regs.A & 0x80) ? SR_Carry : 0;
    m_regs.A = (u8)((u16)m_regs.A << 1);
    m_regs.SetNZC(m_regs.A, newCarry);
}

void Cpu6502::Decode_ASL_Zero()
{
    Decode_ASL(Decode_Zero_Addr());
}

void Cpu6502::Decode_ASL_ZeroX()
{
    Decode_ASL(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_ASL_Abs()
{
    Decode_ASL(Decode_Abs_Addr());
}

void Cpu6502::Decode_ASL_AbsX()
{
    Decode_ASL(Decode_AbsX_Addr());
}

void Cpu6502::Decode_BIT(u16 addr)
{
    u8 M = ReadByte(addr);
    u8 Z = (M & m_regs.A) ? 0 : 1;
    m_regs.SR = (m_regs.SR & ~(SR_Negative | SR_Overflow | SR_Zero)) | (M & (SR_Negative | SR_Overflow)) | SR_Zero;
}

void Cpu6502::Decode_BIT_Zero()
{
    Decode_BIT(Decode_Zero_Addr());
}

void Cpu6502::Decode_BIT_Abs()
{
    Decode_BIT(Decode_Abs_Addr());
}

void Cpu6502::Decode_DEC(u16 addr)
{
    u8 val = (ReadByte(addr) - 1) & 0xff;
    WriteByte(addr, val);
    m_regs.SetNZ(val);
}

void Cpu6502::Decode_DEC_Zero()
{
    Decode_DEC(Decode_Zero_Addr());
}

void Cpu6502::Decode_DEC_ZeroX()
{
    Decode_DEC(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_DEC_Abs()
{
    Decode_DEC(Decode_Abs_Addr());
}
void Cpu6502::Decode_DEC_AbsX()
{
    Decode_DEC(Decode_AbsX_Addr());
}

void Cpu6502::Decode_EOR(u8 M)
{
    m_regs.A ^= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_EOR_Imm()
{
    Decode_EOR((u8)m_regs.operand);
}

void Cpu6502::Decode_EOR_Zero()
{
    Decode_EOR(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_EOR_ZeroX()
{
    Decode_EOR(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_EOR_Abs()
{
    Decode_EOR(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_EOR_AbsX()
{
    Decode_EOR(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_EOR_AbsY()
{
    Decode_EOR(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_EOR_IndX()
{
    Decode_EOR(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_EOR_IndY()
{
    Decode_EOR(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_INC(u16 addr)
{
    u8 M = ReadByte(addr);
    M = M + 1;
    m_regs.SetNZ(M);
    WriteByte(addr, M);
}

void Cpu6502::Decode_INC_Zero()
{
    Decode_INC(Decode_Zero_Addr());
}

void Cpu6502::Decode_INC_ZeroX()
{
    Decode_INC(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_INC_Abs()
{
    Decode_INC(Decode_Abs_Addr());
}

void Cpu6502::Decode_INC_AbsX()
{
    Decode_INC(Decode_AbsX_Addr());
}

void Cpu6502::Decode_LSR(u16 addr)
{
    u8 val = ReadByte(addr);
    u8 newCarry = val & 0x01;
    val = (u8)((u16)val >> 1);
    WriteByte(addr, val);
    m_regs.SetNZC(val, newCarry);
}

void Cpu6502::Decode_LSR_Imp()
{
    u8 newCarry = m_regs.A & 0x01;
    m_regs.A = (u8)((u16)m_regs.A >> 1);
    m_regs.SetNZC(m_regs.A, newCarry);
}

void Cpu6502::Decode_LSR_Zero()
{
    Decode_LSR(Decode_Zero_Addr());
}

void Cpu6502::Decode_LSR_ZeroX()
{
    Decode_LSR(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_LSR_Abs()
{
    Decode_LSR(Decode_Abs_Addr());
}

void Cpu6502::Decode_LSR_AbsX()
{
    Decode_LSR(Decode_AbsX_Addr());
}

void Cpu6502::Decode_ORA(u8 M)
{
    m_regs.A |= M;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_ORA_Imm()
{
    Decode_ORA((u8)m_regs.operand);
}

void Cpu6502::Decode_ORA_Zero()
{
    Decode_ORA(ReadByte(m_regs.operand));
}

void Cpu6502::Decode_ORA_ZeroX()
{
    Decode_ORA(ReadByte(Decode_ZeroX_Addr()));
}

void Cpu6502::Decode_ORA_Abs()
{
    Decode_ORA(ReadByte(Decode_Abs_Addr()));
}

void Cpu6502::Decode_ORA_AbsX()
{
    Decode_ORA(ReadByte(Decode_AbsX_Addr()));
}

void Cpu6502::Decode_ORA_AbsY()
{
    Decode_ORA(ReadByte(Decode_AbsY_Addr()));
}

void Cpu6502::Decode_ORA_IndX()
{
    Decode_ORA(ReadByte(Decode_IndX_Addr()));
}

void Cpu6502::Decode_ORA_IndY()
{
    Decode_ORA(ReadByte(Decode_IndY_Addr()));
}

void Cpu6502::Decode_LDA(u16 addr)
{
    m_regs.A = ReadByte(addr);
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_Imm()
{
    m_regs.A = (u8)m_regs.operand;
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_Zero()
{
    Decode_LDA(m_regs.operand);
}

void Cpu6502::Decode_LDA_ZeroX()
{
    Decode_LDA(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_LDA_Abs()
{
    Decode_LDA(Decode_Abs_Addr());
}

void Cpu6502::Decode_LDA_AbsX()
{
    Decode_LDA(Decode_AbsX_Addr());
}

void Cpu6502::Decode_LDA_AbsY()
{
    Decode_LDA(Decode_AbsY_Addr());
}

void Cpu6502::Decode_LDA_IndX()
{
    Decode_LDA(Decode_IndX_Addr());

    m_regs.A = ReadByte(Decode_IndX_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDA_IndY()
{
    m_regs.A = ReadByte(Decode_IndY_Addr());
    m_regs.SetNZ(m_regs.A);
}

void Cpu6502::Decode_LDX(u16 addr)
{
    m_regs.X = ReadByte(addr);
    m_regs.SetNZ(m_regs.X);
}
void Cpu6502::Decode_LDX_Imm()
{
    m_regs.X = (u8)m_regs.operand;
    m_regs.SetNZ(m_regs.X);
}
void Cpu6502::Decode_LDX_Zero()
{
    Decode_LDX(m_regs.operand);
}
void Cpu6502::Decode_LDX_ZeroY()
{
    Decode_LDX(Decode_ZeroY_Addr());
}
void Cpu6502::Decode_LDX_Abs()
{
    Decode_LDX(Decode_Abs_Addr());
}
void Cpu6502::Decode_LDX_AbsY()
{
    Decode_LDX(Decode_AbsY_Addr());
}

void Cpu6502::Decode_LDY(u16 addr)
{
    m_regs.Y = ReadByte(addr);
    m_regs.SetNZ(m_regs.Y);
}
void Cpu6502::Decode_LDY_Imm()
{
    m_regs.Y = (u8)m_regs.operand;
    m_regs.SetNZ(m_regs.Y);
}
void Cpu6502::Decode_LDY_Zero()
{
    Decode_LDY(m_regs.operand);
}
void Cpu6502::Decode_LDY_ZeroX()
{
    Decode_LDY(Decode_ZeroX_Addr());
}
void Cpu6502::Decode_LDY_Abs()
{
    Decode_LDY(Decode_Abs_Addr());
}
void Cpu6502::Decode_LDY_AbsX()
{
    Decode_LDY(Decode_AbsX_Addr());
}

void Cpu6502::Decode_JMP_Abs()
{
    m_regs.PC = Decode_Abs_Addr();
}

void Cpu6502::Decode_JMP_Ind()
{
    m_regs.PC = Decode_Ind_Addr();
}

void Cpu6502::Decode_JSR_Abs()
{
    WriteByte(0x100 + m_regs.SP--, (u8)((m_regs.PC-1)>>8));
    WriteByte(0x100 + m_regs.SP--, (u8)(m_regs.PC-1));
    m_regs.PC = Decode_Abs_Addr();
}

void Cpu6502::Decode_ROL(u16 addr)
{
    u8 val = ReadByte(addr);
    u8 newCarry = (val & 0x80) ? SR_Carry : 0;
    u8 oldCarry = (m_regs.SR & SR_Carry);
    val = (u8)((u16)val << 1) | oldCarry;
    WriteByte(addr, val);
    m_regs.SetNZC(val, newCarry);
}

void Cpu6502::Decode_ROR(u16 addr)
{
    u8 val = ReadByte(addr);
    u8 newCarry = val & 1;
    u8 oldCarry = (m_regs.SR & SR_Carry) ? 0x80 : 0;
    val = (u8)((u16)val >> 1) | oldCarry;
    WriteByte(addr, val);
    m_regs.SetNZC(val, newCarry);
}

void Cpu6502::Decode_ROL_Imp()
{
    u8 newCarry = (m_regs.A & 0x80) ? SR_Carry : 0;
    u8 oldCarry = (m_regs.SR & SR_Carry);
    m_regs.A = (u8)((u16)m_regs.A << 1) | oldCarry;
    m_regs.SetNZC(m_regs.A, newCarry);
}

void Cpu6502::Decode_ROR_Imp()
{
    u8 newCarry = m_regs.A & 1;
    u8 oldCarry = (m_regs.SR & SR_Carry) ? 0x80 : 0;
    m_regs.A = (u8)((u16)m_regs.A >> 1) | oldCarry;
    m_regs.SetNZC(m_regs.A, newCarry);
}

void Cpu6502::Decode_ROR_Zero()
{
    Decode_ROR(Decode_Zero_Addr());
}

void Cpu6502::Decode_ROR_ZeroX()
{
    Decode_ROR(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_ROR_Abs()
{
    Decode_ROR(Decode_Abs_Addr());
}

void Cpu6502::Decode_ROR_AbsX()
{
    Decode_ROR(Decode_AbsX_Addr());
}

void Cpu6502::Decode_ROL_Zero()
{
    Decode_ROL(Decode_Zero_Addr());
}

void Cpu6502::Decode_ROL_ZeroX()
{
    Decode_ROL(Decode_ZeroX_Addr());
}

void Cpu6502::Decode_ROL_Abs()
{
    Decode_ROL(Decode_Abs_Addr());
}

void Cpu6502::Decode_ROL_AbsX()
{
    Decode_ROL(Decode_AbsX_Addr());
}

void Cpu6502::Decode_STA_Zero()
{
    WriteByte(m_regs.operand, m_regs.A);
}

void Cpu6502::Decode_STA_ZeroX()
{
    WriteByte(Decode_ZeroX_Addr(), m_regs.A);
}

void Cpu6502::Decode_STA_Abs()
{
    WriteByte(Decode_Abs_Addr(), m_regs.A);
}

void Cpu6502::Decode_STA_AbsX()
{
    WriteByte(Decode_AbsX_Addr(), m_regs.A);
}

void Cpu6502::Decode_STA_AbsY()
{
    WriteByte(Decode_AbsY_Addr(), m_regs.A);
}

void Cpu6502::Decode_STA_IndX()
{
    WriteByte(Decode_IndX_Addr(), m_regs.A);
}

void Cpu6502::Decode_STA_IndY()
{
    WriteByte(Decode_IndY_Addr(), m_regs.A);
}


void Cpu6502::Decode_STX_Zero()
{
    WriteByte(Decode_Zero_Addr(), m_regs.X);
}

void Cpu6502::Decode_STX_ZeroY()
{
    WriteByte(Decode_ZeroY_Addr(), m_regs.X);
}

void Cpu6502::Decode_STX_Abs()
{
    WriteByte(Decode_Abs_Addr(), m_regs.X);
}

void Cpu6502::Decode_STY_Zero()
{
    WriteByte(Decode_Zero_Addr(), m_regs.Y);
}

void Cpu6502::Decode_STY_ZeroX()
{
    WriteByte(Decode_ZeroX_Addr(), m_regs.Y);
}

void Cpu6502::Decode_STY_Abs()
{
    WriteByte(Decode_Abs_Addr(), m_regs.Y);
}



Cpu6502::Opcode gOpcodeUnknown = { "???", Cpu6502::AM_Imp, 0x00, 2 };

int gAddressingModeSize[] =
{
    1,  //    AM_Imp,              // operand
    2,  //    AM_Imm,            // operand #value
    2,  //    AM_Relative,             // operand value
    2,  //    AM_Zero,             // operand value
    2,  //    AM_ZeroX,            // operand value,x
    2,  //    AM_ZeroY,            // operand value,y
    2,  //    AM_IndX,            // operand (value, x)
    2,  //    AM_IndY,            // operand (value), y
    3,  //    AM_Abs,             // operand value
    3,  //    AM_AbsX,            // operand value,x
    3,  //    AM_AbsY,            // operand value,y
    3   //    AM_Ind              // operand (value)
};

const char* gAddressingModeName[] =
{
    "Imp", "Imm", "Rel", "Zero", "Zero X", "Zero Y", "Ind X", "Ind Y", "Abs", "Abs X", "Abs Y", "Ind"
};

Cpu6502::Cpu6502()
{
    gOpcodeUnknown.decode = DELEGATE(Cpu6502::Decode_Unknown);
    for (int i = 0; i < 256; i++)
    {
        m_opcodes[i] = gOpcodeUnknown;
    }

    // declare all the opcodes into the m_opcodes array and auto generate the decode function hooks
    OPC(ADC, Imm, 0x69, 2);
    OPC(ADC, Zero,  0x65, 3);
    OPC(ADC, ZeroX, 0x75, 4);
    OPC(ADC, Abs,  0x6D, 4);
    OPC(ADC, AbsX, 0x7D, 4);
    OPC(ADC, AbsY, 0x79, 4);
    OPC(ADC, IndX, 0x61, 6);
    OPC(ADC, IndY, 0x71, 5);

    OPC(AND, Imm, 0x29, 2);
    OPC(AND, Zero,  0x25, 3);
    OPC(AND, ZeroX, 0x35, 4);
    OPC(AND, Abs,  0x2D, 4);
    OPC(AND, AbsX, 0x3D, 4);
    OPC(AND, AbsY, 0x39, 4);
    OPC(AND, IndX, 0x21, 6);
    OPC(AND, IndY, 0x31, 5);

    OPC(ASL, Imp,   0x0A, 2);
    OPC(ASL, Zero,  0x06, 5);
    OPC(ASL, ZeroX, 0x16, 6);
    OPC(ASL, Abs,  0x0E, 6);
    OPC(ASL, AbsX, 0x1E, 7);

    OPC(BCC, Rel,  0x90, 2);
    OPC(BCS, Rel,  0xB0, 2);
    OPC(BEQ, Rel,  0xF0, 2);

    OPC(BIT, Zero,  0x24, 3);
    OPC(BIT, Abs,  0x2C, 4);

    OPC(BMI, Rel,  0x30, 2);
    OPC(BNE, Rel,  0xD0, 2);
    OPC(BPL, Rel,  0x10, 2);

    OPC(BRK, Imp,   0x00, 7);

    OPC(BVC, Rel,  0x50, 2);
    OPC(BVS, Rel,  0x70, 2);

    OPC(CLC, Imp,   0x18, 2);
    OPC(CLD, Imp,   0xD8, 2);
    OPC(CLI, Imp,   0x58, 2);
    OPC(CLV, Imp,   0xB8, 2);

    OPC(CMP, Imm, 0xC9, 2);
    OPC(CMP, Zero,  0xC5, 3);
    OPC(CMP, ZeroX, 0xD5, 4);
    OPC(CMP, Abs,  0xCD, 4);
    OPC(CMP, AbsX, 0xDD, 4);
    OPC(CMP, AbsY, 0xD9, 4);
    OPC(CMP, IndX, 0xC1, 6);
    OPC(CMP, IndY, 0xD1, 5);

    OPC(CPX, Imm, 0xE0, 2);
    OPC(CPX, Zero,  0xE4, 3);
    OPC(CPX, Abs,  0xEC, 4);

    OPC(CPY, Imm, 0xC0, 2);
    OPC(CPY, Zero,  0xC4, 3);
    OPC(CPY, Abs,  0xCC, 4);

    OPC(DEC, Zero,  0xC6, 5);
    OPC(DEC, ZeroX, 0xD6, 6);
    OPC(DEC, Abs,  0xCE, 6);
    OPC(DEC, AbsX, 0xDE, 7);

    OPC(DEX, Imp,   0xCA, 2);
    OPC(DEY, Imp,   0x88, 2);

    OPC(EOR, Imm, 0x49, 2);
    OPC(EOR, Zero,  0x45, 3);
    OPC(EOR, ZeroX, 0x55, 4);
    OPC(EOR, Abs,  0x4D, 4);
    OPC(EOR, AbsX, 0x5D, 4);
    OPC(EOR, AbsY, 0x59, 4);
    OPC(EOR, IndX, 0x41, 6);
    OPC(EOR, IndY, 0x51, 5);

    OPC(INC, Zero,  0xE6, 5);
    OPC(INC, ZeroX, 0xF6, 6);
    OPC(INC, Abs,  0xEE, 6);
    OPC(INC, AbsX, 0xFE, 7);

    OPC(INX, Imp,   0xE8, 2);
    OPC(INY, Imp,   0xC8, 2);

    OPC(JMP, Abs,  0x4C, 3);
    OPC(JMP, Ind,  0x6C, 5);

    OPC(JSR, Abs,  0x20, 6);

    OPC(LDA, Imm, 0xA9, 2);
    OPC(LDA, Zero,  0xA5, 3);
    OPC(LDA, ZeroX, 0xB5, 4);
    OPC(LDA, Abs,  0xAD, 4);
    OPC(LDA, AbsX, 0xBD, 4);
    OPC(LDA, AbsY, 0xB9, 4);
    OPC(LDA, IndX, 0xA1, 6);
    OPC(LDA, IndY, 0xB1, 5);

    OPC(LDX, Imm, 0xA2, 2);
    OPC(LDX, Zero,  0xA6, 3);
    OPC(LDX, ZeroY, 0xB6, 4);
    OPC(LDX, Abs,  0xAE, 4);
    OPC(LDX, AbsY, 0xBE, 4);

    OPC(LDY, Imm, 0xA0, 2);
    OPC(LDY, Zero,  0xA4, 3);
    OPC(LDY, ZeroX, 0xB4, 4);
    OPC(LDY, Abs,  0xAC, 4);
    OPC(LDY, AbsX, 0xBC, 4);

    OPC(LSR, Imp,   0x4A, 2);
    OPC(LSR, Zero,  0x46, 5);
    OPC(LSR, ZeroX, 0x56, 6);
    OPC(LSR, Abs,  0x4E, 6);
    OPC(LSR, AbsX, 0x5E, 7);

    OPC(NOP, Imp,   0xEA, 2);

    OPC(ORA, Imm, 0x09, 2);
    OPC(ORA, Zero,  0x05, 3);
    OPC(ORA, ZeroX, 0x15, 4);
    OPC(ORA, Abs,  0x0D, 4);
    OPC(ORA, AbsX, 0x1D, 4);
    OPC(ORA, AbsY, 0x19, 4);
    OPC(ORA, IndX, 0x01, 6);
    OPC(ORA, IndY, 0x11, 5);

    OPC(PHA, Imp,   0x48, 3);
    OPC(PHP, Imp,   0x08, 3);
    OPC(PLA, Imp,   0x68, 4);
    OPC(PLP, Imp,   0x28, 4);

    OPC(ROL, Imp, 0x2A, 2);
    OPC(ROL, Zero, 0x26, 5);
    OPC(ROL, ZeroX, 0x36, 6);
    OPC(ROL, Abs, 0x2E, 6);
    OPC(ROL, AbsX, 0x3E, 7);

    OPC(ROR, Imp, 0x6A, 2);
    OPC(ROR, Zero,  0x66, 5);
    OPC(ROR, ZeroX, 0x76, 6);
    OPC(ROR, Abs,  0x4E, 6);
    OPC(ROR, AbsX, 0x7E, 7);

    OPC(RTI, Imp,   0x40, 6);
    OPC(RTS, Imp,   0x60, 6);

    OPC(SBC, Imm, 0xE9, 2);
    OPC(SBC, Zero,  0xE5, 3);
    OPC(SBC, ZeroX, 0xF5, 4);
    OPC(SBC, Abs,  0xED, 4);
    OPC(SBC, AbsX, 0xFD, 4);
    OPC(SBC, AbsY, 0xF9, 4);
    OPC(SBC, IndX, 0xE1, 6);
    OPC(SBC, IndY, 0xF1, 5);

    OPC(SEC, Imp,   0x38, 2);
    OPC(SED, Imp,   0xF8, 2);
    OPC(SEI, Imp,   0x78, 2);

    OPC(STA, Zero,  0x85, 3);
    OPC(STA, ZeroX, 0x95, 4);
    OPC(STA, Abs,  0x8D, 4);
    OPC(STA, AbsX, 0x9D, 5);
    OPC(STA, AbsY, 0x99, 5);
    OPC(STA, IndX, 0x81, 6);
    OPC(STA, IndY, 0x91, 6);

    OPC(STX, Zero,  0x86, 3);
    OPC(STX, ZeroY, 0x96, 4);
    OPC(STX, Abs,  0x8E, 4);

    OPC(STY, Zero,  0x84, 3);
    OPC(STY, ZeroX, 0x94, 4);
    OPC(STY, Abs,  0x8C, 4);

    OPC(TAX, Imp,   0xAA, 3);
    OPC(TAY, Imp,   0xA8, 3);
    OPC(TSX, Imp,   0xBA, 3);
    OPC(TXA, Imp,   0x8A, 3);
    OPC(TXS, Imp,   0x9A, 3);
    OPC(TYA, Imp,   0x98, 3);

    // copy opcode data to indexed table - we'll need this for fast emulation
    // TODO: make this a hash table
    for (int i = 0; i < 256; i++)
    {
        if (std::find(m_uniqueOpcodes.begin(), m_uniqueOpcodes.end(), m_opcodes[i].name) == m_uniqueOpcodes.end())
            m_uniqueOpcodes.push_back(m_opcodes[i].name);
    }
}

void Cpu6502::Reset(u16 cpuStart)
{
    memset(&m_regs, 0, sizeof(m_regs));
    m_regs.PC = cpuStart;
    m_regs.SP = 0xf6;
    m_regs.SR = 0;
    m_irqInterrupt = 0;
    m_nmiInterrupt = 0;
    m_interruptDelay = 0;

#if defined(TRACE_FILE)
    traceFH = fopen("trace.txt", "w");
#endif
}

bool Cpu6502::IsOpcode(const char* text, ForceAddressing* forceAM)
{
    int len;
    auto* end = SDL_strchr(text, '.');
    if (forceAM)
        *forceAM = FA_Auto;

    if (end)
    {
        len = (int)(end - text);
        if (forceAM)
        {
            char ch = tolower(end[1]);
            if (ch == 'z' && end[2] == 0)
                *forceAM = FA_Zero;
            else if (ch == 'a' && end[2] == 0)
                *forceAM = FA_Absolute;
            else
                *forceAM = FA_Error;
        }
    }
    else
    {
        len = (int)SDL_strlen(text);
        if (forceAM)
            *forceAM = FA_Auto;
    }

    for (auto opcode : m_uniqueOpcodes)
    {
        if (SDL_strncasecmp(text, opcode.c_str(), len) == 0)
        {
            return true;
        }
    }
    return false;
}

Cpu6502::Opcode* Cpu6502::FindOpcode(const string& name, AddressingMode am)
{
    size_t idx = name.find_first_of('.');
    size_t len = (idx == string::npos) ? name.size() : idx;

    for (int i=0; i<255; i++)
    {
        if ((SDL_strncasecmp(name.c_str(), m_opcodes[i].name, len) == 0) && m_opcodes[i].addressMode == am)
        {
            return &m_opcodes[i];
        }
    }
    return nullptr;
}

int Cpu6502::GetAddressingModeSize(AddressingMode am)
{
    return gAddressingModeSize[am];
}
const char *Cpu6502::GetAddressingModeName(AddressingMode am)
{
    return gAddressingModeName[am];
}

bool Cpu6502::Step()
{
    if (m_interruptDelay)
        m_interruptDelay--;

    m_regs.frameCycle++;
    if (m_regs.delayCycles)
    {
        if ((--m_regs.delayCycles) == 0)
        {
            BreakpointCheck(BRK_Execute, m_regs.PC, 0);
            return true;
        }
        return false;
    }
    else
    {
        if (m_regs.decodeCycle == 0)
        {
            if (m_interruptDelay == 0 && (m_nmiInterrupt || m_irqInterrupt))
            {
                // not sure how long an interrupt takes...
                m_regs.delayCycles = 4;
                WriteByte(0x100 + m_regs.SP--, (u8)((m_regs.PC) >> 8));
                WriteByte(0x100 + m_regs.SP--, (u8)(m_regs.PC));
                WriteByte(0x100 + m_regs.SP--, m_regs.SR);
                m_regs.SR |= SR_Interrupt;
                if (m_nmiInterrupt)
                {
                    m_nmiInterrupt = false;
                    m_regs.PC = ReadByte(0xfffa) + (ReadByte(0xfffb) << 8);

#if defined(TRACE_FILE)
                    fprintf(traceFH, "NMI -> %04x\n", m_regs.PC);
#endif
                }
                else if (m_irqInterrupt)
                {
                    m_irqInterrupt = false;
                    m_regs.PC = ReadByte(0xfffe) + (ReadByte(0xffff) << 8);

#if defined(TRACE_FILE)
                    fprintf(traceFH, "IRQ -> %04x\n", m_regs.PC);
#endif
                }
                return false;
            }
            else
            {
#if defined(TRACE_FILE)
                string out = Disassemble(m_regs.PC);
                string regsOut = FormatString("%-40s  A %02x X %02x Y %02x SP %02x SR %c%c%c%c%c%c%c%c  STACK %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", out.c_str(), m_regs.A, m_regs.X, m_regs.Y, m_regs.SP,
                    (m_regs.SR & SR_Negative) ? 'N' : '.',
                    (m_regs.SR & SR_Overflow) ? 'V' : '.',
                    '-',
                    (m_regs.SR & SR_Break) ? 'B' : '.',
                    (m_regs.SR & SR_Decimal) ? 'D' : '.',
                    (m_regs.SR & SR_Interrupt) ? 'I' : '.',
                    (m_regs.SR & SR_Zero) ? 'Z' : '.',
                    (m_regs.SR & SR_Carry) ? 'C' : '.',

                    ReadByte(0x100 + m_regs.SP + 1),
                    ReadByte(0x100 + m_regs.SP + 2),
                    ReadByte(0x100 + m_regs.SP + 3),
                    ReadByte(0x100 + m_regs.SP + 4),
                    ReadByte(0x100 + m_regs.SP + 5),
                    ReadByte(0x100 + m_regs.SP + 6),
                    ReadByte(0x100 + m_regs.SP + 7),
                    ReadByte(0x100 + m_regs.SP + 8),
                    ReadByte(0x100 + m_regs.SP + 9),
                    ReadByte(0x100 + m_regs.SP + 10),
                    ReadByte(0x100 + m_regs.SP + 11),
                    ReadByte(0x100 + m_regs.SP + 12),
                    ReadByte(0x100 + m_regs.SP + 13),
                    ReadByte(0x100 + m_regs.SP + 14),
                    ReadByte(0x100 + m_regs.SP + 15),
                    ReadByte(0x100 + m_regs.SP + 16)
                );
                fprintf(traceFH, "%s\n", regsOut.c_str());
#endif

                u8 op = ReadByte(m_regs.PC++);
                m_regs.op = &m_opcodes[op];
                m_regs.opcodeCycleCount = m_regs.op->cycles;
            }
        }
        else if (m_regs.decodeCycle == 1 && m_regs.op->addressMode >= AM_Imm)
        {
            m_regs.operand = ReadByte(m_regs.PC++);
        }
        else if (m_regs.decodeCycle == 2 && m_regs.op->addressMode >= AM_Abs)
        {
            m_regs.operand = m_regs.operand | ((u16)ReadByte(m_regs.PC++) << 8);
        }

        if (++m_regs.decodeCycle == m_regs.opcodeCycleCount)
        {
            if (m_regs.op->decode)
            {
                // execute the instruction,  this will update regs/memory and may generate delay cycles
                m_regs.op->decode();
                m_regs.decodeCycle = 0;
                if (!m_regs.delayCycles)
                {
                    BreakpointCheck(BRK_Execute, m_regs.PC, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

u8 Cpu6502::ReadByte(u16 addr)
{
    u8 result = MemReadByte(addr);
    BreakpointCheck(BRK_Read, addr, result);
    return result;
}

void Cpu6502::WriteByte(u16 addr, u8 val)
{
    MemWriteByte(addr, val);
    BreakpointCheck(BRK_Write, addr, val);
}

void Cpu6502::TriggerColdStart()
{
    Reset(0xfffc);
}
void Cpu6502::TriggerInterrupt()
{
    if ((m_regs.SR & SR_Interrupt)==0)
    {
        m_irqInterrupt = true;
    }
}
void Cpu6502::TriggerNMInterrupt()
{
    if ((m_regs.SR & SR_Interrupt)==0)
    {
        m_nmiInterrupt = true;
    }
}

string Cpu6502::Disassemble(u16 addr)
{
    DisassembledLine dl;
    dl.size = SDL_min(0x10000 - addr, 3);
    for (int i=0; i<dl.size; i++)
        dl.ram[i] = ReadByte(addr+i);
    if (!Disassemble(dl))
    {
        dl.text = "???";
    }
    return dl.text;
}

bool Cpu6502::Disassemble(DisassembledLine &dl)
{
    u8 opc = dl.ram[0];
    auto &opcode = m_opcodes[opc];
    if (StrEqual(opcode.name, "???"))
        return false;

    switch (opcode.addressMode)
    {
        case AM_Imp:
            dl.text = FormatString("    %s", opcode.name);
            dl.size = 1;
            return true;

        case AM_Imm:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s #$%02x", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_Rel:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.target = dl.addr + (i8)val;
                    dl.text = FormatString("    %s $%04x", opcode.name, dl.target);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_Zero:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s $%02x", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_ZeroX:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s $%02x,x", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_ZeroY:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s $%02x,y", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_IndX:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s ($%02x,x)", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_IndY:
            {
                if (dl.size > 1)
                {
                    u8 val = dl.ram[1];
                    dl.text = FormatString("    %s ($%02x),y", opcode.name, val);
                    dl.size = 2;
                    return true;
                }
            }
            break;

        case AM_Abs:
            {
                if (dl.size > 2)
                {
                    u16 target = dl.ram[1] + (((u16)dl.ram[2]) << 8);
                    if (opcode.opc == 0x4c || opcode.opc == 0x20 || target > 0xff)
                        dl.text = FormatString("    %s $%04x", opcode.name, target);
                    else
                        dl.text = FormatString("    %s.a $%04x", opcode.name, target);
                    dl.size = 3;
                    return true;
                }
            }
            break;

        case AM_AbsX:
            {
                if (dl.size > 2)
                {
                    u16 target = dl.ram[1] + (((u16)dl.ram[2]) << 8);
                    if (target > 0xff)
                        dl.text = FormatString("    %s $%04x,x", opcode.name, target);
                    else
                        dl.text = FormatString("    %s.a $%04x,x", opcode.name, target);
                    dl.size = 3;
                    return true;
                }
            }
            break;

        case AM_AbsY:
            {
                if (dl.size > 2)
                {
                    u16 target = dl.ram[1] + (((u16)dl.ram[2]) << 8);
                    if (target > 0xff)
                        dl.text = FormatString("    %s $%04x,y", opcode.name, target);
                    else
                        dl.text = FormatString("    %s.a $%04x,y", opcode.name, target);
                    dl.size = 3;
                    return true;
                }
            }
            break;

        case AM_Ind:
            {
                if (dl.size > 2)
                {
                    u16 target = dl.ram[1] + (((u16)dl.ram[2]) << 8);
                    dl.text = FormatString("    %s ($%04x)", opcode.name, target);
                    dl.size = 3;
                    return true;
                }
            }
            break;
    }
    dl.text = FormatString("?!?");
    return false;
}


