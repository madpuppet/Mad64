#include "common.h"
#include "cia1.h"

void Cia1::Reset()
{
    memset(&m_regs, 0, sizeof(m_regs));

    m_regs.dataPortA = 0x7f;
    m_regs.dataPortB = 0xff;
    m_regs.dataPortA = 0xff;
    m_regs.dataDirPortA = 0xff;
    m_regs.controlTimerA = 0x01;
    m_regs.controlTimerB = 0x08;

    m_timerAMode = TimerAMode::Cycle;
    m_timerBMode = TimerBMode::Off;
    m_timerAStart = m_timerAVal = 0xffff;
    m_timerBStart = m_timerBVal = 0xffff;
}

void Cia1::StepTimerA()
{
    m_timerAVal--;
    if (m_timerAVal == 0xffff)
    {
        // underflow - trigger interrupt


        // update timer b
        if (m_timerBMode == TimerBMode::TimerA || m_timerBMode == TimerBMode::CNTAndTimerA)
        {
            StepTimerB();
        }

        // reset timer a
        m_timerAVal = m_timerAStart;
    }

    m_regs.timerALow = m_timerAVal & 0xff;
    m_regs.timerAHigh = (m_timerAVal >> 8) & 0xff;
}

void Cia1::StepTimerB()
{
    m_timerBVal--;
    if (m_timerBVal == 0xffff)
    {
        // underflow - trigger interrupt


        // reset timer
        m_timerBVal = m_timerBStart;
    }

    m_regs.timerBLow = m_timerBVal & 0xff;
    m_regs.timerBHigh = (m_timerBVal >> 8) & 0xff;
}

void Cia1::Step()
{
    switch (m_timerAMode)
    {
        case TimerAMode::Off:
            break;

        case TimerAMode::Cycle:
            {

                m_regs.timerALow--;
                if (m_regs.timerALow == 0xff)
                {
                    m_regs.timerAHigh--;
                    if (m_regs.timerAHigh == 0xff && m_timerBMode == TimerBMode::TimerA)
                    {
                        m_regs.timerBLow--;
                        if (m_regs.timerBLow == 0xff)
                            m_regs.timerBHigh--;
                    }
                }
            }
            break;
    }

    switch (m_timerBMode)
    {
        case TimerBMode::Off:
            break;

        case TimerBMode::Cycle:
            {
                m_regs.timerBLow++;
                if (m_regs.timerBLow == 0)
                    m_regs.timerBHigh++;
            }
            break;

        case TimerBMode::CNT:
            break;

        case TimerBMode::TimerA:
            break;

        case TimerBMode::CNTAndTimerA:
            break;
    }
}

u8 Cia1::ReadReg(u16 addr)
{
    addr = addr & 15;
    return ((u8*)&m_regs)[addr];
}

void Cia1::WriteReg(u16 addr, u8 val)
{
    addr = addr & 15;
    ((u8*)&m_regs)[addr] = val;

    if (addr == (u16)((u64) & (((Registers*)0)->controlTimerA)))
    {
        if (val & 1)
        {
            m_timerAMode = TimerAMode::Cycle;
        }
        else
        {
            m_timerAMode = TimerAMode::Off;
        }
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->controlTimerB)))
    {
        if (val & 1)
        {
            m_timerBMode = (TimerBMode)(((val >> 5) & 3) + 1);
        }
        else
        {
            m_timerBMode = TimerBMode::Off;
        }
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerALow)))
    {
        m_timerAStart = (m_timerAStart & 0xff00) | val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerAHigh)))
    {
        m_timerAStart = (m_timerAStart & 0xff) | ((u16)val<<8);
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerBLow)))
    {
        m_timerBStart = (m_timerBStart & 0xff00) | val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerBHigh)))
    {
        m_timerBStart = (m_timerBStart & 0xff) | ((u16)val << 8);
    }
}


