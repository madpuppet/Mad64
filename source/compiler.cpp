#include "common.h"
#include "compiler.h"
#include "graphicChunk.h"
#include <algorithm>
#include "contextualHelp.h"

char s_asciiToScreenCode[] = {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 00
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 10
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 20
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 30
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 40
    0,0,0,0,0,0,0,0, 0,0,0,27,0,0,0,0,       // 50
    0,1,2,3,4,5,6,7, 8,9,10,11,12,13,14,15,       // 60
    16,17,18,19,20,21,22,23, 24,25,26,0,0,0,0,0,       // 70
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 80
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // 90
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // a0
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // b0
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // c0
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // d0
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,       // e0
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0        // f0
};

CompilerLabel s_systemLabels[] =
{
    CompilerLabel("PI", M_PI), CompilerLabel("I", 0),

    CompilerLabel("vic.sprite0X", 0xd000),  CompilerLabel("vic.sprite0Y", 0xd001),  CompilerLabel("vic.sprite1X", 0xd002),  CompilerLabel("vic.sprite1Y", 0xd003),
    CompilerLabel("vic.sprite2X", 0xd004),  CompilerLabel("vic.sprite2Y", 0xd005),  CompilerLabel("vic.sprite3X", 0xd006),  CompilerLabel("vic.sprite3Y", 0xd007),
    CompilerLabel("vic.sprite4X", 0xd008),  CompilerLabel("vic.sprite4Y", 0xd009),  CompilerLabel("vic.sprite5X", 0xd00a),  CompilerLabel("vic.sprite5Y", 0xd00b),
    CompilerLabel("vic.sprite6X", 0xd00c),  CompilerLabel("vic.sprite6Y", 0xd00d),  CompilerLabel("vic.sprite7X", 0xd00e),  CompilerLabel("vic.sprite7Y", 0xd00f),
    CompilerLabel("vic.spriteXMSB", 0xd010), CompilerLabel("vic.control1", 0xd011), CompilerLabel("vic.rasterCounter", 0xd012), CompilerLabel("vic.lightPenX", 0xd013),
    CompilerLabel("vic.lightPenY", 0xd014), CompilerLabel("vic.spriteEnable", 0xd015), CompilerLabel("vic.control2", 0xd016), CompilerLabel("vic.spriteYSize", 0xd017),
    CompilerLabel("vic.memoryPointer", 0xd018), CompilerLabel("vic.interruptRegister", 0xd019), CompilerLabel("vic.interruptEnable", 0xd01a), CompilerLabel("vic.spritePriority", 0xd01b),
    CompilerLabel("vic.spriteMulticolor", 0xd01c), CompilerLabel("vic.spriteXSize", 0xd01d), CompilerLabel("vic.spriteToSpriteCollision", 0xd01e), CompilerLabel("vic.spriteToDataCollision", 0xd01f),
    CompilerLabel("vic.borderColor", 0xd020), CompilerLabel("vic.backgroundColor0", 0xd021), CompilerLabel("vic.backgroundColor1", 0xd022), CompilerLabel("vic.backgroundColor2", 0xd023),
    CompilerLabel("vic.backgroundColor3", 0xd024), CompilerLabel("vic.spriteMulticolor0", 0xd025), CompilerLabel("vic.spriteMulticolor1", 0xd026), CompilerLabel("vic.sprite0Color", 0xd027),
    CompilerLabel("vic.sprite1Color", 0xd028), CompilerLabel("vic.sprite2Color", 0xd029), CompilerLabel("vic.sprite3Color", 0xd02a), CompilerLabel("vic.sprite4Color", 0xd02b),
    CompilerLabel("vic.sprite5Color", 0xd02c), CompilerLabel("vic.sprite6Color", 0xd02d), CompilerLabel("vic.sprite7Color", 0xd02e), CompilerLabel("vic.colorMemory", 0xd800),

    CompilerLabel("cia1.dataPortA", 0xdc00), CompilerLabel("cia1.dataPortB", 0xdc01), CompilerLabel("cia1.dataDirectionA", 0xdc02), CompilerLabel("cia1.dataDirectionB", 0xdc03),
    CompilerLabel("cia1.timerALow", 0xdc04), CompilerLabel("cia1.timerAHigh", 0xdc05), CompilerLabel("cia1.timerBLow", 0xdc06), CompilerLabel("cia1.timerBHigh", 0xdc07),
    CompilerLabel("cia1.clockTenths", 0xdc08), CompilerLabel("cia1.clockSeconds", 0xdc09), CompilerLabel("cia1.clockMinutes", 0xdc0a), CompilerLabel("cia1.clockHours", 0xdc0b),
    CompilerLabel("cia1.serialShift", 0xdc0c), CompilerLabel("cia1.interruptControl", 0xdc0d), CompilerLabel("cia1.controlTimerA", 0xdc0e), CompilerLabel("cia1.controlTimerB", 0xdc0f),

    CompilerLabel("cia2.dataPortA", 0xdd00), CompilerLabel("cia2.dataPortB", 0xdd01), CompilerLabel("cia2.dataDirectionA", 0xdd02), CompilerLabel("cia2.dataDirectionB", 0xdd03),
    CompilerLabel("cia2.timerALow", 0xdd04), CompilerLabel("cia2.timerAHigh", 0xdd05), CompilerLabel("cia2.timerBLow", 0xdd06), CompilerLabel("cia2.timerBHigh", 0xdd07),
    CompilerLabel("cia2.clockTenths", 0xdd08), CompilerLabel("cia2.clockSeconds", 0xdd09), CompilerLabel("cia2.clockMinutes", 0xdd0a), CompilerLabel("cia2.clockHours", 0xdd0b),
    CompilerLabel("cia2.serialShift", 0xdd0c), CompilerLabel("cia2.interruptControl", 0xdd0d), CompilerLabel("cia2.controlTimerA", 0xdd0e), CompilerLabel("cia2.controlTimerB", 0xdd0f),

    CompilerLabel("rom.SETLFS", 0xffba), CompilerLabel("rom.SETNAM", 0xffbd), CompilerLabel("rom.LOAD", 0xffd5),

    CompilerLabel("sid.v1_freqLow", 0xd400), CompilerLabel("sid.v1_freqHi", 0xd401), CompilerLabel("sid.v1_pulseWaveformWidthLo", 0xd402), CompilerLabel("sid.v1_pulseWaveformWidthHi", 0xd403),
    CompilerLabel("sid.b1_Control", 0xd404), CompilerLabel("sid.v1_envelope_ad", 0xd405), CompilerLabel("sid.v1_envelope_sr", 0xd406), CompilerLabel("sid.v2_freqLow", 0xd407),
    CompilerLabel("sid.v2_freqHi", 0xd408), CompilerLabel("sid.v2_pulseWaveformWidthLo", 0xd409), CompilerLabel("sid.v2_pulseWaveformWidthHi", 0xd40a), CompilerLabel("sid.v2_control", 0xd40b),
    CompilerLabel("sid.v2_envelope_ad", 0xd40c), CompilerLabel("sid.v2_envelope_sr", 0xd40d), CompilerLabel("sid.v3_freqLow", 0xd40e), CompilerLabel("sid.v3_freqHi", 0xd40f),
    CompilerLabel("sid.v3_pulseWaveformWidthLo", 0xd410), CompilerLabel("sid.v3_pulseWaveformWidthHi", 0xd411), CompilerLabel("sid.v3_control", 0xd412), CompilerLabel("sid.v3_envelope_ad", 0xd413),
    CompilerLabel("sid.v3_envelope_sr", 0xd414), CompilerLabel("sid.filterCutoffFreqLo", 0xd415), CompilerLabel("sid.filterCutoffFreqHi", 0xd416), CompilerLabel("sid.filterResonance", 0xd417),
    CompilerLabel("sid.filterModeAndVolume", 0xd418), CompilerLabel("sid.analogDigitalConvert1", 0xd419), CompilerLabel("sid.analogDigitalConvert2", 0xd41a), CompilerLabel("sid.oscillator3Random", 0xd41b),
    CompilerLabel("sid.envelope3Output", 0xd41c)
};



