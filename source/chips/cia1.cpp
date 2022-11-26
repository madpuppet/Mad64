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

    m_interruptEnabledMask = (u8)Interrupts::TimerA;

    m_timerARunning = true;
    m_timerAOneShot = false;
    m_timerACNT = false;
    m_serialShiftIsWrite = false;
    m_realTimeClock50hz = true;

    m_timerALatch = m_timerAVal = 0x7ff;
    m_timerBRunning = false;
    m_timerBOneShot = false;
    m_timerBMode = TimerBMode::Cycle;
    m_timerBLatch = m_timerBVal = 0x7ff;
}

void Cia1::StepTimerB()
{
    m_timerBVal--;
    if (m_timerBVal == 0xffff)
    {
        m_regs.interruptControl |= 2;

        // underflow - trigger interrupt


        // reset
        if (m_timerBOneShot)
            m_timerBRunning = false;
        else
            m_timerBVal = m_timerBLatch;
    }

    m_regs.timerBLow = m_timerBVal & 0xff;
    m_regs.timerBHigh = (m_timerBVal >> 8) & 0xff;
}

void Cia1::Step()
{
    // step timer A
    if (m_timerARunning)
    {
        if (m_timerACNT)
        {
            // decrement if CNT is positive slope somehow..  Not Supported
            // I think this is custom user port stuff
            // also do timer B CNT & CNTAndTimerA modes
        }
        else
        {
            m_timerAVal--;
        }

        // underflow?
        if (m_timerAVal == 0xffff)
        {
            m_regs.interruptControl |= 1;

            // trigger interrupt

            // check for timer B update
            if (m_timerBRunning && (m_timerBMode >= TimerBMode::TimerA))
            {
                StepTimerB();
            }

            // reset
            if (m_timerAOneShot)
                m_timerARunning = false;
            else
                m_timerAVal = m_timerALatch;
        }

        m_regs.timerALow = m_timerAVal & 0xff;
        m_regs.timerAHigh = (m_timerAVal >> 8) & 0xff;
    }

    if (m_timerBRunning && (m_timerBMode == TimerBMode::Cycle))
    {
        StepTimerB();
    }

    if (m_regs.interruptControl & m_interruptEnabledMask)
        TriggerInterrupt();
}

u8 Cia1::ReadReg(u16 addr)
{
    addr = addr & 15;
    u8 val = ((u8*)&m_regs)[addr];
    if (addr == (u16)((u64) & (((Registers*)0)->interruptControl)))
    {
        m_regs.interruptControl = 0;
    }
    return val;
}

void Cia1::WriteReg(u16 addr, u8 val)
{
    addr = addr & 15;
    if (addr == (u16)((u64) & (((Registers*)0)->controlTimerA)))
    {
        // todo: I believe there is a 3 cycle delay to start a timer running...
        m_timerARunning = (val & 1) ? true : false;
        m_timerAOneShot = (val & 8) ? true : false;
        if (val & 16)
            m_timerAVal = m_timerALatch;
        m_timerACNT = (val & 32) ? true : false;
        m_serialShiftIsWrite = (val & 64) ? true : false;
        m_realTimeClock50hz = (val & 128) ? true : false;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->controlTimerB)))
    {
        m_timerBRunning = (val & 1) ? true : false;
        m_timerBOneShot = (val & 8) ? true : false;
        if (val & 16)
            m_timerBVal = m_timerBLatch;
        m_timerBMode = (TimerBMode)((val >> 5) & 3);
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerALow)))
    {
        m_timerALatch = (m_timerALatch & 0xff00) | val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerAHigh)))
    {
        m_timerALatch = (m_timerALatch & 0xff) | ((u16)val<<8);
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerBLow)))
    {
        m_timerBLatch = (m_timerBLatch & 0xff00) | val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->timerBHigh)))
    {
        m_timerBLatch = (m_timerBLatch & 0xff) | ((u16)val << 8);
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->interruptControl)))
    {
        if (val & 0x80)
        {
            m_interruptEnabledMask |= (val & 0x1f);
        }
        else
        {
            m_interruptEnabledMask &= ~(val & 0x1f);
        }
    }
}


