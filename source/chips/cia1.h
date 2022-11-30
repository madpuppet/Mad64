#pragma once

class Cia1
{
public:
    Cia1();

    struct Registers
    {
        // Monitoring / control of the 8 data lines of Port A.
        // The lines are used for multiple purposes :
        //    Read / Write : Bit 0..7 keyboard matrix columns
        //    Read : Joystick Port 2 : Bit 0..3 Direction(Left / Right / Up / Down), Bit 4 Fire button. 0 = activated.
        //    Read : Lightpen : Bit 4 (as fire button), connected also with “ / LP”(Pin 9) of the VIC
        //    Read : Paddles: Bit 2..3 Fire buttons, Bit 6..7 Switch control port 1 (% 01 = Paddles A) or 2 (% 10 = Paddles B)
        u8 dataPortA;

        // Monitoring / control of the 8 data lines of Port B.
        // The lines are used for multiple purposes :
        //    Read / Write : Bit 0..7 keyboard matrix rows
        //    Read : Joystick Port 1 : Bit 0..3 Direction(Left / Right / Up / Down), Bit 4 Fire button. 0 = activated.
        //    Read : Bit 6 : Timer A : Toggle / Impulse output(see register 14 bit 2)
        //    Read : Bit 7 : Timer B : Toggle / Impulse output(see register 15 bit 2)
        u8 dataPortB;

        // Bit X : 0 = Input(read only), 1 = Output(read and write)
        u8 dataDirPortA;

        // Bit X : 0 = Input(read only), 1 = Output(read and write)
        u8 dataDirPortB;

        // Read : actual value Timer A
        // Writing : Set latch of Timer A
        //           When writing to high byte, if the timer is stopped, the high-byte will automatically be re-set as well
        u8 timerALow;
        u8 timerAHigh;

        // Read : actual value Timer B
        // Writing : Set latch of Timer B
        //           When writing to high byte, if the timer is stopped, the high-byte will automatically be re-set as well
        u8 timerBLow;
        u8 timerBHigh;

        // Real Time Clock
        // 1 / 10s Read :
        // Bit 0..3 : Tenth seconds in BCD - format($0 - $9)
        // Bit 4..7 : always 0
        // Writing :
        // Bit 0..3 : if CRB - Bit7 = 0 : Set the tenth seconds in BCD - format
        // Bit 0..3 : if CRB - Bit7 = 1 : Set the tenth seconds of the alarm time in BCD - format
        u8 clockTenths;

        // Bit 0..3 : Single seconds in BCD - format($0 - $9)
        // Bit 4..6 : Ten seconds in BCD - format($0 - $5)
        // Bit 7    : always 0
        u8 clockSeconds;

        // Bit 0..3: Single minutes in BCD - format($0 - $9)
        // Bit 4..6 : Ten minutes in BCD - format($0 - $5)
        // Bit 7 : always 0
        u8 clockMinutes;

        // Bit 0..3: Single hours in BCD - format($0 - $9)
        // Bit 4..6 : Ten hours in BCD - format($0 - $5)
        // Bit 7 : Differentiation AM / PM, 0 = AM, 1 = PM
        // Writing into this register stops TOD, until register 8 (TOD 10THS) will be read.
        u8 clockHours;

        // The byte within this register will be shifted bitwise to or from the SP-pin with every positive slope at the CNT-pin.
        u8 serialShift;

