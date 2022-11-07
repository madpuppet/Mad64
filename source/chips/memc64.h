#pragma once

class MemC64
{
public:
    void Reset(u8* mem, u8* memMask);
    void Step() {};

    u8 ReadByte(u16 addr);
    void WriteByte(u16 addr, u8 value);

private:
    u8 m_ram[65536];
};
