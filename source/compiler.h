#pragma once

class Compiler
{
public:
	Compiler();

	// return opcode index or -1 if not an opcode
	int FindOpCode(const char* text);
};

