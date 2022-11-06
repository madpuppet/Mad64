#pragma once

extern CpuRegs gC64_cpuState;
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

	void ConvertSnapshot();
	void Update();
	void Draw();

	// reset memory and PC pointer
	void Reset(u8 *ram, u8 *ramMask, u16 cpuStart);

	// step one cycle
	void Step();

	u8 GetByte(u16 addr);
	void SetByte(u16 addr, u8 val);

	// copy regs in a thread safe way
	void CopyRegs(CpuRegs& regs) { regs = m_regs; }

	// current cpu state
	CpuRegs m_regs;

	bool m_delayCycle;				// instruction generated extra delay cycle
	int m_decodeCycle;				// current decode cycle
	int m_opcodeCycleCount;			// how many cycles in the current opcode execution
	Opcode* m_co;					// currently decoded opcode (valid after the first cycle)

protected:
	// current ram state
	u8 m_ram[65536];
}


;