double ExprAdd(double a, double b, double c)
{
    return a + b;
}
double ExprSub(double a, double b, double c)
{
    return a - b;
}
double ExprDiv(double a, double b, double c)
{
    if (abs(b) < 1.192092896e-20F)
        return 0.0;

    return a / b;
}
double ExprMul(double a, double b, double c)
{
    return a * b;
}
double ExprMod(double a, double b, double c)
{
    if ((u32)b == 0)
        return 0.0;

    return (double)((u32)a % (u32)b);
}
double ExprRShift(double a, double b, double c)
{
    return (double)((u32)a >> (u32)b);
}
double ExprLShift(double a, double b, double c)
{
    return (double)((u32)a << (u32)b);
}
double ExprOr(double a, double b, double c)
{
    return (double)((u32)a | (u32)b);
}
double ExprAnd(double a, double b, double c)
{
    return (double)((u32)a & (u32)b);
}
double ExprXor(double a, double b, double c)
{
    return (double)((u32)a ^ (u32)b);
}
double ExprNot(double a, double b, double c)
{
    return (double)(~(u64)a);
}
double ExprLowByte(double a, double b, double c)
{
    return (double)((u32)a & 0xff);
}
double ExprHighByte(double a, double b, double c)
{
    return (double)(((u32)a >> 8) & 0xff);
}
double ExprBoolNot(double a, double b, double c)
{
    return a ? 0.0 : 1.0;
}
double ExprBoolOr(double a, double b, double c)
{
    return (a || b) ? 1.0 : 0.0;
}
double ExprBoolAnd(double a, double b, double c)
{
    return (a && b) ? 1.0 : 0.0;
}
double ExprTernary(double a, double b, double c)
{
    return a ? b : c;
}
double ExprBoolLess(double a, double b, double c)
{
    return (a < b) ? 1.0 : 0.0;
}
double ExprBoolLessEqual(double a, double b, double c)
{
    return (a <= b) ? 1.0 : 0;
}
double ExprBoolGreater(double a, double b, double c)
{
    return (a > b) ? 1.0 : 0.0;
}
double ExprBoolGreaterEqual(double a, double b, double c)
{
    return (a >= b) ? 1.0 : 0.0;
}
double ExprBoolEqual(double a, double b, double c)
{
    return (a == b) ? 1.0 : 0.0;
}
double ExprBoolNotEqual(double a, double b, double c)
{
    return (a != b) ? 1.0 : 0.0;
}

double ExprRound(double a, double b, double c)
{
    return round(a);
}

double ExprFloor(double a, double b, double c)
{
    return floor(a);
}

double ExprFMod(double a, double b, double c)
{
    return fmod(a,b);
}

double ExprSin(double a, double b, double c)
{
    return sin(a);
}

double ExprCos(double a, double b, double c)
{
    return cos(a);
}

double ExprTan(double a, double b, double c)
{
    return tan(a);
}

double ExprATan(double a, double b, double c)
{
    return atan(a);
}

double ExprACos(double a, double b, double c)
{
    // avoid NaNs
    if (a < -1.0 || a > 1.0)
        return 0.0;

    return acos(a);
}

double ExprASin(double a, double b, double c)
{
    // avoid NaNs
    if (a < -1.0 || a > 1.0)
        return 0.0;

    return asin(a);
}

double ExprATan2(double a, double b, double c)
{
    return atan2(a,b);
}

double ExprRand(double a, double b, double c)
{
    int range = (int)(b - a + 1.0);
    if (range == 0)
        return a;

    return a + rand() % range;
}

double ExprRadToDeg(double a, double b, double c)
{
    return a * 180.0 / M_PI;
}

double ExprDegToRad(double a, double b, double c)
{
    return a * M_PI / 180.0;
}

CompilerExpressionOpcode s_exprOpcodes[] = {
    {"/",  2, 11, CEOT_Binary, &ExprDiv},
    {"*",  2, 11, CEOT_Binary, &ExprMul},
    {"%",  2, 11, CEOT_Binary, &ExprMod},
    {"+",  2, 10, CEOT_Binary, &ExprAdd},
    {"-",  2, 10, CEOT_Binary, &ExprSub},
    {">>", 2, 9, CEOT_Binary, &ExprRShift},
    {"<<", 2, 9, CEOT_Binary, &ExprLShift},
    {"<",  2, 8, CEOT_Binary, &ExprBoolLess},
    {"<=", 2, 8, CEOT_Binary, &ExprBoolLessEqual},
    {">",  2, 8, CEOT_Binary, &ExprBoolGreater},
    {">=", 2, 8, CEOT_Binary, &ExprBoolGreaterEqual},
    {"==", 2, 7, CEOT_Binary, &ExprBoolEqual},
    {"!=", 2, 7, CEOT_Binary, &ExprBoolNotEqual},
    {"&",  2, 6,  CEOT_Binary, &ExprAnd},
    {"^",  2, 5,  CEOT_Binary, &ExprXor},
    {"|",  2, 4,  CEOT_Binary, &ExprOr},
    {"&&", 2, 3,  CEOT_Binary, &ExprBoolAnd},
    {"||", 2, 2,  CEOT_Binary, &ExprBoolOr},
    {"?",  3, 1,  CEOT_Ternary, &ExprTernary}
};
CompilerExpressionOpcode s_exprPrefixOpcodes[] = {
    { "<", 1, 12, CEOT_Prefix, &ExprLowByte },
    { ">", 1, 12, CEOT_Prefix, &ExprHighByte },
    { "~",  1, 12, CEOT_Prefix, &ExprNot},
    { "!",  1, 12, CEOT_Prefix, &ExprBoolNot},
    {"round", 1, 13, CEOT_Func, &ExprRound},
    {"floor", 1, 13, CEOT_Func, &ExprFloor},
    {"mod", 2, 13, CEOT_Func, &ExprFMod},
    {"sin", 1, 13, CEOT_Func, &ExprSin},
    {"cos", 1, 13, CEOT_Func, &ExprCos},
    {"tan", 1, 13, CEOT_Func, &ExprTan},
    {"asin", 1, 13, CEOT_Func, &ExprASin},
    {"acos", 1, 13, CEOT_Func, &ExprACos},
    {"atan", 1, 13, CEOT_Func, &ExprATan},
    {"atan2", 2, 13, CEOT_Func, &ExprATan2},
    {"rand", 2, 13, CEOT_Func, &ExprRand},
    {"rad", 1, 13, CEOT_Func, &ExprDegToRad},
    {"deg", 1, 13, CEOT_Func, &ExprRadToDeg}
};

CompilerExpressionOpcode* Compiler::FindExprOpcode(string& token)
{
    for (int i = 0; i < sizeof(s_exprOpcodes) / sizeof(CompilerExpressionOpcode); i++)
    {
        if (token == s_exprOpcodes[i].text)
            return &s_exprOpcodes[i];
    }
    return nullptr;
}

CompilerExpressionOpcode* Compiler::FindPrefixExprOpcode(string& token)
{
    for (int i = 0; i < sizeof(s_exprPrefixOpcodes) / sizeof(CompilerExpressionOpcode); i++)
    {
        if (StrEqual(token, s_exprPrefixOpcodes[i].text))
            return &s_exprPrefixOpcodes[i];
    }
    return nullptr;
}

int gAddressingModeSize[] =
{
    1,  //    AM_Implied,              // operand
    2,  //    AM_Immediate,            // operand #value
    2,  //    AM_ZeroPage,             // operand value
    2,  //    AM_ZeroPageX,            // operand value,x
    3,  //    AM_ZeroPageY,            // operand value,y
    3,  //    AM_Absolute,             // operand value
    3,  //    AM_AbsoluteX,            // operand value,x
    3,  //    AM_AbsoluteY,            // operand value,y
    3,  //    AM_Indirect,             // operand (value)
    2,  //    AM_IndirectX,            // operand (value, x)
    2,  //    AM_IndirectY,            // operand (value), y
    2   //    AM_Relative              // operand value
};

const char* gAddressingModeName[] =
{
    "Implied", "Immediate", "Zero Page", "Zero Page X", "Zero Page Y", "Absolute", "Absolute X", "Absolute Y", "Indirect", "Indirect X", "Indirect Y", "Relative"
};

