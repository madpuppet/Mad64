#include "common.h"
#include "compiler.h"
#include "graphicChunk.h"
#include "contextualHelp.h"
#include "emulatorc64.h"
#include "dockableWindow_log.h"
#include "dockableWindow_memoryImage.h"

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

    CompilerLabel("cpu.bank", gHELP_CPU_MEMBANK, 1),

    CompilerLabel("vic.sprite0Ptr", 0x3f8), CompilerLabel("vic.sprite1Ptr", 0x3f9), CompilerLabel("vic.sprite2Ptr", 0x3fa), CompilerLabel("vic.sprite3Ptr", 0x3fb),
    CompilerLabel("vic.sprite4Ptr", 0x3fc), CompilerLabel("vic.sprite5Ptr", 0x3fd), CompilerLabel("vic.sprite6Ptr", 0x3fe), CompilerLabel("vic.sprite7Ptr", 0x3ff),

    CompilerLabel("vic.sprite0X", 0xd000),  CompilerLabel("vic.sprite0Y", 0xd001),  CompilerLabel("vic.sprite1X", 0xd002),  CompilerLabel("vic.sprite1Y", 0xd003),
    CompilerLabel("vic.sprite2X", 0xd004),  CompilerLabel("vic.sprite2Y", 0xd005),  CompilerLabel("vic.sprite3X", 0xd006),  CompilerLabel("vic.sprite3Y", 0xd007),
    CompilerLabel("vic.sprite4X", 0xd008),  CompilerLabel("vic.sprite4Y", 0xd009),  CompilerLabel("vic.sprite5X", 0xd00a),  CompilerLabel("vic.sprite5Y", 0xd00b),
    CompilerLabel("vic.sprite6X", 0xd00c),  CompilerLabel("vic.sprite6Y", 0xd00d),  CompilerLabel("vic.sprite7X", 0xd00e),  CompilerLabel("vic.sprite7Y", 0xd00f),
    CompilerLabel("vic.spriteXMSB", 0xd010), CompilerLabel("vic.control1", gHELP_VIC_CONTROL1, 0xd011), CompilerLabel("vic.rasterCounter", 0xd012), CompilerLabel("vic.lightPenX", 0xd013),
    CompilerLabel("vic.lightPenY", 0xd014), CompilerLabel("vic.spriteEnable", 0xd015), CompilerLabel("vic.control2", gHELP_VIC_CONTROL2, 0xd016), CompilerLabel("vic.spriteYSize", 0xd017),
    CompilerLabel("vic.memoryPointer", gHELP_VIC_SCRMEM, 0xd018), CompilerLabel("vic.intRegister", gHELP_VIC_IRQREQ, 0xd019), CompilerLabel("vic.intEnable", gHELP_VIC_INTENA, 0xd01a), CompilerLabel("vic.spritePriority", 0xd01b),
    CompilerLabel("vic.spriteMulticolor", 0xd01c), CompilerLabel("vic.spriteXSize", 0xd01d), CompilerLabel("vic.spriteToSpriteCollision", 0xd01e), CompilerLabel("vic.spriteToDataCollision", 0xd01f),
    CompilerLabel("vic.borderColor", 0xd020), CompilerLabel("vic.backgroundColor0", 0xd021), CompilerLabel("vic.backgroundColor1", 0xd022), CompilerLabel("vic.backgroundColor2", 0xd023),
    CompilerLabel("vic.backgroundColor3", 0xd024), CompilerLabel("vic.spriteMulticolor0", 0xd025), CompilerLabel("vic.spriteMulticolor1", 0xd026), CompilerLabel("vic.sprite0Color", 0xd027),
    CompilerLabel("vic.sprite1Color", 0xd028), CompilerLabel("vic.sprite2Color", 0xd029), CompilerLabel("vic.sprite3Color", 0xd02a), CompilerLabel("vic.sprite4Color", 0xd02b),
    CompilerLabel("vic.sprite5Color", 0xd02c), CompilerLabel("vic.sprite6Color", 0xd02d), CompilerLabel("vic.sprite7Color", 0xd02e), CompilerLabel("vic.colorMemory", 0xd800),

    CompilerLabel("vic.YSCR", 0x07), CompilerLabel("vic.RSEL", 0x08), CompilerLabel("vic.DEN", 0x10), CompilerLabel("vic.BMM", 0x20), CompilerLabel("vic.ECM", 0x40), CompilerLabel("vic.RST8", 0x80),
    CompilerLabel("vic.XSCR", 0x07), CompilerLabel("vic.CSEL", 0x08), CompilerLabel("vic.MCM", 0x10), CompilerLabel("vic.RES", 0x20),

    CompilerLabel("cia1.dataPortA", gHELP_CIA1_DATAPORTA, 0xdc00), CompilerLabel("cia1.dataPortB", gHELP_CIA1_DATAPORTB, 0xdc01), CompilerLabel("cia1.dataDirectionA", 0xdc02), CompilerLabel("cia1.dataDirectionB", 0xdc03),
    CompilerLabel("cia1.timerALow", 0xdc04), CompilerLabel("cia1.timerAHigh", 0xdc05), CompilerLabel("cia1.timerBLow", 0xdc06), CompilerLabel("cia1.timerBHigh", 0xdc07),
    CompilerLabel("cia1.clockTenths", 0xdc08), CompilerLabel("cia1.clockSeconds", 0xdc09), CompilerLabel("cia1.clockMinutes", 0xdc0a), CompilerLabel("cia1.clockHours", 0xdc0b),
    CompilerLabel("cia1.serialShift", 0xdc0c), CompilerLabel("cia1.interruptControl", gHELP_CIA1_INTCONTROL, 0xdc0d), CompilerLabel("cia1.controlTimerA", gHELP_CIA1_CONTROLTIMERA, 0xdc0e), CompilerLabel("cia1.controlTimerB", gHELP_CIA1_CONTROLTIMERB, 0xdc0f),

    CompilerLabel("cia2.dataPortA", gHELP_CIA1_DATAPORTA, 0xdd00), CompilerLabel("cia2.dataPortB", gHELP_CIA1_DATAPORTB, 0xdd01), CompilerLabel("cia2.dataDirectionA", 0xdd02), CompilerLabel("cia2.dataDirectionB", 0xdd03),
    CompilerLabel("cia2.timerALow", 0xdd04), CompilerLabel("cia2.timerAHigh", 0xdd05), CompilerLabel("cia2.timerBLow", 0xdd06), CompilerLabel("cia2.timerBHigh", 0xdd07),
    CompilerLabel("cia2.clockTenths", 0xdd08), CompilerLabel("cia2.clockSeconds", 0xdd09), CompilerLabel("cia2.clockMinutes", 0xdd0a), CompilerLabel("cia2.clockHours", 0xdd0b),
    CompilerLabel("cia2.serialShift", 0xdd0c), CompilerLabel("cia2.interruptControl", gHELP_CIA1_INTCONTROL, 0xdd0d), CompilerLabel("cia2.controlTimerA", 0xdd0e), CompilerLabel("cia2.controlTimerB", 0xdd0f),

    CompilerLabel("rom.ACPTR", gHELP_ROM_ACPTR, 0xFFA5),    CompilerLabel("rom.CHKIN", gHELP_ROM_CHKIN, 0xFFC6),
    CompilerLabel("rom.CHKOUT", gHELP_ROM_CHKOUT, 0xFFC9),    CompilerLabel("rom.CHRIN", gHELP_ROM_CHRIN, 0xFFCF),
    CompilerLabel("rom.CHROUT", gHELP_ROM_CHROUT, 0xFFD2),    CompilerLabel("rom.CIOUT", gHELP_ROM_CIOUT, 0xFFA8),
    CompilerLabel("rom.CINT", gHELP_ROM_CINT, 0xFF81),    CompilerLabel("rom.CLALL", gHELP_ROM_CLALL, 0xFFE7),
    CompilerLabel("rom.CLOSE", gHELP_ROM_CLOSE, 0xFFC3),    CompilerLabel("rom.CLRCHN", gHELP_ROM_CLRCHN, 0xFFCC),
    CompilerLabel("rom.GETIN", gHELP_ROM_GETIN, 0xFFE4),    CompilerLabel("rom.IOBASE", gHELP_ROM_IOBASE, 0xFFF3),
    CompilerLabel("rom.IOINIT", gHELP_ROM_IOINIT, 0xFF84),    CompilerLabel("rom.LISTEN", gHELP_ROM_LISTEN, 0xFFB1),
    CompilerLabel("rom.LOAD", gHELP_ROM_LOAD, 0xFFD5),    CompilerLabel("rom.MEMBOT", gHELP_ROM_MEMBOT, 0xFF9C),
    CompilerLabel("rom.MEMTOP", gHELP_ROM_MEMTOP, 0xFF99),    CompilerLabel("rom.OPEN", gHELP_ROM_OPEN, 0xFFC0),
    CompilerLabel("rom.PLOT", gHELP_ROM_PLOT, 0xFFF0),    CompilerLabel("rom.RAMTAS", gHELP_ROM_RAMTAS, 0xFF87),
    CompilerLabel("rom.RDTIM", gHELP_ROM_RDTIM, 0xFFDE),    CompilerLabel("rom.READST", gHELP_ROM_READST, 0xFFB7),
    CompilerLabel("rom.RESTOR", gHELP_ROM_RESTOR, 0xFF8A),    CompilerLabel("rom.SAVE", gHELP_ROM_SAVE, 0xFFD8),
    CompilerLabel("rom.SCNKEY", gHELP_ROM_SCNKEY, 0xFF9F),    CompilerLabel("rom.SCREEN", gHELP_ROM_SCREEN, 0xFFED),
    CompilerLabel("rom.SECOND", gHELP_ROM_SECOND, 0xFF93),    CompilerLabel("rom.SETLFS", gHELP_ROM_SETLFS, 0xFFBA),
    CompilerLabel("rom.SETMSG", gHELP_ROM_SETMSG, 0xFF90),    CompilerLabel("rom.SETNAM", gHELP_ROM_SETNAM, 0xFFBD),
    CompilerLabel("rom.SETTIM", gHELP_ROM_SETTIM, 0xFFDB),    CompilerLabel("rom.SETTMO", gHELP_ROM_SETTMO, 0xFFA2),
    CompilerLabel("rom.STOP", gHELP_ROM_STOP, 0xFFE1),    CompilerLabel("rom.TALK", gHELP_ROM_TALK, 0xFFB4),
    CompilerLabel("rom.TKSA", gHELP_ROM_TKSA, 0xFF96),    CompilerLabel("rom.UDTIM", gHELP_ROM_UDTIM, 0xFFEA),
    CompilerLabel("rom.UNLSN", gHELP_ROM_UNLSN, 0xFFAE),    CompilerLabel("rom.UNTLK", gHELP_ROM_UNTLK, 0xFFAB),
    CompilerLabel("rom.VECTOR", gHELP_ROM_VECTOR, 0xFF8D),

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

