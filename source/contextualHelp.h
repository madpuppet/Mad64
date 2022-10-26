#pragma once

#include "compiler.h"

struct CommandHelp
{
	const char* name;
	const char* help;
};
extern CommandHelp gHELP_CMD[];


extern const char* gHELP_LT_Address[];
extern const char* gHELP_LT_Import[];
extern const char* gHELP_LT_Label[];
extern const char* gHELP_LT_Variable[];
extern const char* gHELP_LT_DataBytes[];
extern const char* gHELP_LT_DataWords[];
extern const char* gHELP_LT_DataText[];
extern const char* gHELP_LT_GenerateBytes[];
extern const char* gHELP_LT_GenerateWords[];
extern const char* gHELP_LT_BasicStartup[];

extern const char* gHELP_OPC_ADC[];
extern const char* gHELP_OPC_AND[];
extern const char* gHELP_OPC_ASL[];
extern const char* gHELP_OPC_BCC[];
extern const char* gHELP_OPC_BCS[];
extern const char* gHELP_OPC_BEQ[];
extern const char* gHELP_OPC_BIT[];
extern const char* gHELP_OPC_BMI[];
extern const char* gHELP_OPC_BNE[];
extern const char* gHELP_OPC_BPL[];
extern const char* gHELP_OPC_BRK[];
extern const char* gHELP_OPC_BVC[];
extern const char* gHELP_OPC_BVS[];
extern const char* gHELP_OPC_CLC[];
extern const char* gHELP_OPC_CLD[];
extern const char* gHELP_OPC_CLI[];
extern const char* gHELP_OPC_CLV[];
extern const char* gHELP_OPC_CMP[];
extern const char* gHELP_OPC_CPX[];
extern const char* gHELP_OPC_CPY[];
extern const char* gHELP_OPC_DEC[];
extern const char* gHELP_OPC_DEX[];
extern const char* gHELP_OPC_DEY[];
extern const char* gHELP_OPC_EOR[];
extern const char* gHELP_OPC_INC[];
extern const char* gHELP_OPC_INX[];
extern const char* gHELP_OPC_INY[];
extern const char* gHELP_OPC_JMP[];
extern const char* gHELP_OPC_JSR[];
extern const char* gHELP_OPC_LDA[];
extern const char* gHELP_OPC_LDX[];
extern const char* gHELP_OPC_LDY[];
extern const char* gHELP_OPC_LSR[];
extern const char* gHELP_OPC_NOP[];
extern const char* gHELP_OPC_ORA[];
extern const char* gHELP_OPC_PHA[];
extern const char* gHELP_OPC_PHP[];
extern const char* gHELP_OPC_PLA[];
extern const char* gHELP_OPC_PLP[];
extern const char* gHELP_OPC_ROL[];
extern const char* gHELP_OPC_ROR[];
extern const char* gHELP_OPC_RTI[];
extern const char* gHELP_OPC_RTS[];
extern const char* gHELP_OPC_SBC[];
extern const char* gHELP_OPC_SEC[];
extern const char* gHELP_OPC_SED[];
extern const char* gHELP_OPC_SEI[];
extern const char* gHELP_OPC_STA[];
extern const char* gHELP_OPC_STX[];
extern const char* gHELP_OPC_STY[];
extern const char* gHELP_OPC_TAX[];
extern const char* gHELP_OPC_TAY[];
extern const char* gHELP_OPC_TSX[];
extern const char* gHELP_OPC_TXA[];
extern const char* gHELP_OPC_TXS[];
extern const char* gHELP_OPC_TYA[];
