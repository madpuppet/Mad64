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

	m_ram[0] = 7;
	m_ram[1] = 7;
}

u8 MemC64::ReadByte(u16 addr)
{
	if (addr >= 0xa000 && addr < 0xc000)
	{
		if ((m_ram[1] & 3) == 3)
		{
			return gC64_basicRom[addr - 0xa000];
		}
		else
		{
			return m_ram[addr];
		}
	}
	else if (addr >= 0xd000 && addr < 0xe000)
	{
		if ((m_ram[1] & 7) > 4)
		{
			// I/O
			if (addr >= 0xd000 && addr < 0xd400)
			{
				// vic registers
				return ReadVicRegByte(addr - 0xd000);
			}
			else if (addr >= 0xd400 && addr < 0xd800)
			{
				// sid
				return 0;
			}
			else if (addr >= 0xd800 && addr < 0xdc00)
			{
				// vic color ram
				return ReadVicColorByte(addr - 0xd800);
			}
			else if (addr >= 0xdc00 && addr < 0xdd00)
			{
				// cia 1
				return ReadCia1Byte(addr);
			}
			else if (addr >= 0xdd00 && addr < 0xde00)
			{
				// cia 2
				return 0;// ReadCia2Byte(addr);
			}
			else if (addr >= 0xde00 && addr < 0xdf00)
			{
				// i/o 1
				return 0;
			}
			else
			{
				// i/o 2
				return 0;
			}
		}
		else if (m_ram[1] & 2)
		{
			// char rom
			return gC64_charRom[addr - 0xd000];
		}
		else
		{
			return m_ram[addr];
		}
	}
	else if (addr >= 0xe000 && addr < 0xffff)
	{
		if (m_ram[1] & 2)
		{
			return gC64_kernalRom[addr - 0xe000];
		}
		else
		{
			return m_ram[addr];
		}
	}
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

void MemC64::WriteByte(u16 addr, u8 val)
{
	if (addr >= 0xd000 && addr < 0xe000)
	{
		if ((m_ram[1] & 7) > 4)
		{
			// I/O
			if (addr >= 0xd000 && addr < 0xd400)
			{
				// vic registers
				WriteVicRegByte(addr - 0xd000, val);
			}
			else if (addr >= 0xd400 && addr < 0xd800)
			{
				// sid
				return;
			}
			else if (addr >= 0xd800 && addr < 0xdc00)
			{
				// vic color ram
				WriteVicColorByte(addr - 0xd800, val);
			}
			else if (addr >= 0xdc00 && addr < 0xdd00)
			{
				// cia 1
				WriteCia1Byte(addr, val);
				return;
			}
			else if (addr >= 0xdd00 && addr < 0xde00)
			{
				// cia 2
				//WriteCia2Byte(addr, val);
				return;
			}
			else if (addr >= 0xde00 && addr < 0xdf00)
			{
				// i/o 1
				return;
			}
			else
			{
				// i/o 2
				return;
			}
		}
	}
	m_ram[addr] = val;
}