bool HexToNumber(const string& token, int& value)
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

bool BinaryToNumber(const string& token, int& value)
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

bool Base4ToNumber(const string& token, int& value)
{
    value = 0;
    for (int i = 0; i < (int)token.size(); i++)
    {
        char ch = toupper(token[i]);
        if (ch != '0' && ch != '1' && ch != '2' && ch != '3')
            return false;

        value = value * 4;
        value += (ch - '0');
    }
    return true;
}

Compiler::Compiler() : m_compileRequested(0,1), Thread("Compiler")
{
    m_compilationActive = false;
    m_nextFile = 0;
    m_activeFile = 0;
    m_compiledFile = 0;

    Start();
}

Compiler::~Compiler()
{
    m_terminate = true;
    m_compileRequested.Signal();
    WaitForThreadCompletion();

    delete m_nextFile;
    delete m_activeFile;
}

CompilerLabel* Compiler::FindLabel(CompilerSourceInfo* csi, const string& name, LabelResolve resolve, u32 resolveStartAddr)
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
        for (auto l : csi->m_labels)
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
        for (auto l : csi->m_labels)
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
        for (auto l : csi->m_labels)
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

void Compiler::CompileLinePass1(CompilerLineInfo* li, TokenisedLine *sourceLine, u32& currentMemAddr)
{
    auto tokens = sourceLine->GetTokens();
    Cpu6502::ForceAddressing fa;
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

        string path = m_activeFile->m_workingDir + filename.substr(1, filename.size() - 2);
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

            li->type = LT_ImportedData;
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
        if (!EvaluateExpression(li, li->dataExpr[0], startVal))
            ERR("Generate.b START param is not evaluable on first pass");
        li->cmdParams.push_back(startVal);
            
        if (fifo.Pop() != ",")
            ERR("Expected comma before START param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[1], 0);
        if (li->error)
            return;

        double endVal;
        if (!EvaluateExpression(li, li->dataExpr[1], endVal))
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
        if (!EvaluateExpression(li, li->dataExpr[0], startVal))
            ERR("Generate.w START param is not evaluable on first pass");
        li->cmdParams.push_back(startVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before START param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[1], 0);
        if (li->error)
            return;

        double endVal;
        if (!EvaluateExpression(li, li->dataExpr[1], endVal))
            ERR("Generate.w END param is not be evaluable on first pass");
        li->cmdParams.push_back(endVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before END param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[2], 0);
        if (li->error)
            return;

        int length = (int)endVal - (int)startVal + 1;
        if (length <= 0)
            ERR("Generate.w has zero length");

        currentMemAddr += length*2;
    }
    else if (StrEqual(token, ".generate.s"))
    {
        li->type = LT_GenerateSprites;
        li->memAddr = currentMemAddr;
        li->dataEvaluated = false;

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[0], 0);
        if (li->error)
            return;

        double startVal;
        if (!EvaluateExpression(li, li->dataExpr[0], startVal))
            ERR("Generate.b START param is not evaluable on first pass");
        li->cmdParams.push_back(startVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before START param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[1], 0);
        if (li->error)
            return;

        double endVal;
        if (!EvaluateExpression(li, li->dataExpr[1], endVal))
            ERR("Generate.s END param is not be evaluable on first pass");
        li->cmdParams.push_back(endVal);

        if (fifo.Pop() != ",")
            ERR("Expected comma before END param");

        li->dataExpr.push_back(new CompilerExpression());
        PopExpressionValue(fifo, li, li->dataExpr[2], 0);
        if (li->error)
            return;

        int length = (int)endVal - (int)startVal + 1;
        if (length <= 0)
            ERR("Generate.s has zero length");

        currentMemAddr += length * 3;
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
        if (!EvaluateExpression(li, li->dataExpr[0], addr))
            return;

        currentMemAddr = (int)addr;
        li->type = LT_Address;
        li->memAddr = (u32)addr;
        return;
    }
    else if (m_cpu->IsOpcode(token.c_str(), &fa))
    {
        // decode the opcode
        li->type = LT_Instruction;
        li->memAddr = currentMemAddr;
        li->addressMode = Cpu6502::AM_Imp;
        li->opcode = 0;
        li->operandValue = 0;
        li->dataExpr.push_back(new CompilerExpression());
        li->dataEvaluated = false;

        if (fifo.Peek().empty())
        {
            // implied
            li->addressMode = Cpu6502::AM_Imp;
        }
        else if (StrEqual(fifo.Peek(), "#"))
        {
            // immediate
            fifo.Pop();
            li->addressMode = Cpu6502::AM_Imm;

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
                li->addressMode = Cpu6502::AM_IndX;

                // indirect, indirectY
                if (fifo.Pop() != ")")
                    ERR("Expected close bracket");
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
                    li->addressMode = Cpu6502::AM_IndY;
                }
                else
                {
                    li->addressMode = Cpu6502::AM_Ind;
                }
            }
        }
        else
        {
            // zeroPage, zeroPageX, absoluteX, absoluteY, relative
            PopExpressionValue(fifo, li, li->dataExpr[0], 0);

            if (m_cpu->FindOpcode(token, Cpu6502::AM_Rel))
            {
                li->addressMode = Cpu6502::AM_Rel;
            }
            else
            {
                // if we can't evaluate it now, then we consider it absolute addressing
                double value;
                bool canEvaluate = EvaluateExpression(li, li->dataExpr[0], value);
                bool isZeroPage = (canEvaluate && value >= -128 && value <= 255);

                if (fifo.Peek() == ",")
                {
                    fifo.Pop();
                    if (fifo.Peek() == "x")
                    {
                        fifo.Pop();
                        if (fa != Cpu6502::FA_Auto)
                            li->addressMode = (fa == Cpu6502::FA_Zero) ? Cpu6502::AM_ZeroX : Cpu6502::AM_AbsX;
                        else
                            li->addressMode = isZeroPage ? Cpu6502::AM_ZeroX : Cpu6502::AM_AbsX;
                    }
                    else if (fifo.Peek() == "y")
                    {
                        fifo.Pop();
                        if (fa != Cpu6502::FA_Auto)
                            li->addressMode = (fa == Cpu6502::FA_Zero) ? Cpu6502::AM_ZeroY : Cpu6502::AM_AbsY;
                        else
                            li->addressMode = isZeroPage ? Cpu6502::AM_ZeroY : Cpu6502::AM_AbsY;
                    }
                    else
                        ERR("Expected x or y for absolute indexed addressing mode")
                }
                else
                {
                    if (fa != Cpu6502::FA_Auto)
                        li->addressMode = (fa == Cpu6502::FA_Zero) ? Cpu6502::AM_Zero : Cpu6502::AM_Abs;
                    else
                        li->addressMode = isZeroPage ? Cpu6502::AM_Zero : Cpu6502::AM_Abs;
                }
            }
        }

        auto opcode = m_cpu->FindOpcode(token, li->addressMode);
        if (opcode == nullptr)
        {
            if (li->addressMode == Cpu6502::AM_Zero)
            {
                opcode = m_cpu->FindOpcode(token, Cpu6502::AM_Abs);
                if (opcode)
                {
                    li->addressMode = Cpu6502::AM_Abs;
                }
            }
            if (opcode == nullptr)
                ERR("Opcode '%s' with address mode '%s'", token.c_str(), m_cpu->GetAddressingModeName(li->addressMode));
        }

        if (fa == Cpu6502::FA_Error)
            ERR("Unknown explicit addressing mode on '%s' - should be .b (zero page) or .w (absolute)", token.c_str());

        li->opcode = opcode->opc;

        currentMemAddr += m_cpu->GetAddressingModeSize(li->addressMode);
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
        m_compiledFile->m_labels.push_back(new CompilerLabel(li->label, currentMemAddr, li->lineNmbr, true));
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
    else if (StrEqual(token, "dc.s") || StrEqual(token, ".sprite"))
    {
        li->type = LT_DataSprites;
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
        currentMemAddr += (int)li->dataExpr.size()*3;
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
    else if (StrEqual(token, ".text") || StrEqual(token, "dc.t"))
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

        if (m_compiledFile->DoesLabelExist(li->label.c_str()))
            ERR("Duplicate variable declaration: %s", li->label.c_str());

        if (fifo.Peek() == "=")
        {
            fifo.Pop();

            // value by assignment
            li->type = LT_Variable;
            li->dataExpr.push_back(new CompilerExpression());

            PopExpressionValue(fifo, li, li->dataExpr[0], 0);

            double value;
            if (EvaluateExpression(li, li->dataExpr[0], value))
            {
                m_compiledFile->m_labels.push_back(new CompilerLabel(li->label, value, li->lineNmbr));
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
            m_compiledFile->m_labels.push_back(new CompilerLabel(li->label, currentMemAddr, li->lineNmbr));
        }
    }

    if (!fifo.IsEmpty())
        ERR("Extra text at end of line.");
}

