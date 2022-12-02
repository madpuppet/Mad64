#pragma once

class Cia2
{
public:
    struct Registers
    {
        // Bit 0..1: Select the position of the VIC - memory
        //   % 00, 0 : Bank 3 : $C000 - $FFFF, 49152 - 65535
        //   % 01, 1 : Bank 2 : $8000 - $BFFF, 32768 - 49151
        //   % 10, 2 : Bank 1 : $4000 - $7FFF, 16384 - 32767
        //   % 11, 3 : Bank 0 : $0000 - $3FFF, 0 - 16383 (standard)
        //   Bit 2 : RS - 232 : TXD Output, userport : Data PA 2 (pin M)
        //   Bit 3..5 : serial bus Output(0 = High / Inactive, 1 = Low / Active)
        //   Bit 3 : ATN OUT
        //   Bit 4 : CLOCK OUT
        //   Bit 5 : DATA OUT
        //   Bit 6..7 : serial bus Input(0 = Low / Active, 1 = High / Inactive)
        //   Bit 6 : CLOCK IN
        //   Bit 7 : DATA IN
        u8 dataPortA;

        //Bit 0..7: userport Data PB 0 - 7 (Pins C, D, E, F, H, J, K, L)
        //    The KERNAL offers several RS232 - Routines, which use the pins as followed :
        //Bit 0, 3..7 : RS - 232 : reading
        //    Bit 0 : RXD
        //    Bit 3 : RI
        //    Bit 4 : DCD
        //    Bit 5 : User port pin J
        //    Bit 6 : CTS
        //    Bit 7 : DSR
        //    Bit 1..5 : RS - 232 : writing
        //    Bit 1 : RTS
        //    Bit 2 : DTR
        //    Bit 3 : RI
        //    Bit 4 : DCD
        //    Bit 5 : User port pin J
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

        // CIA2 is connected to the NMI - Line.
        //   Bit 4: 1 = NMI Signal occured at FLAG - pin(RS - 232 data received)
        //   Bit 7 : 1 = NMI An interrupt occured, so at least one bit of INT MASK and INT DATA is set in both registers
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
    void SetVicBank(const SetByteHook& hook) { SetVicBankIdx = hook; }

protected:
    InterruptHook TriggerInterrupt;
    SetByteHook SetVicBankIdx;

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

    u8 m_vicBank;

    SDL_GameController* m_joystick;
};