CompilerOpcode s_opcodesRaw[] =
{
    {"ADC", gHELP_OPC_ADC, AM_Immediate, 0x69, 2},
    {"ADC", gHELP_OPC_ADC, AM_ZeroPage,  0x65, 3},
    {"ADC", gHELP_OPC_ADC, AM_ZeroPageX, 0x75, 4},
    {"ADC", gHELP_OPC_ADC, AM_Absolute,  0x6D, 4},
    {"ADC", gHELP_OPC_ADC, AM_AbsoluteX, 0x7D, 4, true},
    {"ADC", gHELP_OPC_ADC, AM_AbsoluteY, 0x79, 4, true},
    {"ADC", gHELP_OPC_ADC, AM_IndirectX, 0x61, 6},
    {"ADC", gHELP_OPC_ADC, AM_IndirectY, 0x71, 5, true},

    {"AND", gHELP_OPC_AND, AM_Immediate, 0x29, 2},
    {"AND", gHELP_OPC_AND, AM_ZeroPage,  0x25, 3},
    {"AND", gHELP_OPC_AND, AM_ZeroPageX, 0x35, 4},
    {"AND", gHELP_OPC_AND, AM_Absolute,  0x2D, 4},
    {"AND", gHELP_OPC_AND, AM_AbsoluteX, 0x3D, 4, true},
    {"AND", gHELP_OPC_AND, AM_AbsoluteY, 0x39, 4, true},
    {"AND", gHELP_OPC_AND, AM_IndirectX, 0x21, 6},
    {"AND", gHELP_OPC_AND, AM_IndirectY, 0x31, 5, true},

    {"ASL", gHELP_OPC_ASL, AM_Implied,   0x0A, 2},
    {"ASL", gHELP_OPC_ASL, AM_ZeroPage,  0x06, 5},
    {"ASL", gHELP_OPC_ASL, AM_ZeroPageX, 0x16, 6},
    {"ASL", gHELP_OPC_ASL, AM_Absolute,  0x0E, 6},
    {"ASL", gHELP_OPC_ASL, AM_AbsoluteX, 0x1E, 7},

    {"BCC", gHELP_OPC_BCC, AM_Relative,  0x90, 2, false, true},
    {"BCS", gHELP_OPC_BCS, AM_Relative,  0xB0, 2, false, true},
    {"BEQ", gHELP_OPC_BEQ, AM_Relative,  0xF0, 2, false, true},

    {"BIT", gHELP_OPC_BIT, AM_ZeroPage,  0x24, 3},
    {"BIT", gHELP_OPC_BIT, AM_Absolute,  0x2C, 4},

    {"BMI", gHELP_OPC_BMI, AM_Relative,  0x30, 2, false, true},
    {"BNE", gHELP_OPC_BNE, AM_Relative,  0xD0, 2, false, true},
    {"BPL", gHELP_OPC_BPL, AM_Relative,  0x10, 2, false, true},

    {"BRK", gHELP_OPC_BRK, AM_Implied,   0x30, 7},

    {"BVC", gHELP_OPC_BVC, AM_Relative,  0x50, 2, false, true},
    {"BVS", gHELP_OPC_BVS, AM_Relative,  0x70, 2, false, true},

    {"CLC", gHELP_OPC_CLC, AM_Implied,   0x18, 2},
    {"CLD", gHELP_OPC_CLD, AM_Implied,   0xD8, 2},
    {"CLI", gHELP_OPC_CLI, AM_Implied,   0x58, 2},
    {"CLV", gHELP_OPC_CLV, AM_Implied,   0xB8, 2},

    {"CMP", gHELP_OPC_CMP, AM_Immediate, 0xC9, 2},
    {"CMP", gHELP_OPC_CMP, AM_ZeroPage,  0xC5, 3},
    {"CMP", gHELP_OPC_CMP, AM_ZeroPageX, 0xD5, 4},
    {"CMP", gHELP_OPC_CMP, AM_Absolute,  0xCD, 4},
    {"CMP", gHELP_OPC_CMP, AM_AbsoluteX, 0xDD, 4, true},
    {"CMP", gHELP_OPC_CMP, AM_AbsoluteY, 0xD9, 4, true},
    {"CMP", gHELP_OPC_CMP, AM_IndirectX, 0xC1, 6},
    {"CMP", gHELP_OPC_CMP, AM_IndirectY, 0xD1, 5, true},

    {"CPX", gHELP_OPC_CPX, AM_Immediate, 0xE0, 2},
    {"CPX", gHELP_OPC_CPX, AM_ZeroPage,  0xE4, 3},
    {"CPX", gHELP_OPC_CPX, AM_Absolute,  0xEC, 4},

    {"CPY", gHELP_OPC_CPY, AM_Immediate, 0xC0, 2},
    {"CPY", gHELP_OPC_CPY, AM_ZeroPage,  0xC4, 3},
    {"CPY", gHELP_OPC_CPY, AM_Absolute,  0xCC, 4},

    {"DEC", gHELP_OPC_DEC, AM_ZeroPage,  0xC6, 5},
    {"DEC", gHELP_OPC_DEC, AM_ZeroPageX, 0xD6, 6},
    {"DEC", gHELP_OPC_DEC, AM_Absolute,  0xCE, 6},
    {"DEC", gHELP_OPC_DEC, AM_AbsoluteX, 0xDE, 7},

    {"DEX", gHELP_OPC_DEX, AM_Implied,   0xCA, 2},
    {"DEY", gHELP_OPC_DEY, AM_Implied,   0x88, 2},

    {"EOR", gHELP_OPC_EOR, AM_Immediate, 0x49, 2},
    {"EOR", gHELP_OPC_EOR, AM_ZeroPage,  0x45, 3},
    {"EOR", gHELP_OPC_EOR, AM_ZeroPageX, 0x55, 4},
    {"EOR", gHELP_OPC_EOR, AM_Absolute,  0x4D, 4},
    {"EOR", gHELP_OPC_EOR, AM_AbsoluteX, 0x5D, 4, true},
    {"EOR", gHELP_OPC_EOR, AM_AbsoluteY, 0x59, 4, true},
    {"EOR", gHELP_OPC_EOR, AM_IndirectX, 0x41, 6},
    {"EOR", gHELP_OPC_EOR, AM_IndirectY, 0x51, 5, true},

    {"INC", gHELP_OPC_INC, AM_ZeroPage,  0xE6, 5},
    {"INC", gHELP_OPC_INC, AM_ZeroPageX, 0xF6, 6},
    {"INC", gHELP_OPC_INC, AM_Absolute,  0xEE, 6},
    {"INC", gHELP_OPC_INC, AM_AbsoluteX, 0xFE, 7},

    {"INX", gHELP_OPC_INX, AM_Implied,   0xE8, 2},
    {"INY", gHELP_OPC_INY, AM_Implied,   0xC8, 2},

    {"JMP", gHELP_OPC_JMP, AM_Absolute,  0x4C, 3},
    {"JMP", gHELP_OPC_JMP, AM_Indirect,  0x6C, 5},

    {"JSR", gHELP_OPC_JSR, AM_Absolute,  0x20, 6},

    {"LDA", gHELP_OPC_LDA, AM_Immediate, 0xA9, 2},
    {"LDA", gHELP_OPC_LDA, AM_ZeroPage,  0xA5, 3},
    {"LDA", gHELP_OPC_LDA, AM_ZeroPageX, 0xB5, 4},
    {"LDA", gHELP_OPC_LDA, AM_Absolute,  0xAD, 4},
    {"LDA", gHELP_OPC_LDA, AM_AbsoluteX, 0xBD, 4, true},
    {"LDA", gHELP_OPC_LDA, AM_AbsoluteY, 0xB9, 4, true},
    {"LDA", gHELP_OPC_LDA, AM_IndirectX, 0xA1, 6},
    {"LDA", gHELP_OPC_LDA, AM_IndirectY, 0xB1, 5, true},

    {"LDX", gHELP_OPC_LDX, AM_Immediate, 0xA2, 2},
    {"LDX", gHELP_OPC_LDX, AM_ZeroPage,  0xA6, 3},
    {"LDX", gHELP_OPC_LDX, AM_ZeroPageY, 0xB6, 4},
    {"LDX", gHELP_OPC_LDX, AM_Absolute,  0xAE, 4},
    {"LDX", gHELP_OPC_LDX, AM_AbsoluteY, 0xBE, 4, true},

    {"LDY", gHELP_OPC_LDY, AM_Immediate, 0xA0, 2},
    {"LDY", gHELP_OPC_LDY, AM_ZeroPage,  0xA4, 3},
    {"LDY", gHELP_OPC_LDY, AM_ZeroPageX, 0xB4, 4},
    {"LDY", gHELP_OPC_LDY, AM_Absolute,  0xAC, 4},
    {"LDY", gHELP_OPC_LDY, AM_AbsoluteX, 0xBC, 4, true},

    {"LSR", gHELP_OPC_LSR, AM_Implied,   0x4A, 2},
    {"LSR", gHELP_OPC_LSR, AM_ZeroPage,  0x46, 5},
    {"LSR", gHELP_OPC_LSR, AM_ZeroPageX, 0x56, 6},
    {"LSR", gHELP_OPC_LSR, AM_Absolute,  0x4E, 6},
    {"LSR", gHELP_OPC_LSR, AM_AbsoluteX, 0x5E, 7},

    {"NOP", gHELP_OPC_NOP, AM_Implied,   0xEA, 2},

    {"ORA", gHELP_OPC_ORA, AM_Immediate, 0x09, 2},
    {"ORA", gHELP_OPC_ORA, AM_ZeroPage,  0x05, 3},
    {"ORA", gHELP_OPC_ORA, AM_ZeroPageX, 0x15, 4},
    {"ORA", gHELP_OPC_ORA, AM_Absolute,  0x0D, 4},
    {"ORA", gHELP_OPC_ORA, AM_AbsoluteX, 0x1D, 4, true},
    {"ORA", gHELP_OPC_ORA, AM_AbsoluteY, 0x19, 4, true},
    {"ORA", gHELP_OPC_ORA, AM_IndirectX, 0x01, 6},
    {"ORA", gHELP_OPC_ORA, AM_IndirectY, 0x11, 5, true},

    {"PHA", gHELP_OPC_PHA, AM_Implied,   0x48, 3},
    {"PHP", gHELP_OPC_PHP, AM_Implied,   0x08, 3},
    {"PLA", gHELP_OPC_PLA, AM_Implied,   0x68, 4},
    {"PLP", gHELP_OPC_PLP, AM_Implied,   0x28, 4},

    {"ROR", gHELP_OPC_ROR, AM_Immediate, 0x6A, 2},
    {"ROR", gHELP_OPC_ROR, AM_ZeroPage,  0x66, 5},
    {"ROR", gHELP_OPC_ROR, AM_ZeroPageX, 0x76, 6},
    {"ROR", gHELP_OPC_ROR, AM_Absolute,  0x4E, 6},
    {"ROR", gHELP_OPC_ROR, AM_AbsoluteX, 0x7E, 7},

    {"RTI", gHELP_OPC_RTI, AM_Implied,   0x40, 6},
    {"RTS", gHELP_OPC_RTS, AM_Implied,   0x60, 6},

    { "SBC", gHELP_OPC_SBC, AM_Immediate, 0xE9, 2},
    { "SBC", gHELP_OPC_SBC, AM_ZeroPage,  0xE5, 3},
    { "SBC", gHELP_OPC_SBC, AM_ZeroPageX, 0xF5, 4},
    { "SBC", gHELP_OPC_SBC, AM_Absolute,  0xED, 4},
    { "SBC", gHELP_OPC_SBC, AM_AbsoluteX, 0xFD, 4, true},
    { "SBC", gHELP_OPC_SBC, AM_AbsoluteY, 0xF9, 4, true},
    { "SBC", gHELP_OPC_SBC, AM_IndirectX, 0xE1, 6},
    { "SBC", gHELP_OPC_SBC, AM_IndirectY, 0xF1, 5, true},

    { "SEC", gHELP_OPC_SEC, AM_Implied,   0x38, 2},
    { "SED", gHELP_OPC_SED, AM_Implied,   0xF8, 2 },
    { "SEI", gHELP_OPC_SEI, AM_Implied,   0x78, 2 },

    { "STA", gHELP_OPC_STA, AM_ZeroPage,  0x85, 3 },
    { "STA", gHELP_OPC_STA, AM_ZeroPageX, 0x95, 4 },
    { "STA", gHELP_OPC_STA, AM_Absolute,  0x8D, 4 },
    { "STA", gHELP_OPC_STA, AM_AbsoluteX, 0x9D, 5 },
    { "STA", gHELP_OPC_STA, AM_AbsoluteY, 0x99, 5 },
    { "STA", gHELP_OPC_STA, AM_IndirectX, 0x81, 6 },
    { "STA", gHELP_OPC_STA, AM_IndirectY, 0x91, 6 },

    { "STX", gHELP_OPC_STX, AM_ZeroPage,  0x86, 3 },
    { "STX", gHELP_OPC_STX, AM_ZeroPageY, 0x96, 4 },
    { "STX", gHELP_OPC_STX, AM_Absolute,  0x8E, 4 },

    { "STY", gHELP_OPC_STY, AM_ZeroPage,  0x84, 3 },
    { "STY", gHELP_OPC_STY, AM_ZeroPageX, 0x94, 4 },
    { "STY", gHELP_OPC_STY, AM_Absolute,  0x8C, 4 },

    { "TAX", gHELP_OPC_TAX, AM_Implied,   0xAA, 3 },
    { "TAY", gHELP_OPC_TAY, AM_Implied,   0xA8, 3 },
    { "TSX", gHELP_OPC_TSX, AM_Implied,   0xBA, 3 },
    { "TXA", gHELP_OPC_TXA, AM_Implied,   0x8A, 3 },
    { "TXS", gHELP_OPC_TXS, AM_Implied,   0x9A, 3 },
    { "TYA", gHELP_OPC_TYA, AM_Implied,   0x98, 3 }
};