bool Compiler::ResolveExpressionToken(CompilerExpressionToken *token)
{
    if (token->resolve != LabelResolve_Done)
    {
        auto label = FindLabel(m_compiledFile, token->label, token->resolve, (u32)token->value);
        if (!label)
            return false;

        token->resolve = LabelResolve_Done;
        token->value = label->m_value;
    }
    return true;
}

bool Compiler::EvaluateExpression(CompilerLineInfo* line, CompilerExpression* expr, double &value)
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
                if (!ResolveExpressionToken(token))
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

                if (!ResolveExpressionToken(inTok1))
                    return false;
                if (!ResolveExpressionToken(inTok2))
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

                if (!ResolveExpressionToken(inTok1))
                    return false;
                if (!ResolveExpressionToken(inTok2))
                    return false;
                if (!ResolveExpressionToken(inTok3))
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
    if (!ResolveExpressionToken(tok))
        return false;

    value = tok->value;
    return true;
}



bool Compiler::CompileLinePass2(CompilerLineInfo* li, TokenisedLine* sourceLine)
{
    if (li->error)
        return true;

    if (li->type == LT_Instruction)
    {
        if (!li->dataEvaluated)
        {
            int instructionLength = m_cpu->GetAddressingModeSize(li->addressMode);
            if (instructionLength > 1)
            {
                if (!EvaluateExpression(li, li->dataExpr[0], li->operand))
                    return false;
            }

            li->dataEvaluated = true;
            li->data.push_back(li->opcode);
            if (instructionLength == 2)
            {
                if (li->addressMode == Cpu6502::AM_Rel)
                    li->operand = li->operand - (li->memAddr + 2);

                if (li->operand > 255)
                    ERR_RF("Operand (%d) too large for addressing mode %s", li->opcode, m_cpu->GetAddressingModeName(li->addressMode));

                li->data.push_back((u8)(li->operand) & 0xff);
            }
            else if (instructionLength == 3)
            {
                if ((i32)li->operand < -32768 || (i32)li->operand > 65535)
                    ERR_RF("Operand (%d) too large for addressing mode %s", li->opcode, m_cpu->GetAddressingModeName(li->addressMode));

                li->data.push_back((u8)li->operand);
                li->data.push_back((u8)((u16)li->operand >> 8));
            }
        }
    }
    else if (li->type == LT_Variable)
    {
        if (!li->dataEvaluated)
        {
            if (!EvaluateExpression(li, li->dataExpr[0], li->operand))
                return false;
            li->dataEvaluated = true;
            m_compiledFile->m_labels.push_back(new CompilerLabel(li->label, li->operand, li->lineNmbr));
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
                if (!EvaluateExpression(li, li->dataExpr[i], value))
                    return false;

                li->data.push_back((u8)value & 0xff);
            }
            li->dataEvaluated = true;
        }
    }
    else if (li->type == LT_DataSprites)
    {
        if (!li->dataEvaluated)
        {
            li->data.clear();
            for (int i = 0; i < li->dataExpr.size(); i++)
            {
                double value;
                if (!EvaluateExpression(li, li->dataExpr[i], value))
                    return false;

                li->data.push_back((u8)((u32)value >> 16));
                li->data.push_back((u8)((u32)value >> 8));
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
                if (!EvaluateExpression(li, li->dataExpr[i], value))
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
            if (!EvaluateExpression(li, li->dataExpr[0], value))
                return false;
            li->dataEvaluated = true;

            int addr = (int)value;
            li->data.push_back(0x0b);
            li->data.push_back(0x08);
            li->data.push_back(0x0A);
            li->data.push_back(0x00);
            li->data.push_back(0x9E);
            
            char num[14];
            SDL_snprintf(num, 14, "%d", addr);
            int length = (int)strlen(num);
            for (int i = 0; i < length; i++)
                li->data.push_back(num[i]);
            for (int i = 0; i < 8 - length; i++)
                li->data.push_back(0);
        }
    }
    else if (li->type == LT_GenerateBytes)
    {
        if (!li->dataEvaluated)
        {
            string name = "I";
            auto it = FindLabel(m_compiledFile, name, LabelResolve_Global, 0);
            li->data.clear();

            // copy the expression because we need to reset it after every evaluation
            for (int i = (int)li->cmdParams[0]; i <= (int)li->cmdParams[1]; i++)
            {
                it->m_value = (double)i;
                double value;
                auto expr = li->dataExpr[2]->Clone();
                bool success = EvaluateExpression(li, expr, value);
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
            auto it = FindLabel(m_compiledFile, name, LabelResolve_Global, 0);
            li->data.clear();

            // copy the expression because we need to reset it after every evaluation
            for (int i = (int)li->cmdParams[0]; i <= (int)li->cmdParams[1]; i++)
            {
                it->m_value = (double)i;
                double value;
                auto expr = li->dataExpr[2]->Clone();
                bool success = EvaluateExpression(li, expr, value);
                delete expr;

                if (!success)
                    return false;

                li->data.push_back((u8)value & 0xff);
                li->data.push_back((u8)((u16)value >> 8));
            }
            li->dataEvaluated = true;
        }
    }
    else if (li->type == LT_GenerateSprites)
    {
        if (!li->dataEvaluated)
        {
            string name = "I";
            auto it = FindLabel(m_compiledFile, name, LabelResolve_Global, 0);
            li->data.clear();

            // copy the expression because we need to reset it after every evaluation
            for (int i = (int)li->cmdParams[0]; i <= (int)li->cmdParams[1]; i++)
            {
                it->m_value = (double)i;
                double value;
                auto expr = li->dataExpr[2]->Clone();
                bool success = EvaluateExpression(li, expr, value);
                delete expr;

                if (!success)
                    return false;

                li->data.push_back((u8)((u32)value >> 16));
                li->data.push_back((u8)((u16)value >> 8));
                li->data.push_back((u8)value & 0xff);
            }
            li->dataEvaluated = true;
        }
    }
    return true;
}

GraphicChunk* CompilerSourceInfo::GetMemAddrGC(int line)
{
    auto settings = gApp->GetSettings();
    if (line < m_lines.size())
    {
        auto sl = m_lines[line];
        if (sl->type != LT_Unknown && sl->type != LT_Comment)
        {
            if (sl->gcMemAddr->IsEmpty())
            {
                if (sl->error)
                {
                    SDL_Color addrCol = { 255, 0, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), "****", addrCol, 0, 0));
                }
                else
                {
                    char buffer[16];
                    SDL_snprintf(buffer, 16, "%04x", sl->memAddr);
                    SDL_Color addrCol = { 255, 255, 0, 255 };
                    sl->gcMemAddr->Add(GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), buffer, addrCol, 0, 0));
                }
            }
            return sl->gcMemAddr;
        }
    }
    return nullptr;
}

