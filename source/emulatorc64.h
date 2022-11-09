#pragma once

#include "chips/cia1.h"
#include "chips/cia2.h"
#include "chips/cpu6502.h"
#include "chips/memc64.h"
#include "chips/sid.h"
#include "chips/vic.h"

class EmulatorC64
{
public:
	EmulatorC64();
	~EmulatorC64();

	u32 CyclesPerSecond() { return 985248; }

	void ConvertSnapshot();
	void Update();
	void Draw();

	// reset memory and PC pointer
	void Reset(u8 *ram, u8 *ramMask, u16 cpuStart);

	// step one cycle
	// return true if cpu completed an instruction
	bool Step();

	u8 GetByte(u16 addr);
	void SetByte(u16 addr, u8 val);

	// copy regs in a thread safe way
	u16 GetCurrentPC() { return m_cpu->Regs().PC; }
	Cpu6502* GetCpu() { return m_cpu; }
	Vic* GetVic() { return m_vic; }

protected:
	Cpu6502* m_cpu;
	Vic* m_vic;
	Cia1* m_cia1;
	Cia2* m_cia2;
	MemC64* m_mem;
	Sid* m_sid;
}


;