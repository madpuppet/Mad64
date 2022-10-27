#pragma once

#include "compiler.h"

struct CommandHelp
{
	const char* name;
	const char* help;
	const char* detail1, * detail2;
};
extern CommandHelp gHELP_CMD[];

extern const char* gHELP_VIC_CONTROL1[];
extern const char* gHELP_VIC_CONTROL2[];
extern const char* gHELP_VIC_SCRMEM[];
extern const char* gHELP_VIC_IRQREQ[];
extern const char* gHELP_VIC_INTENA[];