GraphicChunk* CompilerSourceInfo::GetDecodeGC(int line)
{
    SDL_Color dataCol = { 255, 64, 64, 255 };
    SDL_Color cycleCol = { 255, 255, 0, 255 };

    auto settings = gApp->GetSettings();
    if (line < m_lines.size())
    {
        auto sl = m_lines[line];
        if (sl->gcDecode->IsEmpty())
        {
            if (sl->type == LT_Instruction)
            {
                char buffer[16];
                auto opcode = gApp->GetEmulator()->GetCpu()->GetOpcode(sl->opcode);
                SDL_snprintf(buffer, 16, "%d", opcode->cycles);
                sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), buffer, cycleCol, 0, 0));
            }
            if (sl->data.size() > 0)
            {
                char buffer[16];
                for (int i = 0; i < min((int)sl->data.size(), 16); i++)
                {
                    SDL_snprintf(buffer, 16, "%02x", sl->data[i]);
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), buffer, dataCol, gApp->GetWhiteSpaceWidth() * (3 + i * 3), 0));
                }
                if (sl->data.size() > 16)
                {
                    dataCol = { 255, 255, 64, 255 };
                    string text = FormatString(".. %d bytes", sl->data.size());
                    sl->gcDecode->Add(GraphicElement::CreateFromText(gApp->GetRenderer(), gApp->GetFont(), text.c_str(), dataCol, gApp->GetWhiteSpaceWidth() * (3 + 16 * 3), 0));
                }
            }
        }
        return sl->gcDecode->IsEmpty() ? nullptr : sl->gcDecode;
    }
    return nullptr;
}

