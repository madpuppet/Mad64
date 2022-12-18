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
	m_vicBank = 0;

	// set the stack
	m_ram[0x1f7] = 0x46;
	m_ram[0x1f8] = 0xe1;
	m_ram[0x1f9] = 0xe9;
	m_ram[0x1fa] = 0xa7;
	m_ram[0x1fb] = 0xa7;
	m_ram[0x1fc] = 0x79;
	m_ram[0x1fd] = 0xa6;
	m_ram[0x1fe] = 0x9c;
	m_ram[0x1ff] = 0xe3;
}

u16 MemC64::GetKernelResetAddr()
{
	return gC64_kernalRom[0x1ffc] + (((u16)gC64_kernalRom[0x1ffd]) << 8);
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
				return ReadCia2Byte(addr);
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

u8 MemC64::ReadByteVic(u16 addr)
{
	if ((addr >= 0x1000 && addr < 0x2000) || (addr >= 0x9000 && addr < 0xa000))
	{
		return gC64_charRom[addr & 0xfff];
	}
	else
	{
		return m_ram[addr];
	}
}

u8 MemC64::ReadVicBankByte(u16 addr)
{
	addr = (addr & 0x3fff) + (m_vicBank & 3) * 16384;
	if ((addr >= 0x1000 && addr < 0x2000) || (addr >= 0x9000 && addr < 0xa000))
	{
		return gC64_charRom[addr & 0xfff];
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
				WriteCia2Byte(addr, val);
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