        // CIA1 is connected to the IRQ-Line.
        // Read: (Bit0..4 = INT DATA, Origin of the interrupt)
        //   Bit 0 : 1 = Underflow Timer A
        //   Bit 1 : 1 = Underflow Timer B
        //   Bit 2 : 1 = Time of day and alarm time is equal
        //   Bit 3 : 1 = SDR full or empty, so full byte was transferred, depending of operating mode serial bus
        //   Bit 4 : 1 = IRQ Signal occured at FLAG - pin(cassette port Data input, serial bus SRQ IN)
        //   Bit 5..6 : always 0
        //   Bit 7 : 1 = IRQ An interrupt occured, so at least one bit of INT MASK and INT DATA is set in both registers.
        // Flags will be cleared after reading the register!
        // Write : (Bit 0..4 = INT MASK, Interrupt mask)
        //   Bit 0 : 1 = Interrupt release through timer A underflow
        //   Bit 1 : 1 = Interrupt release through timer B underflow
        //   Bit 2 : 1 = Interrupt release if clock = alarmtime
        //   Bit 3 : 1 = Interrupt release if a complete byte has been received / sent.
        //   Bit 4 : 1 = Interrupt release if a positive slope occurs at the FLAG - Pin.
        //   Bit 5..6 : unused
        //   Bit 7 : Source bit. 0 = set bits 0..4 are clearing the according mask bit. 1 = set bits 0..4 are setting the according mask bit.If all bits 0..4 are cleared, there will be no change to the mask.
        u8 interruptControl;

        // Bit 0: 0 = Stop timer; 1 = Start timer
        // Bit 1: 1 = Indicates a timer underflow at port B in bit 6.
        // Bit 2 : 0 = Through a timer overflow, bit 6 of port B will get high for one cycle, 1 = Through a timer underflow, bit 6 of port B will be inverted
        // Bit 3 : 0 = Timer - restart after underflow(latch will be reloaded), 1 = Timer stops after underflow.
        // Bit 4 : 1 = Load latch into the timer once.
        // Bit 5 : 0 = Timer counts system cycles, 1 = Timer counts positive slope at CNT - pin
        // Bit 6 : Direction of the serial shift register, 0 = SP - pin is input(read), 1 = SP - pin is output(write)
        // Bit 7 : Real Time Clock, 0 = 60 Hz, 1 = 50 Hz
        u8 controlTimerA;

        // Bit 0: 0 = Stop timer; 1 = Start timer
        // Bit 1: 1 = Indicates a timer underflow at port B in bit 7.
        // Bit 2 : 0 = Through a timer overflow, bit 7 of port B will get high for one cycle, 1 = Through a timer underflow, bit 7 of port B will be inverted
        // Bit 3 : 0 = Timer - restart after underflow(latch will be reloaded), 1 = Timer stops after underflow.
        // Bit 4 : 1 = Load latch into the timer once.
        // Bit 5..6 :
        //   % 00 = Timer counts System cycle
        //   % 01 = Timer counts positive slope on CNT - pin
        //   % 10 = Timer counts underflow of timer A
        //   % 11 = Timer counts underflow of timer A if the CNT - pin is high
        // Bit 7 : 0 = Writing into the TOD register sets the clock time, 1 = Writing into the TOD register sets the alarm time.
        u8 controlTimerB;
    } m_regs;

    u8 ReadReg(u16 addr);
    void WriteReg(u16 addr, u8 val);

    void Reset();
    void Step();

    // set the callback for triggering a cpu interrupt
    void SetTriggerInterrupt(const InterruptHook& hook) { TriggerInterrupt = hook; }

    // capture keyboard
    void OnKeyDown(SDL_Event* e);
    void OnKeyUp(SDL_Event* e);

protected:
    InterruptHook TriggerInterrupt;

    bool m_timerARunning;
    bool m_timerAOneShot;
    bool m_timerACNT;
    bool m_serialShiftIsWrite;
    bool m_realTimeClock50hz;
    u16 m_timerALatch;              // latch is what value timer resets to on reset
    u16 m_timerAVal;

    enum class TimerBMode
    {
        Cycle,
        CNT,
        TimerA,
        CNTAndTimerA
    };
    TimerBMode m_timerBMode;
    bool m_timerBRunning;
    bool m_timerBOneShot;

    enum class Interrupts
    {
        TimerA = 1,
        TimerB = 2,
        Alarm = 4,
        ByteTransferred = 8,
        FlagPort = 16
    };
    u8 m_interruptEnabledMask;

    u16 m_timerBLatch;              // latch is what value timer resets to on reset
    u16 m_timerBVal;
    void StepTimerB();

    u8 m_keyState[8];               // 8 rows of keys - each bit represents a key (column) in that row
    u8 m_keyboardRowMask;
    int m_keyMap[256];              // map Scan codes to keyState entries  r*8 + c;
};

