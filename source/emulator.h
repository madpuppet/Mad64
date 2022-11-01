#pragma once

class Emulator 
{
public:
	Emulator();
	~Emulator();

	void BuildRam(CompilerSourceInfo *src);
	void ConvertSnapshot();
	void Update();
	void Draw();

protected:
	u8 m_bootRam[65536];
	u8 m_ram[65536];
}


;