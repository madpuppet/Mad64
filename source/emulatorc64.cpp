#include "common.h"
#include "emulatorc64.h"
#include "compiler.h"

EmulatorC64::EmulatorC64()
{
	m_cia1 = new Cia1();
	m_cia2 = new Cia2();
	m_mem = new MemC64();
	m_cpu = new Cpu6502();
	m_vic = new Vic();

	// hook chips up to communicate with each other in a way that they don't need to know what the chips are
	m_mem->SetReadVicRegByte(DELEGATE_EX(m_vic, Vic::ReadVicRegByte));
	m_mem->SetWriteVicRegByte(DELEGATE_EX(m_vic, Vic::WriteVicRegByte));
	m_mem->SetReadVicColorByte(DELEGATE_EX(m_vic, Vic::ReadVicColorByte));
	m_mem->SetWriteVicColorByte(DELEGATE_EX(m_vic, Vic::WriteVicColorByte));

	m_cpu->SetMemReadByte(DELEGATE_EX(m_mem, MemC64::ReadByte));
	m_cpu->SetMemWriteByte(DELEGATE_EX(m_mem, MemC64::WriteByte));
	m_cpu->SetBreakpointCheck(DELEGATE(EmulatorC64::CheckBreakpoint));

	m_vic->SetReadVicByte(DELEGATE_EX(m_mem, MemC64::ReadVicBankByte));
	m_vic->SetTriggerInterrupt(DELEGATE_EX(m_cpu, Cpu6502::TriggerInterrupt));
}

EmulatorC64::~EmulatorC64()
{
}

void EmulatorC64::Reset(u8* ram, u8* ramMask, u16 cpuStart)
{
	m_mem->Reset(ram, ramMask);
	m_cpu->Reset(cpuStart);
	m_cia1->Reset();
	m_cia2->Reset();
	m_vic->Reset();
	m_sid->Reset();
}

bool EmulatorC64::Step()
{
	m_vic->Step();
	m_sid->Step();
	m_cia1->Step();
	m_cia2->Step();
	return m_cpu->Step();
}

void EmulatorC64::Update()
{
}

void EmulatorC64::Draw()
{
}

u8 EmulatorC64::FindBreakpoint(u16 addr, u16 &size)
{
	for (auto& brk : m_breakpoints)
	{
		if (contains(brk.addr, brk.addr + brk.size, addr))
		{
			size = brk.size;
			return brk.type;
		}
	}
	size = 0;
	return 0;
}

void EmulatorC64::AddBreakpoint(u16 addr, u16 size, u8 breakpointType)
{
	for (auto& brk : m_breakpoints)
	{
		if (brk.addr == addr && brk.size == size)
		{
			brk.type |= breakpointType;
			return;
		}
	}

	Breakpoint brk;
	brk.addr = addr;
	brk.size = size;
	brk.type = breakpointType;
	m_breakpoints.push_back(brk);
}

void EmulatorC64::RemoveBreakpoint(u16 addr, u16 size, u8 breakpointType)
{
	for (auto brkIt = m_breakpoints.begin(); brkIt != m_breakpoints.end(); brkIt++)
	{
		auto &brk = *brkIt;
		if (brk.addr == addr && brk.size == size)
		{
			brk.type &= ~breakpointType;
			if (brk.type == 0)
			{
				m_breakpoints.erase(brkIt);
				return;
			}
		}
	}
}

void EmulatorC64::ToggleBreakpoint(u16 addr, u16 size, u8 breakpointType)
{
	for (auto brkIt = m_breakpoints.begin(); brkIt != m_breakpoints.end(); brkIt++)
	{
		auto& brk = *brkIt;
		if (brk.addr == addr && brk.size == size)
		{
			brk.type ^= breakpointType;
			if (brk.type == 0)
			{
				m_breakpoints.erase(brkIt);
				return;
			}
		}
	}

	Breakpoint brk;
	brk.addr = addr;
	brk.size = size;
	brk.type = breakpointType;
	m_breakpoints.push_back(brk);
}

void EmulatorC64::CheckBreakpoint(u8 breakpointType, u16 addr, u8 val)
{
	for (auto &brk : m_breakpoints)
	{
		if (contains(brk.addr, brk.addr + brk.size, addr) && (brk.type & breakpointType))
		{
			m_breakpointHit = true;
		}
	}
}


