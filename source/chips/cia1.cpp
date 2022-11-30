#include "common.h"
#include "cia1.h"

#define K(key,r,c) m_keyMap[(SDLK_##key) & 255]=r*8+(7-c)
#define KEY_ROW(r, k0, k1, k2, k3, k4, k5, k6, k7) K(k0,r,0);K(k1,r,1);K(k2,r,2);K(k3,r,3);K(k4,r,4);K(k5,r,5);K(k6,r,6);K(k7,r,7);

Cia1::Cia1()
{
    memset(m_keyMap, -1, sizeof(m_keyMap));
    memset(m_keyState, 0xff, sizeof(m_keyState));

    KEY_ROW(0, DOWN, F5, F3, F1, F7, RIGHT, RETURN, DELETE);
    KEY_ROW(1, LSHIFT, e, s, z, 4, a, w, 3);
    KEY_ROW(2, x, t, f, c, 6, d, r, 5);
    KEY_ROW(3, v, u, h, b, 8, g, y, 7);
    KEY_ROW(4, n, o, k, m, 0, j, i, 9);
    KEY_ROW(5, COMMA, AT, COLON, PERIOD, MINUS, l, p, PLUS);
    KEY_ROW(6, SLASH, CARET, EQUALS, RSHIFT, HOME, SEMICOLON, LEFTBRACKET, RIGHTBRACKET);
    KEY_ROW(7, TAB, q, MENU, SPACE, 2, LCTRL, BACKQUOTE, 1);

    m_keyMap[SDLK_k] = 4 * 8 + 2;
}

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

    m_timerALatch = m_timerAVal = 0x3fff;
    m_timerARunning = true;
    m_timerAOneShot = false;
    m_timerACNT = false;
    m_serialShiftIsWrite = false;
    m_realTimeClock50hz = true;

    m_timerBLatch = m_timerBVal = 0x3fff;
    m_timerBRunning = false;
    m_timerBOneShot = false;
    m_timerBMode = TimerBMode::Cycle;
}

void Cia1::StepTimerB()
{
    m_timerBVal--;
    if (m_timerBVal == 0xffff)
    {
        m_regs.interruptControl |= 2;

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
    if (addr == (u16)((u64) & (((Registers*)0)->dataPortB)))
    {
        u8 val = 0xff;
        for (int i = 0; i < 8; i++)
        {
            if (!(m_keyboardRowMask & (1<<i)))
                val &= m_keyState[i];
        }
        return val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->interruptControl)))
    {
        m_regs.interruptControl = 0;
    }
    return val;
}

void Cia1::WriteReg(u16 addr, u8 val)
{
    addr = addr & 15;
    if (addr == (u16)((u64) & (((Registers*)0)->dataPortA)))
    {
        m_keyboardRowMask = val;
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->controlTimerA)))
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

void Cia1::OnKeyDown(SDL_Event* e)
{
    int dat = m_keyMap[(e->key.keysym.sym)&255];
    if (dat != -1)
    {
        int r = (dat >> 3) & 7;
        int c = dat & 7;
        m_keyState[r] &= ~(1 << c);
    }
}

void Cia1::OnKeyUp(SDL_Event* e)
{
    int dat = m_keyMap[(e->key.keysym.sym)&255];
    if (dat != -1)
    {
        int r = (dat >> 3) & 7;
        int c = dat & 7;
        m_keyState[r] |= (1 << c);
    }
}
