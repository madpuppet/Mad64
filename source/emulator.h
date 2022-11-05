#pragma once

struct Cpu6502State
{
	u16 regPC;
	u8 regA;
	u8 regX;
	u8 regY;
	u8 regSR;
	u8 regSP;
};
extern Cpu6502State gC64_cpuState;
extern u8 gC64_ramState[65536];
extern u8 gC64_kernalRom[8192];
extern u8 gC64_basicRom[8192];
extern u8 gC64_chargenRom[4096];
extern u8 gC64_sidState[0x4c6];
extern u8 gC64_vicIIState[0x4c7];

class Emulator 
{
public:
	Emulator();
	~Emulator();

	void BuildRam(CompilerSourceInfo *src);
	void ConvertSnapshot();
	void Update();
	void Draw();
	void Step();

protected:
	u8 m_ram[65536];

	Cpu6502State m_cpuState;
}


;