vector<CompilerOpcode*> s_uniqueOpcodes;
CompilerOpcode s_opcodes[256];

bool HexToNumber(string& token, int& value)
{
    value = 0;
    for (int i = 0; i < (int)token.size(); i++)
    {
        char ch = toupper(token[i]);
        if ((ch < 'A' && ch > 'F') || (ch < '0' && ch > '9'))
            return false;

        value = value * 16;
        value += (ch >= 'A' && ch <= 'F') ? ch - 'A' + 10 : ch - '0';
    }
    return true;
}

bool BinaryToNumber(string& token, int& value)
{
    value = 0;
    for (int i = 0; i < (int)token.size(); i++)
    {
        char ch = toupper(token[i]);
        if (ch != '0' && ch != '1')
            return false;

        value = value * 2;
        value += ch == '0' ? 0 : 1;
    }
    return true;
}

Compiler::Compiler() : m_compileRequested(1,1), m_compileCompleted(1,1), Thread("Compiler")
{
    memset(m_ram, 0, sizeof(m_ram));
    memset(s_opcodes, 0, sizeof(s_opcodes));

    // copy opcode data to indexed table - we'll need this for fast emulation
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        s_opcodes[s_opcodesRaw[i].opc] = s_opcodesRaw[i];

        if ((i == 0) || (s_opcodesRaw[i - 1].name != s_opcodesRaw[i].name))
            s_uniqueOpcodes.push_back(&s_opcodesRaw[i]);
    }
}

bool Compiler::IsOpCode(const char* text)
{
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        if (StrEqual(text, s_opcodesRaw[i].name))
        {
            return true;
        }
    }
    return false;
}

CompilerOpcode* Compiler::FindOpcode(string &name, AddressingMode am)
{
    int cnt = sizeof(s_opcodesRaw) / sizeof(CompilerOpcode);
    for (int i = 0; i < cnt; i++)
    {
        if (StrEqual(name, s_opcodesRaw[i].name) && s_opcodesRaw[i].addressMode == am)
        {
            return &s_opcodesRaw[i];
        }
    }
    return nullptr;;
}

CompilerLabel* Compiler::FindLabel(CompilerSourceInfo* si, string& name, LabelResolve resolve, u32 resolveStartAddr)
{
    if (resolve == LabelResolve_Global)
    {
        for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
        {
            if (StrEqual(s_systemLabels[i].m_name, name))
            {
                return &s_systemLabels[i];
            }
        }
    }

    if (resolve == LabelResolve_Global)
    {
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                return l;
            }
        }
    }
    else if (resolve == LabelResolve_Backwards)
    {
        CompilerLabel* best = nullptr;

        // find largest label that is smaller than the start addr
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                if (!best || (l->m_value > best->m_value))
                {
                    if (l->m_value < resolveStartAddr)
                    {
                        best = l;
                    }
                }
            }
        }
        return best;
    }
    else
    {
        CompilerLabel* best = nullptr;

        // find smallest label that is larger than the start addr
        for (auto l : si->m_labels)
        {
            if (StrEqual(l->m_name, name))
            {
                if (!best || (l->m_value < best->m_value))
                {
                    if (l->m_value >= resolveStartAddr)
                    {
                        best = l;
                    }
                }
            }
        }
        return best;
    }
    return nullptr;
}

void Compiler::CmdImport_Parse(TokenFifo &fifo, CompilerSourceInfo *si, CompilerLineInfo *li, u32& currentMemAddr)
{
    string filename = fifo.Pop();
    if (filename.empty())
        ERR("No filename specified");

    string path = si->m_workingDir + filename.substr(1, filename.size() - 2);
    FILE* fh = fopen(path.c_str(), "rb");
    if (fh)
    {
        fseek(fh, 0, SEEK_END);
        int size = ftell(fh);
        fseek(fh, 0, SEEK_SET);

        if (size == 0)
            ERR("Import file size is 0");

        u8* data = (u8*)SDL_malloc(size);
        int readSize = (int)fread(data, size, 1, fh);
        if (readSize != 1)
        {
            fclose(fh);
            SDL_free(data);
            ERR("Error reading file");
        }

        for (int i = 0; i < size; i++)
            li->data.push_back(data[i]);

        li->type = LT_DataBytes;
        li->memAddr = currentMemAddr;
        currentMemAddr += size;
        SDL_free(data);
        fclose(fh);
    }
    else
    {
        ERR("Unable to open file %s", path.c_str());
    }
}