void Compiler::StartThreadedCompile()
{
    // start recompile now
    auto lw = gApp->GetWindowCompiler();
    lw->Clear();
    Profile PF("Start Compile");
    PF.Log();
}

void Compiler::Compile(SourceFile* file)
{
    Profile PF("Compile Prep");
    m_cpu = gApp->GetEmulator()->GetCpu();
    if (m_nextFile)
        delete m_nextFile;
    m_nextFile = new TokenisedFile(file);
    m_nextFile->m_sf = file;
    Log(PF.Log().c_str());
}

void Compiler::Update()
{
    // TODO: handle aborted compilation - ie. if source file is destroyed while we compile

    if (!m_compilationActive)
    {
        // collect any results from last compile
        if (m_compiledFile)
        {
            // collect file
            m_activeFile->m_sf->SetCompileInfo(m_compiledFile);
            delete m_activeFile;

            // compilation errors & compile time
            auto winLabels = gApp->GetWindowLabels();
            winLabels->Clear();
            for (auto l : m_compiledFile->m_labels)
            {
                if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0)) < 0.0000001 && abs(l->m_value) < 1000000000.0f)
                    winLabels->LogText(FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr, 0, (int)l->m_value);
                else
                    winLabels->LogText(FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr, 0, 0);
            }

            gApp->GetWindowMemoryImage()->SetMemMap(m_compiledFile->m_ramColorMap);

            auto winCom = gApp->GetWindowCompiler();
            winCom->Clear();
            winCom->LogText(FormatString("Compiled Time: %1.2fms", m_compiledFile->m_compileTimeMS),0,-1);

            m_compiledFile->FlushErrors();

            // reset emulator
            auto startLabel = gApp->GetCompiler()->FindMatchingLabel(m_compiledFile, "start");
            if (startLabel)
            {
                gApp->ApplyBreakpoints();
                gApp->GetEmulator()->Reset(m_compiledFile->m_ramDataMap, m_compiledFile->m_ramMask, (u16)startLabel->m_value);
            }

            m_activeFile = 0;
            m_compiledFile = 0;
        }

        // kick off next compile
        if (m_nextFile)
        {
            m_activeFile = m_nextFile;
            m_nextFile = 0;
            m_compilationActive = true;
            m_compileRequested.Signal();
        }
    }
}

