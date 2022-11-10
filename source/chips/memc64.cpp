#include "common.h"
#include "memc64.h"
#include "c64ram.h"

void MemC64::Reset(u8* mem, u8* memMask)
{
	// reset ram to base memory + code memory using mask to tell what code memory is used
	u64* out = (u64*)m_ram;
	u64* in = (u64*)mem;
	u64* in_base = (u64*)gC64_ramState;
	u64* in_mask = (u64*)memMask;
	u64* out_end = (u64*)(m_ram + 65536);
	while (out < out_end)
	{
		u64 mask = *in_mask++;
		u64 base = *in_base++;
		u64 codeMem = *in++;
		*out++ = (base & ~mask) | (codeMem & mask);
	}
}

u8 MemC64::ReadByte(u16 addr)
{
	if (addr >= 0xd000 && addr < 0xd100)
		return ReadVicRegByte(addr - 0xd000);
	else
		return m_ram[addr];
}

u8 MemC64::ReadVicBankByte(u16 addr)
{
	addr &= 0x3fff;
	if (addr >= 0x1000 && addr < 0x2000)
	{
		return gC64_charRom[addr - 0x1000];
	}
	else
	{
		return m_ram[addr];
	}
}

u8 MemC64::ReadColorRamByte(u16 addr)
{
	return m_ram[0xd800 + (addr&0x3ff)];
}

void MemC64::WriteByte(u16 addr, u8 value)
{
	if (addr >= 0xd000 && addr < 0xd100)
		WriteVicRegByte(addr - 0xd000, value);
	else
		m_ram[addr] = value;
}


