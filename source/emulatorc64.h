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
	void CopyRegs(Cpu6502::Registers& regs) { regs = m_cpu->Regs(); }

protected:
	Cpu6502* m_cpu;
	Vic* m_vic;
	Cia1* m_cia1;
	Cia2* m_cia2;
	MemC64* m_mem;
	Sid* m_sid;
}


;