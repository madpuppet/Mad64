#pragma once

#include "compiler.h"

struct CommandHelp
{
	const char* name;
	const char* help;
	const char* detail1, * detail2;
};
extern CommandHelp gHELP_CMD[];

extern const char* gHELP_CPU_MEMBANK[];

extern const char* gHELP_VIC_CONTROL1[];
extern const char* gHELP_VIC_CONTROL2[];
extern const char* gHELP_VIC_SCRMEM[];
extern const char* gHELP_VIC_IRQREQ[];
extern const char* gHELP_VIC_INTENA[];

extern const char* gHELP_ROM_ACPTR[];
extern const char* gHELP_ROM_CHKIN[];
extern const char* gHELP_ROM_CHKOUT[];
extern const char* gHELP_ROM_CHRIN[];
extern const char* gHELP_ROM_CHROUT[];
extern const char* gHELP_ROM_CIOUT[];
extern const char* gHELP_ROM_CINT[];
extern const char* gHELP_ROM_CLALL[];
extern const char* gHELP_ROM_CLOSE[];
extern const char* gHELP_ROM_CLRCHN[];
extern const char* gHELP_ROM_GETIN[];
extern const char* gHELP_ROM_IOBASE[];
extern const char* gHELP_ROM_IOINIT[];
extern const char* gHELP_ROM_LISTEN[];
extern const char* gHELP_ROM_LOAD[];
extern const char* gHELP_ROM_MEMBOT[];
extern const char* gHELP_ROM_MEMTOP[];
extern const char* gHELP_ROM_OPEN[];
extern const char* gHELP_ROM_PLOT[];
extern const char* gHELP_ROM_RAMTAS[];
extern const char* gHELP_ROM_RDTIM[];
extern const char* gHELP_ROM_READST[];
extern const char* gHELP_ROM_RESTOR[];
extern const char* gHELP_ROM_SAVE[];
extern const char* gHELP_ROM_SCNKEY[];
extern const char* gHELP_ROM_SCREEN[];
extern const char* gHELP_ROM_SECOND[];
extern const char* gHELP_ROM_SETLFS[];
extern const char* gHELP_ROM_SETMSG[];
extern const char* gHELP_ROM_SETNAM[];
extern const char* gHELP_ROM_SETTIM[];
extern const char* gHELP_ROM_SETTMO[];
extern const char* gHELP_ROM_STOP[];
extern const char* gHELP_ROM_TALK[];
extern const char* gHELP_ROM_TKSA[];
extern const char* gHELP_ROM_UDTIM[];
extern const char* gHELP_ROM_UNLSN[];
extern const char* gHELP_ROM_UNTLK[];
extern const char* gHELP_ROM_VECTOR[];