void Compiler::CompileLinePass1(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine *sourceLine, u32& currentMemAddr)
{
    auto tokens = sourceLine->GetTokens();
    TokenFifo fifo(tokens);
    string &token = fifo.Pop();
    if (token.empty())
    {
        li->type = LT_Comment;
        return;
    }
    else if (StrEqual(token, ".import"))
    {
        string filename = fifo.Pop();
        if (filename.empty())
            ERR("No filename specified");

        string path = si->m_workingDir + filename.substr(1, filename.size() - 2);
        FILE* fh = fopen(path.c_str(), "rb");
        if (fh)
        {
            fseek(fh, 0, SEEK_END);
            int size = ftell(fh);
            fseek(fh, 0, SEEK_SET);

            if (size == 0)
                ERR("Import file size is 0");

            u8* data = (u8*)SDL_malloc(size);
            int readSize = (int)fread(data, size, 1, fh);
            if (readSize != 1)
            {
                fclose(fh);
                SDL_free(data);
                ERR("Error reading file");
            }

            for (int i = 0; i < size; i++)
                li->data.push_back(data[i]);

            li->type = LT_DataBytes;
            li->dataEvaluated = true;
            li->memAddr = currentMemAddr;
            currentMemAddr += size;
            SDL_free(data);
            fclose(fh);
        }
        else
        {
            ERR("Unable to open file %s", path.c_str());
        }
        return;
    }
    else if (StrEqual(token, ".basicStartup"))
    {
        li->type = LT_BasicStartup;
        li->memAddr = 0x801;
        li->dataEvaluated = false;

        if (fifo.Pop() == "(")
        {
            li->dataExpr.push_back(new CompilerExpression());
            PopExpressionValue(fifo, li, li->dataExpr[0], 0);
            if (li->error)
                return;

            if (fifo.Pop() != ")")
                ERR("Expected close bracket");
        }
        else
        {
            li->dataExpr.push_back(new CompilerExpression(0x801 + 13));
        }

        currentMemAddr = 0x801 + 13;
    }
    else if (StrEqual(token, ".generate.b"))
    {
        li->type = LT_GenerateBytes;
        li->memAddr = currentMemAddr;
        li->dataEvaluated = false;

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[0], 0);
        if (li->error)
            return;

        double startVal;
        if (!EvaluateExpression(si, li, li->dataExpr[0], startVal))
            ERR("Generate.b START param is not evaluable on first pass");
        li->cmdParams.push_back(startVal);
            
        if (fifo.Pop() != ",")
            ERR("Expected comma before START param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[1], 0);
        if (li->error)
            return;

        double endVal;
        if (!EvaluateExpression(si, li, li->dataExpr[1], endVal))
            ERR("Generate.b END param is not be evaluable on first pass");
        li->cmdParams.push_back(endVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before END param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[2], 0);
        if (li->error)
            return;

        int length = (int)endVal - (int)startVal + 1;
        if (length <= 0)
            ERR("Generate.b has zero length");

        currentMemAddr += length;
    }
    else if (StrEqual(token, ".generate.w"))
    {
        li->type = LT_GenerateWords;
        li->memAddr = currentMemAddr;
        li->dataEvaluated = false;

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[0], 0);
        if (li->error)
            return;

        double startVal;
        if (!EvaluateExpression(si, li, li->dataExpr[0], startVal))
            ERR("Generate.b START param is not evaluable on first pass");
        li->cmdParams.push_back(startVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before START param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[1], 0);
        if (li->error)
            return;

        double endVal;
        if (!EvaluateExpression(si, li, li->dataExpr[1], endVal))
            ERR("Generate.b END param is not be evaluable on first pass");
        li->cmdParams.push_back(endVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before END param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[2], 0);
        if (li->error)
            return;

        int length = (int)endVal - (int)startVal + 1;
        if (length <= 0)
            ERR("Generate.b has zero length");

        currentMemAddr += length*2;
    }
    else if (StrEqual(token, "*"))
    {
        li->dataExpr.push_back(new CompilerExpression());

        string& tok1 = fifo.Pop();
        if (!StrEqual(tok1, "="))
            ERR("Expected '=' after '*'");

        PopExpressionValue(fifo, li, li->dataExpr[0], 0);
        if (li->error)
            return;

        double addr;
        if (!EvaluateExpression(si, li, li->dataExpr[0], addr))
            return;

        currentMemAddr = (int)addr;
        li->type = LT_Address;
        li->memAddr = (u32)addr;
        return;
    }
    else if (IsOpCode(token.c_str()))
    {
        // decode the opcode
        li->type = LT_Instruction;
        li->memAddr = currentMemAddr;
        li->addressMode = AM_Implied;
        li->opcode = 0;
        li->operandValue = 0;
        li->dataExpr.push_back(new CompilerExpression());
        li->dataEvaluated = false;

        if (fifo.Peek().empty())
        {
            // implied
            li->addressMode = AM_Implied;
        }
        else if (StrEqual(fifo.Peek(), "#"))
        {
            // immediate
            fifo.Pop();
            li->addressMode = AM_Immediate;

            PopExpressionValue(fifo, li, li->dataExpr[0], 0);
        }
        else if (StrEqual(fifo.Peek(), "("))
        {
            fifo.Pop();

            // indirect, indirectY, indirectX
            PopExpressionValue(fifo, li, li->dataExpr[0], 0);

            if (fifo.Peek() == ",")
            {
                // indirectX
                fifo.Pop();
                if (fifo.Pop() != "x")
                    ERR("Expected 'x' for indirect X addressing mode");
                li->addressMode = AM_IndirectX;
            }
            else
            {
                // indirect, indirectY
                if (fifo.Pop() != ")")
                    ERR("Expected close bracket");

                if (fifo.Peek() == ",")
                {
                    // indirectY
                    fifo.Pop();
                    if (fifo.Pop() != "y")
                        ERR("Expected 'y' for indirect Y addressing mode");
                    li->addressMode = AM_IndirectY;
                }
                else
                {
                    li->addressMode = AM_Indirect;
                }
            }
        }
        else
        {
            // zeroPage, zeroPageX, absoluteX, absoluteY, relative
            PopExpressionValue(fifo, li, li->dataExpr[0], 0);

            if (FindOpcode(token, AM_Relative))
            {
                li->addressMode = AM_Relative;
            }
            else
            {
                // if we can't evaluate it now, then we consider it absolute addressing
                double value;
                bool canEvaluate = EvaluateExpression(si, li, li->dataExpr[0], value);
                bool isZeroPage = (canEvaluate && value >= -128 && value <= 255);

                if (fifo.Peek() == ",")
                {
                    fifo.Pop();
                    if (fifo.Peek() == "x")
                    {
                        fifo.Pop();
                        li->addressMode = isZeroPage ? AM_ZeroPageX : AM_AbsoluteX;
                    }
                    else if (fifo.Peek() == "y")
                    {
                        fifo.Pop();
                        li->addressMode = isZeroPage ? AM_ZeroPageY : AM_AbsoluteY;
                    }
                    else
                        ERR("Expected x or y for absolute indexed addressing mode")
                }
                else
                {
                    li->addressMode = isZeroPage ? AM_ZeroPage : AM_Absolute;
                }
            }
        }

        auto opcode = FindOpcode(token, li->addressMode);
        if (opcode == nullptr)
            ERR("Opcode '%s' with address mode '%s'", token.c_str(), gAddressingModeName[li->addressMode]);

        li->opcode = opcode->opc;

        currentMemAddr += gAddressingModeSize[li->addressMode];
    }
    else if (token == "@")
    {
        // local address label
        li->label = fifo.Pop();

        if (fifo.Pop() != ":")
            ERR("Expected ':' at end of labels.");

        // address label
        li->type = LT_Label;
        li->memAddr = currentMemAddr;
        li->operand = li->memAddr;
        si->m_labels.push_back(new CompilerLabel(li->label, currentMemAddr, li->lineNmbr, true));
    }
    else if (StrEqual(token, "dc.b") || StrEqual(token, ".byte") || StrEqual(token, "byte"))
    {
        li->type = LT_DataBytes;
        li->memAddr = currentMemAddr;
        li->dataEvaluated = false;

        while (!fifo.IsEmpty())
        {
            auto expr = new CompilerExpression();
            PopExpressionValue(fifo, li, expr, 0);
            li->dataExpr.push_back(expr);

            if (fifo.Peek() == ",")
            {
                fifo.Pop();
                if (fifo.IsEmpty())
                    ERR("Expected more data on line");
            }
            else if (!fifo.IsEmpty())
            {
                ERR("Unexpected token '%s' in data bytes", fifo.Peek().c_str());
            }
        }
        currentMemAddr += (int)li->dataExpr.size();
    }
    else if (StrEqual(token, "dc.w") || StrEqual(token, ".word") || StrEqual(token, "word"))
    {
        li->type = LT_DataWords;
        li->memAddr = currentMemAddr;
        li->dataEvaluated = false;

        while (!fifo.IsEmpty())
        {
            auto expr = new CompilerExpression();
            PopExpressionValue(fifo, li, expr, 0);
            li->dataExpr.push_back(expr);

            if (fifo.Peek() == ",")
            {
                fifo.Pop();
                if (fifo.IsEmpty())
                    ERR("Expected more data on line");
            }
            else if (!fifo.IsEmpty())
            {
                ERR("Unexpected token '%s' in data bytes", fifo.Peek().c_str());
            }
        }
        currentMemAddr += (int)li->dataExpr.size() * 2;
    }
    else if (StrEqual(token, ".text"))
    {
        li->type = LT_DataText;
        li->memAddr = currentMemAddr;

        string text = fifo.Pop();
        if (text.size() < 3 || text.front() != '"' || text.back() != '"')
            ERR("Malformed text");
        string subText = text.substr(1, text.size() - 2);
        for (auto c : subText)
        {
            // petsci encode
            li->data.push_back(s_asciiToScreenCode[c]);
        }
        currentMemAddr += (int)li->data.size();
    }
    else
    {
        // address label
        li->label = token;

        if (si->DoesLabelExist(li->label.c_str()))
            ERR("Duplicate variable declaration: %s", li->label.c_str());

        if (fifo.Peek() == "=")
        {
            fifo.Pop();

            // value by assignment
            li->type = LT_Variable;
            li->dataExpr.push_back(new CompilerExpression());

            PopExpressionValue(fifo, li, li->dataExpr[0], 0);

            double value;
            if (EvaluateExpression(si, li, li->dataExpr[0], value))
            {
                si->m_labels.push_back(new CompilerLabel(li->label, value, li->lineNmbr));
                li->dataEvaluated = true;
            }
        }
        else
        {
            // value by address
            if (fifo.Pop() != ":")
                ERR("Cannot evaluate token '%s' - Expected ':' if its a label.", token.c_str());

            // address label
            li->type = LT_Label;
            li->memAddr = currentMemAddr;
            si->m_labels.push_back(new CompilerLabel(li->label, currentMemAddr, li->lineNmbr));
        }
    }

    if (!fifo.IsEmpty())
        ERR("Extra text at end of line.");
}

