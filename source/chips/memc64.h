#pragma once

class MemC64
{
public:
    void Reset(u8* mem, u8* memMask);
    void Step() {};

    u8 ReadByte(u16 addr);
    void WriteByte(u16 addr, u8 value);

    // read byte from current vic 16k bank 0000..3fff
    u8 ReadVicBankByte(u16 addr);

    // hooks to allow memory to redirect to other chips for read/write
    void SetReadVicRegByte(const ReadByteHook& hook) { ReadVicRegByte = hook; }
    void SetWriteVicRegByte(const WriteByteHook& hook) { WriteVicRegByte = hook; }
    void SetReadVicColorByte(const ReadByteHook& hook) { ReadVicColorByte = hook; }
    void SetWriteVicColorByte(const WriteByteHook& hook) { WriteVicColorByte = hook; }

private:
    ReadByteHook ReadVicRegByte;
    WriteByteHook WriteVicRegByte;

    ReadByteHook ReadVicColorByte;
    WriteByteHook WriteVicColorByte;

    u8 m_ram[65536];
};