void Compiler::DoCompile()
{
    Profile PF("Compile Time");

    u32 currentMemAddr = 0;

    m_compiledFile = new CompilerSourceInfo();
    m_compiledFile->m_sourceVersion = m_activeFile->m_sourceVersion;

    int lineNmbr = 0;
    for (auto line : m_activeFile->m_lines)
    {
        auto lineInfo = new CompilerLineInfo();
        lineInfo->lineNmbr = lineNmbr;
        CompileLinePass1(lineInfo, line, currentMemAddr);
        m_compiledFile->m_lines.push_back(lineInfo);
        lineNmbr++;
    }

    // keep passing over the source until 
    int lastCount = 0;
    int thisCount = 0;
    while (true)
    {
        thisCount = 0;
        for (auto li : m_compiledFile->m_lines)
        {
            if (!CompileLinePass2(li, m_activeFile->m_lines[li->lineNmbr]))
                thisCount++;
        }
        if (thisCount == 0 || thisCount == lastCount)
            break;
        lastCount = thisCount;
    }

    if (thisCount != 0)
    {
        for (auto li : m_compiledFile->m_lines)
        {
            if (!CompileLinePass2(li, m_activeFile->m_lines[li->lineNmbr]))
                ERR_NORET("Unable to resolve expression");
        }
    }
    m_compiledFile->BuildMemoryMap();

    m_compiledFile->m_compileTimeMS = PF.Time();
}