bool Compiler::ResolveExpressionToken(CompilerSourceInfo *si, CompilerExpressionToken *token)
{
    if (token->resolve != LabelResolve_Done)
    {
        auto label = FindLabel(si, token->label, token->resolve, (u32)token->value);
        if (!label)
            return false;

        token->resolve = LabelResolve_Done;
        token->value = label->m_value;
    }
    return true;
}

bool Compiler::EvaluateExpression(CompilerSourceInfo* si, CompilerLineInfo* line, CompilerExpression* expr, double &value)
{
    int tokenIdx = 0;
    int operatorIdx = 0;

    while (!expr->m_operators.empty())
    {
        if (operatorIdx < expr->m_operators.size() - 1 && expr->m_operatorPri[operatorIdx + 1] > expr->m_operatorPri[operatorIdx])
        {
            tokenIdx += expr->m_operators[operatorIdx]->params - 1;
            operatorIdx++;
        }
        else
        {
            if (expr->m_tokens.empty())
                return false;

            if (tokenIdx < 0)
                return false;

            if (tokenIdx >= expr->m_tokens.size())
                return false;

            auto op = expr->m_operators[operatorIdx];
            if (op->params == 1)
            {
                auto token = expr->m_tokens[tokenIdx];
                if (!ResolveExpressionToken(si, token))
                    return false;

                double value = op->Evaluate(token->value, 0, 0);

                auto newToken = new CompilerExpressionToken(value);
                expr->m_tokens[tokenIdx] = newToken;
                delete token;
            }
            else if (op->params == 2)
            {
                if (expr->m_tokens.size() < tokenIdx + 2)
                    return false;

                auto inTok1 = expr->m_tokens[tokenIdx];
                auto inTok2 = expr->m_tokens[tokenIdx+1];

                if (!ResolveExpressionToken(si, inTok1))
                    return false;
                if (!ResolveExpressionToken(si, inTok2))
                    return false;

                double value = op->Evaluate(inTok1->value, inTok2->value, 0);

                auto newToken = new CompilerExpressionToken(value);
                expr->m_tokens.erase(expr->m_tokens.begin() + tokenIdx, expr->m_tokens.begin() + (tokenIdx + 2));
                expr->m_tokens.insert(expr->m_tokens.begin() + tokenIdx, newToken);
                delete inTok1;
                delete inTok2;
            }
            else if (op->params == 3)
            {
                if (expr->m_tokens.size() < tokenIdx + 3)
                    return false;

                auto inTok1 = expr->m_tokens[tokenIdx];
                auto inTok2 = expr->m_tokens[tokenIdx + 1];
                auto inTok3 = expr->m_tokens[tokenIdx + 2];

                if (!ResolveExpressionToken(si, inTok1))
                    return false;
                if (!ResolveExpressionToken(si, inTok2))
                    return false;
                if (!ResolveExpressionToken(si, inTok3))
                    return false;

                double value = op->Evaluate(inTok1->value, inTok2->value, inTok3->value);

                auto newToken = new CompilerExpressionToken(value);
                expr->m_tokens.erase(expr->m_tokens.begin() + tokenIdx, expr->m_tokens.begin() + (tokenIdx + 2));
                expr->m_tokens.insert(expr->m_tokens.begin() + tokenIdx, newToken);
                delete inTok1;
                delete inTok2;
                delete inTok3;
            }

            expr->m_operators.erase(expr->m_operators.begin() + operatorIdx);
            expr->m_operatorPri.erase(expr->m_operatorPri.begin() + operatorIdx);

            operatorIdx = 0;
            tokenIdx = 0;
        }
    }

    if (expr->m_tokens.empty())
        return false;

    auto tok = expr->m_tokens.front();
    if (!ResolveExpressionToken(si, tok))
        return false;

    value = tok->value;
    return true;
}



bool Compiler::CompileLinePass2(CompilerSourceInfo* si, CompilerLineInfo* li, SourceLine* sourceLine)
{
    if (li->error)
        return true;

    if (li->type == LT_Instruction)
    {
        if (!li->dataEvaluated)
        {
            int instructionLength = gAddressingModeSize[li->addressMode];
            if (instructionLength > 1)
            {
                if (!EvaluateExpression(si, li, li->dataExpr[0], li->operand))
                    return false;
            }

            li->dataEvaluated = true;
            li->data.push_back(li->opcode);
            if (instructionLength == 2)
            {
                if (li->addressMode == AM_Relative)
                    li->operand = li->operand - (li->memAddr + 2);

                if (li->operand > 255)
                    ERR_RF("Operand (%d) too large for addressing mode %s", li->opcode, gAddressingModeName[li->addressMode]);

                li->data.push_back((u8)(li->operand) & 0xff);
            }
            else if (instructionLength == 3)
            {
                if ((i32)li->operand < -32768 || (i32)li->operand > 65535)
                    ERR_RF("Operand (%d) too large for addressing mode %s", li->opcode, gAddressingModeName[li->addressMode]);

                li->data.push_back((u8)li->operand);
                li->data.push_back((u8)((u16)li->operand >> 8));
            }
        }
    }
    else if (li->type == LT_Variable)
    {
        if (!li->dataEvaluated)
        {
            if (!EvaluateExpression(si, li, li->dataExpr[0], li->operand))
                return false;
            li->dataEvaluated = true;
            si->m_labels.push_back(new CompilerLabel(li->label, li->operand, li->lineNmbr));
        }
    }
    else if (li->type == LT_DataBytes)
    {
        if (!li->dataEvaluated)
        {
            li->data.clear();
            for (int i = 0; i < li->dataExpr.size(); i++)
            {
                double value;
                if (!EvaluateExpression(si, li, li->dataExpr[i], value))
                    return false;

                li->data.push_back((u8)value & 0xff);
            }
            li->dataEvaluated = true;
        }
    }
    else if (li->type == LT_DataWords)
    {
        if (!li->dataEvaluated)
        {
            li->data.clear();
            for (int i = 0; i < li->dataExpr.size(); i++)
            {
                double value;
                if (!EvaluateExpression(si, li, li->dataExpr[i], value))
                    return false;

                li->data.push_back((u8)value & 0xff);
                li->data.push_back((u8)((u16)value >> 8));
            }
            li->dataEvaluated = true;
        }
    }
    else if (li->type == LT_BasicStartup)
    {
        if (!li->dataEvaluated)
        {
            double value;
            if (!EvaluateExpression(si, li, li->dataExpr[0], value))
                return false;
            li->dataEvaluated = true;

            int addr = (int)value;
            if (addr < 0x801 + 13 || addr > 65535)
                ERR_RF("BAD Basic startup address. should be 081e to ffff");

            li->data.push_back(0x0b);
            li->data.push_back(0x08);
            li->data.push_back(0x0A);
            li->data.push_back(0x00);
            li->data.push_back(0x9E);
            
            char num[16];
            SDL_snprintf(num, 16, "%d", addr);
            int length = (int)strlen(num);
            for (int i = 0; i < length; i++)
                li->data.push_back(num[i]);
            for (int i = 0; i < 10 - length; i++)
                li->data.push_back(0);
        }
    }
    else if (li->type == LT_GenerateBytes)
    {
        if (!li->dataEvaluated)
        {
            string name = "I";
            auto it = FindLabel(si, name, LabelResolve_Global, 0);
            li->data.clear();

            // copy the expression because we need to reset it after every evaluation
            for (int i = (int)li->cmdParams[0]; i <= (int)li->cmdParams[1]; i++)
            {
                it->m_value = (double)i;
                double value;
                auto expr = li->dataExpr[2]->Clone();
                bool success = EvaluateExpression(si, li, expr, value);
                delete expr;

                if (!success)
                    return false;

                li->data.push_back((u8)value & 0xff);
            }
            li->dataEvaluated = true;
        }
    }
    else if (li->type == LT_GenerateWords)
    {
        if (!li->dataEvaluated)
        {
            string name = "I";
            auto it = FindLabel(si, name, LabelResolve_Global, 0);
            li->data.clear();

            // copy the expression because we need to reset it after every evaluation
            for (int i = (int)li->cmdParams[0]; i <= (int)li->cmdParams[1]; i++)
            {
                it->m_value = (double)i;
                double value;
                auto expr = li->dataExpr[2]->Clone();
                bool success = EvaluateExpression(si, li, expr, value);
                delete expr;

                if (!success)
                    return false;

                li->data.push_back((u8)value & 0xff);
                li->data.push_back((u8)((u16)value >> 8));
            }
            li->dataEvaluated = true;
        }
    }
    return true;
}

