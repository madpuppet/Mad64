#pragma once

class Cia2
{
public:
    u8 ReadReg(u16 addr);
    void WriteReg(u16 addr, u8 val);

    void Reset();
    void Step();
};
