#include "common.h"
#include "cia1.h"
#include "SDL.h"
#include "SDL_joystick.h"

#define K(key,row,col) m_keyMap[((SDLK_##key)>>22)|((SDLK_##key) & 255)]=(row<<4)+col
#define KEY_ROW(row, k0, k1, k2, k3, k4, k5, k6, k7) K(k0,row,0);K(k1,row,1);K(k2,row,2);K(k3,row,3);K(k4,row,4);K(k5,row,5);K(k6,row,6);K(k7,row,7)

Cia1::Cia1()
{
    memset(m_keyMap, -1, sizeof(m_keyMap));

    KEY_ROW(0, DELETE, RETURN, RIGHT, F7, F1, F3, F5, DOWN);
    KEY_ROW(1, 3, w, a, 4, z, s, e, LSHIFT);
    KEY_ROW(2, 5, r, d, 6, c, f, t, x);
    KEY_ROW(3, 7, y, g, 8, b, h, u, v);
    KEY_ROW(4, 9, i, j, 0, m, k, o, n);
    KEY_ROW(5, PLUS, p, l, MINUS, PERIOD, COLON, AT, COMMA);
    KEY_ROW(6, PAGEDOWN, RIGHTBRACKET, SEMICOLON, HOME, RSHIFT, BACKSLASH, PAGEUP, SLASH);
    KEY_ROW(7, 1, BACKQUOTE, LCTRL, 2, SPACE, LALT, q, TAB);
    
    m_keys.push_back({ SDLK_QUOTE, 0, true, 0x30, 0x17 });
    m_keys.push_back({ SDLK_QUOTE, KMOD_LSHIFT, true, 0x73, 0x17});
    m_keys.push_back({ SDLK_2, KMOD_LSHIFT, true, 0x56, -0x17 });
    m_keys.push_back({ SDLK_6, KMOD_LSHIFT, true, 0x66, -0x17 });
    m_keys.push_back({ SDLK_7, KMOD_LSHIFT, true, 0x23, 0x17 });
    m_keys.push_back({ SDLK_8, KMOD_LSHIFT, true, 0x61, -0x17 });
    m_keys.push_back({ SDLK_9, KMOD_LSHIFT, true, 0x33, 0x17 });
    m_keys.push_back({ SDLK_0, KMOD_LSHIFT, true, 0x40, 0x17 });
    m_keys.push_back({ SDLK_EQUALS, KMOD_LSHIFT, true, 0x50, -0x17 });
    m_keys.push_back({ SDLK_EQUALS, 0, false, 0x65, 0 });
    m_keys.push_back({ SDLK_BACKSPACE, 0, false, 0, 0 });
    m_keys.push_back({ SDLK_SEMICOLON, 0, false, 0x62, 0 });
    m_keys.push_back({ SDLK_SEMICOLON, KMOD_LSHIFT, true, 0x55, -0x17 });
    m_keys.push_back({ SDLK_DOWN, 0, false, 0x07, 0 });
    m_keys.push_back({ SDLK_UP, 0, true, 0x07, 0x17 });
    m_keys.push_back({ SDLK_RIGHT, 0, true, 0x02, 0 });
    m_keys.push_back({ SDLK_LEFT, 0, true, 0x02, 0x17 });
    m_keys.push_back({ SDLK_INSERT, 0, true, 0x00, 0x17 });
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

    memset(m_keyState, 0xff, sizeof(m_keyState));
    memset(m_keyStateOr, 0, sizeof(m_keyStateOr));
    memset(m_keyStateAnd, 0xff, sizeof(m_keyStateAnd));
    m_keyboardRowMask = 0xff;

    m_joystickState[0] = 0xff;
    m_joystickState[1] = 0xff;
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
    if (addr == (u16)((u64) & (((Registers*)0)->dataPortA)))
    {
        return m_joystickState[0];
    }
    else if (addr == (u16)((u64) & (((Registers*)0)->dataPortB)))
    {
        u8 val = 0xff;
        for (int i = 0; i < 8; i++)
        {
            if (!(m_keyboardRowMask & (1 << i)))
                val = ((val & m_keyState[i] & m_keyStateAnd[i]) | m_keyStateOr[i]);
        }
        val &= m_joystickState[1];
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
    // check for special keys
    for (auto &k : m_keys)
    {
        if (e->key.keysym.sym == k.symbol && e->key.keysym.mod == k.modifier)
        {
            m_keyStateAnd[(k.code >> 4)] &= ~(1 << (k.code & 7));
            if (k.secondary)
            {
                if (k.code2 < 0)
                    m_keyStateOr[(-k.code2 >> 4)] |= (1 << (-k.code2 & 7));
                else
                    m_keyStateAnd[(k.code2 >> 4)] &= ~(1 << (k.code2 & 7));
            }
            m_keyDown.push_back(k);
            return;
        }
    }

    int sym = (e->key.keysym.sym >> 22) | (e->key.keysym.sym & 255);
    int dat = m_keyMap[sym];
    if (dat != -1)
    {
        int r = dat >> 4;
        int c = dat & 7;
        m_keyState[r] &= ~(1 << c);
    }
}

void Cia1::OnKeyUp(SDL_Event* e)
{
    // check for special keys
    for (auto it = m_keyDown.begin(); it != m_keyDown.end(); it++)
    {
        auto &k = *it;
        if (e->key.keysym.sym == k.symbol)
        {
            m_keyStateAnd[(k.code >> 4)] |= (1 << (k.code & 7));
            if (k.secondary)
            {
                if (k.code2 < 0)
                    m_keyStateOr[(-k.code2 >> 4)] &= ~(1 << (-k.code2 & 7));
                else
                    m_keyStateAnd[(k.code2 >> 4)] |= (1 << (k.code2 & 7));
            }
            m_keyDown.erase(it);
            return;
        }
    }

    int sym = (e->key.keysym.sym >> 22) | (e->key.keysym.sym & 255);
    int dat = m_keyMap[sym];
    if (dat != -1)
    {
        int r = dat >> 4;
        int c = dat & 7;
        m_keyState[r] |= (1 << c);
    }
}

void Cia1::OnJoystickButtonDown(SDL_Event* e)
{
    int bid = (e->jbutton.which) ? 1 : 0;

    auto settings = gApp->GetSettings();
    if (settings->swapJoystickPorts)
        bid = 1 - bid;

    m_joystickState[bid] &= ~(1 << 4);
}
void Cia1::OnJoystickButtonUp(SDL_Event* e)
{
    int bid = (e->jbutton.which) ? 1 : 0;

    auto settings = gApp->GetSettings();
    if (settings->swapJoystickPorts)
        bid = 1 - bid;

    m_joystickState[bid] |= (1 << 4);
}
void Cia1::OnJoystickAxisMotion(SDL_Event* e)
{
    int bid = (e->jbutton.which) ? 1 : 0;

    auto settings = gApp->GetSettings();
    if (settings->swapJoystickPorts)
        bid = 1 - bid;

    u8 mask = m_joystickState[bid];
    if (e->jaxis.axis == 0)  // x axis
    {
        if (e->jaxis.value < -16000)
            mask &= ~1;
        else
            mask |= 1;

        if (e->jaxis.value > 16000)
            mask &= ~2;
        else
            mask |= 2;
    }
    else if (e->jaxis.axis == 1)  // y axis
    {
        if (e->jaxis.value < -16000)
            mask &= ~4;
        else
            mask |= 4;

        if (e->jaxis.value > 16000)
            mask &= ~8;
        else
            mask |= 8;
    }
    m_joystickState[bid] = (m_joystickState[bid] | 0xf) & mask;
}


