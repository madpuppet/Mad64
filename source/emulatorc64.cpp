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

	m_cpu->SetMemReadByte(DELEGATE_EX(m_mem, MemC64::ReadByte));
	m_cpu->SetMemWriteByte(DELEGATE_EX(m_mem, MemC64::WriteByte));

	m_vic->SetReadVicByte(DELEGATE_EX(m_mem, MemC64::ReadVicBankByte));
	m_vic->SetReadColorByte(DELEGATE_EX(m_mem, MemC64::ReadColorRamByte));
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