GraphicChunk* Compiler::GetMemAddrGC(class SourceFile* file, int line, int sourceVersion)
{
    auto settings = gApp->GetSettings();
    auto ci = file->GetCompileInfo();
    if (ci && ci->m_sourceVersion == file->GetSourceVersion())
    {
        auto sl = ci->m_lines[line];
        if (sl->type != LT_Unknown && sl->type != LT_Comment)
        {
            if (sl->gcMemAddr->IsEmpty())
            {
                if (sl->error)
                {
                    SDL_Color addrCol = { 255, 0, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetFont(), "****", addrCol, 0, 0));
                }
                else
                {
                    char buffer[16];
                    SDL_snprintf(buffer, 16, "%04x", sl->memAddr);
                    SDL_Color addrCol = { 255, 255, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, addrCol, 0, 0));
                }
            }
            return sl->gcMemAddr;
        }
    }
    return nullptr;
}

GraphicChunk* Compiler::GetDecodeGC(class SourceFile* file, int line, int sourceVersion)
{
    SDL_Color dataCol = { 255, 64, 64, 255 };
    SDL_Color cycleCol = { 255, 255, 0, 255 };

    auto settings = gApp->GetSettings();
    auto ci = file->GetCompileInfo();
    if (ci && ci->m_sourceVersion == file->GetSourceVersion())
    {
        auto sl = ci->m_lines[line];
        if (sl->gcDecode->IsEmpty())
        {
            if (sl->type == LT_Instruction)
            {
                char buffer[16];
                auto opcode = &s_opcodes[sl->opcode];
                SDL_snprintf(buffer, 16, "%d", opcode->cycles);
                sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, cycleCol, 0, 0));
            }
            if (sl->data.size() > 0)
            {
                char buffer[16];
                for (int i = 0; i < min((int)sl->data.size(), 16); i++)
                {
                    SDL_snprintf(buffer, 16, "%02x", sl->data[i]);
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), buffer, dataCol, gApp->GetWhiteSpaceWidth() * (3 + i * 3), 0));
                }
                if (sl->data.size() > 16)
                {
                    dataCol = { 255, 255, 64, 255 };
                    string text = FormatString(".. %d bytes", sl->data.size());
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetFont(), text.c_str(), dataCol, gApp->GetWhiteSpaceWidth() * (3 + 16 * 3), 0));
                }
            }
        }
        return sl->gcDecode;
    }
    return nullptr;
}

void Compiler::Compile(SourceFile* file)
{
    Profile PF("Compile Time");

    auto lw = gApp->GetLogWindow();
    lw->ClearLog(LogWindow::LF_CompilerWarning);

    u32 currentMemAddr = 0;

    m_errors.clear();

    auto sourceInfo = new CompilerSourceInfo();
    sourceInfo->m_sourceVersion = file->GetSourceVersion();
    file->SetCompileInfo(sourceInfo);

    string path = file->GetPath();
    int pathEnd = (int)path.find_last_of('\\');
    if (pathEnd == string::npos)
    {
        sourceInfo->m_workingDir = "";
    }
    else
    {
        sourceInfo->m_workingDir = path.substr(0, pathEnd + 1);
    }

    int lineNmbr = 0;
    for (auto line : file->GetLines())
    {
        auto lineInfo = new CompilerLineInfo();
        lineInfo->lineNmbr = lineNmbr;
        CompileLinePass1(sourceInfo, lineInfo, line, currentMemAddr);
        sourceInfo->m_lines.push_back(lineInfo);
        lineNmbr++;
    }

    // keep passing over the source until 
    int lastCount = 0;
    int thisCount = 0;
    while (true)
    {
        thisCount = 0;
        for (auto li : sourceInfo->m_lines)
        {
            if (!CompileLinePass2(sourceInfo, li, file->GetLines()[li->lineNmbr]))
                thisCount++;
        }
        if (thisCount == 0 || thisCount == lastCount)
            break;
        lastCount = thisCount;
    }

    if (thisCount != 0)
    {
        for (auto li : sourceInfo->m_lines)
        {
            if (!CompileLinePass2(sourceInfo, li, file->GetLines()[li->lineNmbr]))
                ERR_NORET("Unable to resolve expression");
        }
    }

    lw->ClearLog(LogWindow::LF_LabelHelp);
    for (auto l : sourceInfo->m_labels)
    {
        if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0))<0.0000001 && abs(l->m_value) < 1000000000.0f)
            lw->LogText(LogWindow::LF_LabelHelp, FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr);
        else
            lw->LogText(LogWindow::LF_LabelHelp, FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr);
    }

    FlushErrors();

    lw->LogText(LogWindow::LF_CompilerWarning, PF.Log());
}

void Compiler::Error(const string& text, int lineNmbr)
{
    string out = FormatString("%d: %s", lineNmbr+1, text.c_str());
    auto item = new ErrorItem();
    item->lineNmbr = lineNmbr;
    item->text = out;
    m_errors.push_back(item);
}

void Compiler::FlushErrors()
{
    std::sort(m_errors.begin(), m_errors.end(), [](ErrorItem* a, ErrorItem* b) { return a->lineNmbr < b->lineNmbr; });

    auto lw = gApp->GetLogWindow();
    for (auto e : m_errors)
    {
        lw->LogText(LogWindow::LF_CompilerWarning, e->text, e->lineNmbr);
        delete e;
    }
    m_errors.clear();
}

bool CompilerSourceInfo::DoesLabelExist(const char* label) 
{
    for (auto l : m_labels)
    {
        if (StrEqual(l->m_name.c_str(), label))
            return true;
    }
    return false;
}

void CompilerSourceInfo::ClearVisuals()
{
    for (auto l : m_lines)
    {
        l->gcDecode->Clear();
        l->gcMemAddr->Clear();
    }
}

void CompilerSourceInfo::SavePrg(const char* path)
{
    u32 startAddr = 0xffff;
    u32 endAddr = 0;

    // pass one - find our memory range
    for (auto l : m_lines)
    {
        if (l->data.size() > 0 && !l->error)
        {
            startAddr = min(l->memAddr, startAddr);
            endAddr = max(l->memAddr, endAddr);
        }
    }

    if (startAddr <= endAddr)
    {
        FILE* fh = fopen(path, "wb");
        if (fh)
        {

            // allocate flat memory block with 2 byte header for address
            u32 memSize = (endAddr + 1) - startAddr + 2;
            u8* mem = (u8*)SDL_malloc((size_t)(memSize + 2));

            // write out lines to memory
            mem[0] = startAddr & 0xff;
            mem[1] = (startAddr >> 8) & 0xff;
            for (auto l : m_lines)
            {
                if (!l->data.empty())
                {
                    memcpy(mem + 2 + (l->memAddr - startAddr), l->data.data(), l->data.size());
                }
            }

            // save and close
            fwrite(mem, memSize, 1, fh);
            fclose(fh);
        }
    }
}


