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

	void Update();
	void Draw();

	// reset memory and PC pointer
	void Reset(u8* ram, u8* ramMask, u16 cpuStart);
	void ColdReset(u8* ram, u8* ramMask);

	// step one cycle
	// return true if cpu completed an instruction
	bool Step();

	// helper to set/get memory in the current emulation
	u8 GetByte(u16 addr) { return m_mem->ReadByte(addr); }
	void SetByte(u16 addr, u8 val) { m_mem->WriteByte(addr,val); }
	u8 GetByteVic(u16 addr) { return m_mem->ReadByteVic(addr); }


	void AddBreakpoint(u16 addr, u16 size, u8 breakpointType);
	void RemoveBreakpoint(u16 addr, u16 size, u8 breakpointType);
	void ToggleBreakpoint(u16 addr, u16 size, u8 breakpointType);
	void CheckBreakpoint(u8 breakpointType, u16 addr, u8 val);
	void ClearAllBreakpoints() { m_breakpoints.clear(); }

	// checks if a breakpoint exists.  Returns the type, and size
	u8 FindBreakpoint(u16 addr, u16 &size);

	bool WasBreakpointHit() { return m_breakpointHit; }
	void ClearBreakpointHit() { m_breakpointHit = false; }

	// copy regs in a thread safe way
	u16 GetCurrentPC() { return m_cpu->Regs().PC; }
	int GetCurrentRasterline() { return m_vic->CurrentRasterLine(); }
	Cpu6502* GetCpu() { return m_cpu; }
	Vic* GetVic() { return m_vic; }

	void OnKeyDown(u32 sym, u32 mod) { m_cia1->OnKeyDown(sym, mod); }
	void OnKeyUp(u32 sym, u32 mod) { m_cia1->OnKeyUp(sym, mod); }

	void OnJoystickButtonDown(SDL_Event* e) { m_cia1->OnJoystickButtonDown(e); }
	void OnJoystickButtonUp(SDL_Event* e) { m_cia1->OnJoystickButtonUp(e); }
	void OnJoystickAxisMotion(SDL_Event* e) { m_cia1->OnJoystickAxisMotion(e); }

protected:
	struct Breakpoint
	{
		u8 type;
		u16 addr;
		u16 size;
	};
	vector<Breakpoint> m_breakpoints;

	bool m_breakpointHit;
	bool m_regsDirty;

	Cpu6502* m_cpu;
	Vic* m_vic;
	Cia1* m_cia1;
	Cia2* m_cia2;
	MemC64* m_mem;
	Sid* m_sid;
};


