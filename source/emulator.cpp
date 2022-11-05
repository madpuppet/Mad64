#include "common.h"
#include "emulator.h"

Emulator::Emulator()
{
}

Emulator::~Emulator()
{
}

void Emulator::BuildRam(CompilerSourceInfo* csi)
{
	for (auto line : csi->m_lines)
	{
		if (line->data.size())
		{
			memcpy(m_ram + line->memAddr, line->data.data(), line->data.size());
		}
	}
}

void Emulator::Step()
{


}

void Emulator::ConvertSnapshot()
{
	FILE* fh = fopen("c64.vsf","rb");
	if (fh)
	{
		fseek(fh, 0, SEEK_END);
		size_t size = ftell(fh);
		fseek(fh, 0, SEEK_SET);

		u8* data = new u8[size];
		fread(data, size, 1, fh);
		fclose(fh);

		// parse
		FILE* fout = fopen("source/c64ram.cpp", "w");
		if (fout)
		{
			fprintf(fout, "#include \"common.h\"\n#include \"emulator.h\"\n\n");

			u8* chunk = data + 58;
			while (chunk < data + size)
			{
				const char* moduleName = (const char*)chunk;
				int moduleSize = *((int*)(chunk + 0x12));
				u8* moduleData = chunk + 0x16;

				if (SDL_strcmp(moduleName, "MAINCPU") == 0)
				{
					fprintf(fout, "// MAINCPU\n");
					fprintf(fout, "Cpu6502State gC64_cpuState = {\n");
					fprintf(fout, "  %d,   // PC\n", *((u16*)(chunk + 0x1E)));
					fprintf(fout, "  %d,   // A\n", chunk[0x1a]);
					fprintf(fout, "  %d,   // X\n", chunk[0x1b]);
					fprintf(fout, "  %d,   // Y\n", chunk[0x1c]);
					fprintf(fout, "  %d,   // SR\n", chunk[0x20]);
					fprintf(fout, "  %d    // SP\n", chunk[0x1D]);
					fprintf(fout, "};\n\n");
				}
				else if (SDL_strcmp(moduleName, "C64MEM") == 0)
				{
					fprintf(fout, "// C64MEM\n");
					fprintf(fout, "u8 gC64_ramState[65536] = {\n");
					for (int i = 0; i < 65536; i+=16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							fprintf(fout, "0x%02x%c", chunk[0x1a + i + ii], (i+ii)!=65535 ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
				}
				else if (SDL_strcmp(moduleName, "C64ROM") == 0)
				{
					fprintf(fout, "// C64ROM\n");
					fprintf(fout, "u8 gC64_kernalRom[8192] = {\n");
					for (int i = 0; i < 8192; i += 16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							fprintf(fout, "0x%02x%c", chunk[0x16 + i + ii], (i + ii) != 8191 ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
					fprintf(fout, "u8 gC64_basicRom[8192] = {\n");
					for (int i = 0; i < 8192; i += 16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							fprintf(fout, "0x%02x%c", chunk[0x2016 + i + ii], (i + ii) != 8191 ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
					fprintf(fout, "u8 gC64_chargenRom[4096] = {\n");
					for (int i = 0; i < 4096; i += 16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							fprintf(fout, "0x%02x%c", chunk[0x4016 + i + ii], (i + ii) != 4095 ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
				}
				else if (SDL_strcmp(moduleName, "VIC-II") == 0)
				{
					fprintf(fout, "// VIC-II\n");
					fprintf(fout, "u8 gC64_vicIIState[0x4c7] = {\n");
					for (int i = 0; i < 0x4c7; i += 16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							if (i+ii < 0x4c7)
								fprintf(fout, "0x%02x%c", chunk[0x12 + i + ii], (i + ii) != 0x4c6 ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
				}
				else if (SDL_strcmp(moduleName, "CIA1") == 0)
				{
					fprintf(fout, "// CIA1\n");
				}
				else if (SDL_strcmp(moduleName, "CIA2") == 0)
				{
					fprintf(fout, "// CIA2\n");
				}
				else if (SDL_strcmp(moduleName, "SID") == 0)
				{
					fprintf(fout, "// SID\n");
					fprintf(fout, "u8 gC64_sidState[0x4c6] = {\n");
					for (int i = 0; i < 0x20; i += 16)
					{
						fprintf(fout, "  ");
						for (int ii = 0; ii < 16; ii++)
						{
							fprintf(fout, "0x%02x%c", chunk[0x12 + i + ii], (i + ii) != 0x1f ? ',' : ' ');
						}
						fprintf(fout, "   // 0x%04x\n", i);
					}
					fprintf(fout, "};\n\n");
				}
				chunk += moduleSize;
			}
			fclose(fout);
		}
	}
}

void Emulator::Update()
{
}

void Emulator::Draw()
{
}

