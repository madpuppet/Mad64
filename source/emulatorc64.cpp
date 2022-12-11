#include "common.h"
#include "emulatorc64.h"
#include "compiler.h"
#include "dockableWindow_log.h"

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
	m_mem->SetReadCia1Byte(DELEGATE_EX(m_cia1, Cia1::ReadReg));
	m_mem->SetWriteCia1Byte(DELEGATE_EX(m_cia1, Cia1::WriteReg));
	m_mem->SetReadCia2Byte(DELEGATE_EX(m_cia2, Cia2::ReadReg));
	m_mem->SetWriteCia2Byte(DELEGATE_EX(m_cia2, Cia2::WriteReg));

	m_cpu->SetMemReadByte(DELEGATE_EX(m_mem, MemC64::ReadByte));
	m_cpu->SetMemWriteByte(DELEGATE_EX(m_mem, MemC64::WriteByte));
	m_cpu->SetBreakpointCheck(DELEGATE(EmulatorC64::CheckBreakpoint));

	m_vic->SetReadVicByte(DELEGATE_EX(m_mem, MemC64::ReadVicBankByte));
	m_vic->SetTriggerInterrupt(DELEGATE_EX(m_cpu, Cpu6502::TriggerInterrupt));

	m_cia1->SetTriggerInterrupt(DELEGATE_EX(m_cpu, Cpu6502::TriggerInterrupt));
	m_cia2->SetTriggerInterrupt(DELEGATE_EX(m_cpu, Cpu6502::TriggerNMInterrupt));
	m_cia2->SetVicBank(DELEGATE_EX(m_mem, MemC64::SetVicBank));

	m_regsDirty = false;
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

	m_regsDirty = true;
}

void EmulatorC64::ColdReset(u8* ram, u8* ramMask)
{
	Reset(ram, ramMask, m_mem->GetKernelResetAddr());
}

bool EmulatorC64::Step()
{
	m_regsDirty = true;
	m_vic->Step();
	m_sid->Step();
	m_cia1->Step();
	m_cia2->Step();
	return m_cpu->Step();
}

void EmulatorC64::Update()
{
	if (m_regsDirty)
	{
		auto winRegs = gApp->GetWindowRegisters();
		if (gApp->GetDockableMgr()->IsWindowEnabled(winRegs))
		{
			// update registers
			auto cpu = gApp->GetEmulator()->GetCpu();
			auto vic = gApp->GetEmulator()->GetVic();
			Cpu6502::Registers& cpuRegs = cpu->Regs();
			Vic::Registers& vicRegs = vic->Regs();
			int rasterLine = vic->CurrentRasterLine();
			int rasterRow = vic->CurrentRasterRow();

			winRegs->Clear();
			winRegs->LogText("CYCLE       PC   SR SP  A  X  Y   N V - B D I Z C",-1,0);
			winRegs->LogText(FormatString("%08x    %04x %02x %02x  %02x %02x %02x  %d %d   %d %d %d %d %d",
				cpuRegs.frameCycle, cpuRegs.PC, cpuRegs.SR, cpuRegs.SP, cpuRegs.A, cpuRegs.X, cpuRegs.Y,
				(cpuRegs.SR & Cpu6502::SR_Negative) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Overflow) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Break) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Decimal) ? 1 : 0,
				(cpuRegs.SR & Cpu6502::SR_Interrupt) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Zero) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Carry) ? 1 : 0),-1,0);

			winRegs->LogText("ROW COL     XSC YSC   CSL RSL   ECM MCM BMM DEN   SSC SDC",-1,0);
			winRegs->LogText(FormatString("%03x %03x      %d   %d     %d   %d     %d   %d   %d   %d   %02x  %02x",
				rasterLine, rasterRow,
				vicRegs.control2 & Vic::XSCROLL, vicRegs.control1 & Vic::YSCROLL,
				(vicRegs.control2 & Vic::CSEL) ? 1 : 0, (vicRegs.control1 & Vic::RSEL) ? 1 : 0,
				(vicRegs.control1 & Vic::ECM) ? 1 : 0, (vicRegs.control2 & Vic::MCM) ? 1 : 0, (vicRegs.control1 & Vic::BMM) ? 1 : 0,
				(vicRegs.control1 & Vic::DEN) ? 1 : 0,
				vicRegs.spriteSpriteCollision, vicRegs.spriteDataCollision),-1,0);

			u8 ram[3];
			ram[0] = gApp->GetEmulator()->GetByte(cpuRegs.PC);
			ram[1] = gApp->GetEmulator()->GetByte(cpuRegs.PC + 1);
			ram[2] = gApp->GetEmulator()->GetByte(cpuRegs.PC + 2);
			string disasm = m_cpu->Disassemble(cpuRegs.PC);
			winRegs->LogText(FormatString("PC: %s", disasm.c_str()),-1,0);
		}
	}
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

