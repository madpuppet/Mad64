#pragma once

#include "compiler.h"

struct CommandHelp
{
	const char* name;
	const char* help;
	const char* detail1, * detail2;
};
extern CommandHelp gHELP_CMD[];