void CompilerSourceInfo::Error(const string& text, int lineNmbr)
{
    string out = FormatString("%d: %s", lineNmbr+1, text.c_str());
    auto item = new ErrorItem();
    item->lineNmbr = lineNmbr;
    item->text = out;
    m_errors.push_back(item);
}

void CompilerSourceInfo::FlushErrors()
{
    std::sort(m_errors.begin(), m_errors.end(), [](ErrorItem* a, ErrorItem* b) { return a->lineNmbr < b->lineNmbr; });

    auto lw = gApp->GetWindowCompiler();
    for (auto e : m_errors)
    {
        lw->LogText(e->text, e->lineNmbr, 0);
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

int CompilerSourceInfo::FindLineByAddress(u32 address)
{
    for (auto l : m_lines)
    {
        if ((address >= l->memAddr) && (address < l->memAddr + l->data.size()))
        {
            return l->lineNmbr;
        }
    }
    return -1;
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
            endAddr = max(l->memAddr+(int)l->data.size()-1, endAddr);
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
    while (1)
    {
        m_compileRequested.Wait();
        if (m_terminate)
            break;

        DoCompile();

        m_compilationActive = false;
    }
    return 0;
}

void CompilerSourceInfo::BuildMemoryMap()
{
    memset(m_ramColorMap, 0, 65536);
    memset(m_ramDataMap, 0, 65536);
    memset(m_ramMask, 0, 65536);
    for (auto cli : m_lines)
    {
        if (cli->data.size() > 0)
        {
            memcpy(m_ramDataMap + cli->memAddr, cli->data.data(), cli->data.size());
            memset(m_ramMask + cli->memAddr, 255, cli->data.size());
            switch (cli->type)
            {
                case LT_Instruction:
                    memset(m_ramColorMap + cli->memAddr, (0 << 5) + (5 << 2) + 0, cli->data.size());
                    break;
                case LT_Include:
                case LT_ImportedData:
                    memset(m_ramColorMap + cli->memAddr, (5 << 5) + (0 << 2) + 0, cli->data.size());
                    break;
                case LT_DataText:
                    memset(m_ramColorMap + cli->memAddr, (1 << 5) + (2 << 2) + 2, cli->data.size());
                    break;
                case LT_GenerateBytes:
                case LT_GenerateWords:
                case LT_GenerateSprites:
                    memset(m_ramColorMap + cli->memAddr, (0 << 5) + (1 << 2) + 2, cli->data.size());
                    break;
                default:
                    memset(m_ramColorMap + cli->memAddr, (0 << 5) + (0 << 2) + 2, cli->data.size());
                    break;
            }
        }
    }
}

#if 0
TokenisedFile::TokenisedFile(SourceFile* sf)
{
    m_memory = (char*)malloc(64 * 1024 * 0124);
    m_lineCount = (int)sf->GetLines().size();
    m_lines = new TokenisedLine[m_lineCount];
    m_tokens = new TokenisedLine::Token[1024 * 1024];
    m_sourceVersion = sf->GetSourceVersion();

    // note - we can't access this inside the thread but we use it to know what source file compile is in process
    m_sf = sf;

    // setup working directory so we can do includes/imports later
    string path = m_sf->GetPath();
    int pathEnd = (int)path.find_last_of('\\');
    if (pathEnd == string::npos)
    {
        m_workingDir = "";
    }
    else
    {
        m_workingDir = path.substr(0, pathEnd + 1);
    }

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
    m_sourceVersion = sf->GetSourceVersion();

    // note - we can't access this inside the thread but we use it to know what source file compile is in process
    m_sf = sf;

    // setup working directory so we can do includes/imports later
    string path = m_sf->GetPath();
    int pathEnd = (int)path.find_last_of('\\');
    if (pathEnd == string::npos)
    {
        m_workingDir = "";
    }
    else
    {
        m_workingDir = path.substr(0, pathEnd + 1);
    }

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
        else if (token == "&")
        {
            // binary number
            int value;
            if (!Base4ToNumber(fifo.Pop(), value))
                ERR("Unable to parse base4 value");
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

void Compiler::AddLabelsMatchingValue(CompilerSourceInfo* si, vector<CompilerLabel*>& labels, double value)
{
    for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
    {
        auto l = &s_systemLabels[i];
        if (l->m_value == value)
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
            if (l->m_value == value)
            {
                if (std::find(labels.begin(), labels.end(), l) == labels.end())
                {
                    labels.push_back(l);
                }
            }
        }
    }
}

CompilerLabel* Compiler::FindMatchingLabel(CompilerSourceInfo *si, const string& token)
{
    for (int i = 0; i < sizeof(s_systemLabels) / sizeof(CompilerLabel); i++)
    {
        auto l = &s_systemLabels[i];
        if (StrEqual(l->m_name, token))
        {
            return l;
        }
    }

    for (auto l : si->m_labels)
    {
        if (StrEqual(l->m_name, token))
        {
            return l;
        }
    }
    return nullptr;
}

CommandHelp* Compiler::FindMatchingCommand(const string& token)
{
    CommandHelp* cmd = gHELP_CMD;
    while (cmd->name)
    {
        if (StrEqual(token, cmd->name))
            return cmd;
        cmd++;
    }
    return nullptr;
}

void Compiler::AddCommandsContaining(vector<CommandHelp*>& commands, const string& token)
{
    CommandHelp* cmd = gHELP_CMD;
    while (cmd->name)
    {
        if (StrFind(cmd->name, token) != string::npos)
        {
            if (std::find(commands.begin(), commands.end(), cmd) == commands.end())
            {
                commands.push_back(cmd);
            }
        }
        cmd++;
    }
}


void Compiler::LogContextualHelp(SourceFile* sf, int line)
{
    auto help = gApp->GetWindowHelp();
    auto si = sf->GetLines()[line];
    auto ci = sf->GetCompileInfo();
    CompilerLineInfo* li = 0;
    if (ci && ci->m_lines.size() > line)
        li = ci->m_lines[line];

    if (li && !li->error)
    {
        help->Clear();
        switch (li->type)
        {
            case LT_BasicStartup:
            case LT_GenerateBytes:
            case LT_GenerateWords:
            case LT_GenerateSprites:
            case LT_ImportedData:
            case LT_DataBytes:
            case LT_DataWords:
            case LT_Include:
                {
                    if (!si->GetTokens().empty())
                    {
                        auto cmd = FindMatchingCommand(si->GetTokens()[0]);
                        if (cmd)
                        {
                            help->LogText(cmd->help, -1, 1);
                            if (cmd->detail1)
                                help->LogText(cmd->detail1, -1, 1);
                            if (cmd->detail2)
                                help->LogText(cmd->detail2, -1, 1);
                        }
                    }
                }
                break;

            case LT_Variable:
            case LT_Label:
                {
                    auto l = FindMatchingLabel(ci, li->label);
                    if (l)
                    {
                        if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0)) < 0.0000001 && abs(l->m_value) < 1000000000.0f)
                            help->LogText(FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr, 1, (int)(l->m_value));
                        else
                            help->LogText(FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr, 1);
                        if (l->m_help)
                            help->LogTextArray(l->m_help, 1);
                    }
                }
                break;

            case LT_Instruction:
                {
                    auto op = m_cpu->GetOpcode(li->opcode);
                    auto cmd = FindMatchingCommand(op->name);
                    if (cmd)
                    {
                        help->LogText(cmd->help, -1, 1);
                        if (cmd->detail1)
                            help->LogText(cmd->detail1, -1, 1);
                        if (cmd->detail2)
                            help->LogText(cmd->detail2, -1, 1);
                    }

                    // find possible labels
                    if (m_cpu->GetAddressingModeSize(op->addressMode) > 1)
                    {
                        vector<CompilerLabel*> labels;
                        double val = (double)li->operand;
                        if (op->addressMode == Cpu6502::AM_Rel)
                            val = (double)(li->memAddr + li->operand + 2);

                        AddLabelsMatchingValue(ci, labels, val);
                        int colorToggle = 0;
                        for (auto l : labels)
                        {
                            if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0)) < 0.0000001 && abs(l->m_value) < 1000000000.0f)
                                help->LogText(FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr, 1 + colorToggle, (int)(l->m_value));
                            else
                                help->LogText(FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr, 1 + colorToggle);
                            if (labels.size() == 1 && l->m_help)
                                help->LogTextArray(l->m_help, 1 + colorToggle);

                            colorToggle = 1 - colorToggle;
                        }
                    }
                }
                break;

            default:
                break;
        }
    }
    else
    {
        auto& tokens = si->GetTokens();

        int tokenIdx = 0;
        int colorToggle = 0;

        vector<CompilerLabel*> labels;
        vector<CommandHelp*> commands;

        auto cs = sf->GetCompileInfo();
        while (tokenIdx < tokens.size())
        {
            auto& token = tokens[tokenIdx++];
            if (token.empty() || token[0] == ' ' || token[0] == '\t')
                continue;

            if (token == "$" || token == "%" || token == "&")
            {
                tokenIdx++;
                continue;
            }

            if (token[0] >= '0' && token[0] <= '9')
                continue;

            AddLabelsContaining(cs, labels, token);
            AddCommandsContaining(commands, token);
        }

        help->Clear();
        if (!labels.empty())
        {
            help->LogText("Suggested Labels:", 0, -1);
            for (auto l : labels)
            {
                if (l->m_value >= 0 && abs(fmod(l->m_value, 1.0)) < 0.0000001 && abs(l->m_value) < 1000000000.0f)
                    help->LogText(FormatString("%s : $%x  %d", l->m_name.c_str(), (u32)l->m_value, (int)(l->m_value)), l->m_lineNmbr, 1 + colorToggle, (int)(l->m_value));
                else
                    help->LogText(FormatString("%s : %1.2f", l->m_name.c_str(), l->m_value), l->m_lineNmbr, 1 + colorToggle);
                if (labels.size() == 1 && l->m_help)
                    help->LogTextArray(l->m_help, 1 + colorToggle);

                colorToggle = 1 - colorToggle;
            }
        }
        if (!commands.empty())
        {
            help->LogText("Suggested Commands:", -1, 0);
            colorToggle = 0;
            for (auto c : commands)
            {
                help->LogText(c->help, -1, 1 + colorToggle);
                if (c->detail1)
                    help->LogText(c->detail1, -1, 1 + colorToggle);
                if (c->detail2)
                    help->LogText(c->detail2, -1, 1 + colorToggle);
                colorToggle = 1 - colorToggle;
            }
        }
    }
}


