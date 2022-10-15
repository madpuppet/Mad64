#include "common.h"
#include "compiler.h"

Compiler::Compiler()
{
}

const char* ops[] = { "lda", "sta", "rts", "inc", "dec", "ror", "lsr", "asl", "bcc", "bcs", "jmp", "jsr", "tay", "tya", "tax", "txa", "pha", "pla" };

int Compiler::FindOpCode(const char* text)
{
	for (int i = 0; i < 3; i++)
		if (SDL_strcasecmp(text, ops[i]) == 0)
			return i;
	return -1;
}