int Compiler::Go()
{
    while (!m_terminate)
    {
        m_compileRequested.Wait();

        // .. do compile

        m_compileCompleted.Signal();
    }
    return 0;
}

#if 1
TokenisedFile::TokenisedFile(SourceFile* sf)
{
    m_memory = (char*)malloc(64 * 1024 * 0124);
    m_lineCount = (int)sf->GetLines().size();
    m_lines = new TokenisedLine[m_lineCount];
    m_tokens = new TokenisedLine::Token[1024 * 1024];

    char* mptr = m_memory;
    int currentToken = 0;
    TokenisedLine* tl = m_lines;
    for (auto sl : sf->GetLines())
    {
        // count the tokens
        tl->m_tokenCount = 0;
        tl->m_firstToken = currentToken;
        for (auto& t : sl->GetTokens())
        {
            if (t[0] != ' ' && t[0] != '\t')
            {
                memcpy(mptr, &t[0], t.size() + 1);
                m_tokens[currentToken].memory = mptr;
                m_tokens[currentToken].length = (int)t.size();
                currentToken++;
                tl->m_tokenCount++;
            }
        }
        tl++;
    }
}

TokenisedFile::~TokenisedFile()
{
    free(m_memory);
    delete[] m_tokens;
    delete[] m_lines;
}

#else

TokenisedFile::TokenisedFile(SourceFile* sf)
{
    for (auto sl : sf->GetLines())
    {
        auto line = new TokenisedLine();
        for (auto& t : sl->GetTokens())
        {
            if (t[0] != ' ' && t[0] != '\t')
            {
                line->m_tokens.push_back(t);
            }
        }
        m_lines.push_back(line);
    }
}

TokenisedFile::~TokenisedFile()
{
    for (auto l : m_lines)
        delete l;
}
#endif

void Compiler::PopExpressionValue(TokenFifo& fifo, CompilerLineInfo* li, CompilerExpression* expr, int priority)
{
    CompilerExpressionOpcode *opc;

    if (fifo.Peek() == "(")
    {
        fifo.Pop();

        PopExpressionValue(fifo, li, expr, priority + 16);
        if (li->error)
            return;

        if (fifo.Peek() != ")")
            ERR("Expected close bracket!");

        fifo.Pop();
    }
    else
    {
        if (fifo.IsEmpty())
            ERR("Expected a label or expression!");

        // label or number
        string& token = fifo.Pop();
        if (token == "$")
        {
            // hex
            int value;
            if (!HexToNumber(fifo.Pop(), value))
                ERR("Unable to parse hex value");
            expr->m_tokens.push_back(new CompilerExpressionToken(value));
        }
        else if (token == "%")
        {
            // binary number
            int value;
            if (!BinaryToNumber(fifo.Pop(), value))
                ERR("Unable to parse binary value");
            expr->m_tokens.push_back(new CompilerExpressionToken(value));
        }
        else if (token == "@")
        {
            // local label
            string& label = fifo.Pop();
            LabelResolve resolve = LabelResolve_Done;
            if (label.empty())
                ERR("Expected local label after '@'");

            string& dir = fifo.Pop();
            if (dir == "+")
                resolve = LabelResolve_Forwards;
            else if (dir == "-")
                resolve = LabelResolve_Backwards;
            else
                ERR("Expected + or -  (seach direction) after local label");
            expr->m_tokens.push_back(new CompilerExpressionToken(label, resolve, li->memAddr));
        }
        else if ((token[0] >= '0' && token[0] <= '9') || token[0] == '.' || token[0] == '-' || token[0] == '+')
        {
            // value
            double value = 0;
            if (token == "-")
            {
                value = -atof(fifo.Pop().c_str());
            }
            else if (token == "+")
            {
                value = atof(fifo.Pop().c_str());
            }
            else
            {
                value = atof(token.c_str());
            }
            expr->m_tokens.push_back(new CompilerExpressionToken(value));
        }
        else if ((opc = FindPrefixExprOpcode(token)) != nullptr)
        {
            if (opc->type == CEOT_Prefix)
            {
                expr->m_operators.push_back(opc);
                expr->m_operatorPri.push_back(priority + opc->priority);
                PopExpressionValue(fifo, li, expr, priority);
            }
            else // must be a function
            {
                expr->m_operators.push_back(opc);
                expr->m_operatorPri.push_back(priority + opc->priority);
                if (fifo.Pop() != "(")
                    ERR("Expected open brackets after function");
                PopExpressionValue(fifo, li, expr, priority+16);

                if (opc->params == 2)
                {
                    if (fifo.Pop() != ",")
                        ERR("Expected comma between function params");
                    PopExpressionValue(fifo, li, expr, priority+16);
                }
                if (fifo.Pop() != ")")
                    ERR("Expected close brackets after function params");
            }
        }
        else
        {
            // global label
            expr->m_tokens.push_back(new CompilerExpressionToken(token, LabelResolve_Global, 0));
        }
    }

    // continue if we follow up with an operand
    if (!fifo.IsEmpty())
    {
        string opcodeToken = fifo.Peek();
        CompilerExpressionOpcode* opc = FindExprOpcode(opcodeToken);
        if (!opc)
            return;

        fifo.Pop();

        if (opc->params == 1)
            ERR("Unexpected prefix maths opcode '%s' after a value", opcodeToken.c_str());

        expr->m_operators.push_back(opc);
        expr->m_operatorPri.push_back(priority + opc->priority);

        if (opc->params == 2)
        {
            PopExpressionValue(fifo, li, expr, priority);
        }
        else if (opc->params == 2)
        {
            PopExpressionValue(fifo, li, expr, priority);
            if (fifo.Pop() != ":")
                ERR("Expected ternary ':'");
            PopExpressionValue(fifo, li, expr, priority);
        }
    }
}

void Compiler::AddLabelsContaining(CompilerSourceInfo* si, vector<CompilerLabel*>& labels, const string& token)
{
    for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
    {
        auto l = &s_systemLabels[i];
        if (StrFind(l->m_name, token) != string::npos)
        {
            if (std::find(labels.begin(), labels.end(), l) == labels.end())
            {
                labels.push_back(l);
            }
        }
    }
    
    if (si)
    {
        for (auto l : si->m_labels)
        {
            if (StrFind(l->m_name, token) != string::npos)
            {
                if (std::find(labels.begin(), labels.end(), l) == labels.end())
                {
                    labels.push_back(l);
                }
            }
        }
    }
}

void Compiler::AddOpcodesContaining(vector<CompilerOpcode*>& opcodes, const string& token)
{
    for (auto o : s_uniqueOpcodes)
    {
        if (StrFind(o->name, token) != string::npos)
        {
            if (std::find(opcodes.begin(), opcodes.end(), o) == opcodes.end())
            {
                opcodes.push_back(o);
            }
        }
    }
}


void Compiler::LogContextualHelp(SourceFile* sf, int line)
{
    auto lw = gApp->GetLogWindow();
    auto si = sf->GetLines()[line];
    auto& tokens = si->GetTokens();

    int tokenIdx = 0;

    vector<CompilerLabel*> labels;
    vector<CompilerOpcode*> opcodes;

    auto cs = sf->GetCompileInfo();
    while (tokenIdx < tokens.size())
    {
        auto& token = tokens[tokenIdx++];
        if (token.empty() || token[0] == ' ' || token[0] == '\t')
            continue;

        if (token == "$" || token == "%")
        {
            tokenIdx++;
            continue;
        }

        if (token[0] >= '0' && token[0] <= '9')
            continue;

        AddLabelsContaining(cs, labels, token);
        AddOpcodesContaining(opcodes, token);
    }
    
    lw->ClearLog(LogWindow::LF_InstructionHelp);
    if (!labels.empty())
    {
        lw->LogText(LogWindow::LF_InstructionHelp, "Suggested Labels:");
        for (auto l : labels)
        {
            if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0)) < 0.0000001 && abs(l->m_value) < 1000000000.0f)
                lw->LogText(LogWindow::LF_LabelHelp, FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr, 1);
            else
                lw->LogText(LogWindow::LF_LabelHelp, FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr, 1);
        }
    }
    if (!opcodes.empty())
    {
        lw->LogText(LogWindow::LF_InstructionHelp, "Suggested Opcodes:");
        for (auto o : opcodes)
        {
            lw->LogTextArray(LogWindow::LF_InstructionHelp, o->help);
        }
    }
}


