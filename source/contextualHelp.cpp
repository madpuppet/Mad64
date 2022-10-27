#include "common.h"
#include "contextualHelp.h"

CommandHelp gHELP_CMD[] = {
    { ".import", ".import <filename> [offset [length]]", "Import file to memory address with optional offset and length", 0 },
    { "dc.b", "dc.b <expr> [, <expr>]", "Insert some 8 bit values at the current memory address", 0 },
    { "dc.w", "dc.w <expr> [, <expr>]", "Insert some 16 bit values at the current memory address", 0 },
    { "dc.t", "dc.t <screenCodeText>", "Insert a string of screen code characters at the current memory address", 0 },
    { ".generate.b", ".generate.b <start>, <end>, <expr>", "Generate bytes from 'start' to 'end' inclusive", "'I' variable is set to the current index" },
    { ".generate.w", ".generate.w <start>, <end>, < expr>", "Generate words from 'start' to 'end' inclusive", "'I' variable is set to the current index" },
    { ".basicStartup", ".basicStartup [ (<addr>) ]", "Insert basic startup code to call into the assembly assembly", 0 },
    { "ADC", "ADC  Add Memory to Accumulator with Carry", "A + M + C -> A, C             N Z C I D V", "                              + + + - - +" },
    { "AND", "AND  And Memory with Accumulator", "A AND M -> A                  N Z C I D V", "                              + + - - - -" },
    { "ASL", "ASL  Shift Left One Bit (Memory or Accumulator)", "C <- [76543210] <- 0          N Z C I D V", "                              + + + - - -" },
    { "BCC", "BCC  Branch on Carry Clear", "branch on C = 0               N Z C I D V", "                              - - - - - -" },
    { "BCS", "BCS  Branch on Carry Set", "branch on C = 1               N Z C I D V", "                              - - - - - -" },
    { "BEQ", "BEQ  Branch on Result Zero", "branch on Z = 1               N Z C I D V", "                              - - - - - -" },
    { "BIT", "BIT  Test Bits in Memory with Accumulator", "A AND M, M7 -> N, M6 -> V     N Z C I D V", "                             M7 + - - - M6" },
    { "BMI", "BMI  Branch on Result Minus", "branch on N = 1               N Z C I D V", "                              - - - - - -" },
    { "BNE", "BNE  Branch on Result not Zero", "branch on Z = 0               N Z C I D V", "                              - - - - - -" },
    { "BPL", "BPL  Branch on Result Plus", "branch on N = 0               N Z C I D V", "                              - - - - - -" },
    { "BRK", "BRK  initiates a software interrupt similar to a hardware", "interrupt,                    N Z C I D V", "push PC+2, push SR            - - - 1 - -" },
    { "BVC", "BVC  Branch on Overflow Clear", "branch on V = 0               N Z C I D V", "                              - - - - - -" },
    { "BVS", "BVS  Branch on Overflow Set", "branch on V = 1               N Z C I D V", "                              - - - - - -" },
    { "CLS", "CLC  Clear Carry Flag", "0 -> C                        N Z C I D V", "                              - - 0 - - -" },
    { "CLD", "CLD  Clear Decimal Mode", "0 -> D                        N Z C I D V", "                              - - - - 0 -" },
    { "CLI", "CLI  Clear Interrupt Disable Bit", "0 -> I                        N Z C I D V", "                              - - - 0 - -" },
    { "CLV", "CLV  Clear Overflow Flag", "0 -> V                        N Z C I D V", "                              - - - - - 0" },
    { "CMP", "CMP  Compare Memory with Accumulator", "A - M                         N Z C I D V", "                              + + + - - -" },
    { "CPX", "CPX  Compare Memory and Index X", "X - M                         N Z C I D V", "                              + + + - - -" },
    { "CPY", "CPY  Compare Memory and Index Y", "Y - M                         N Z C I D V", "                              + + + - - -" },
    { "DEC", "DEC  Decrement Memory by One", "M - 1 -> M                    N Z C I D V", "                              + + - - - -" },
    { "DEX", "DEX  Decrement Index X by One", "X - 1 -> X                    N Z C I D V", "                              + + - - - -" },
    { "DEY", "DEY  Decrement Index Y by One", "Y - 1 -> Y                    N Z C I D V", "                              + + - - - -" },
    { "EOR", "EOR  Exclusive - OR Memory with Accumulator", "A EOR M -> A                  N Z C I D V", "                              + + - - - -" },
    { "INC", "INC  Increment Memory by One", "M + 1 -> M                    N Z C I D V", "                              + + - - - -" },
    { "INX", "INX  Increment Index X by One", "X + 1 -> X                    N Z C I D V", "                              + + - - - -" },
    { "INY", "INY  Increment Index Y by One", "Y + 1 -> Y                    N Z C I D V", "                              + + - - - -" },
    { "JMP", "JMP  Jump to New Location", "(PC+1) -> PCL                 N Z C I D V", "(PC+2) -> PCH                 - - - - - -" },
    { "JSR", "JSR  Jump to New Location Saving Return Address", "push (PC+2)                   N Z C I D V", "(PC+1) -> PCL                 - - - - - -" },
    { "LDA", "LDA  Load Accumulator with Memory", "M -> A                        N Z C I D V", "                              + + - - - -" },
    { "LDX", "LDX  Load Index X with Memory", "M -> X                        N Z C I D V", "                              + + - - - -" },
    { "LDY", "LDY  Load Index Y with Memory", "M -> Y                        N Z C I D V", "                              + + - - - -" },
    { "LSR", "LSR  Shift One Bit Right (Memory or Accumulator)", "0 -> [76543210] -> C          N Z C I D V", "                              0 + + - - -" },
    { "NOP", "NOP  No Operation", "---                           N Z C I D V", "                              0 + + - - -" },
    { "ORA", "ORA  OR Memory with Accumulator", "A OR M -> A                   N Z C I D V", "                              + + - - - -" },
    { "PHA", "PHA  Push Accumulator on Stack", "push A                        N Z C I D V", "                              - - - - - -" },
    { "PHP", "PHP  Push Processor Status on Stack", "push SR                       N Z C I D V", "                              - - - - - -" },
    { "PLA", "PLA  Pull Accumulator from Stack", "pull A                        N Z C I D V", "                              - - - - - -" },
    { "PLP", "PLP  Pull Processor Status from Stack", "pull SR                       N Z C I D V", "                              from stack" },
    { "ROL", "ROL  Rotate One Bit Left (Memory or Accumulator)", "C <- [76543210] <- C          N Z C I D V", "                              + + + - - -" },
    { "ROR", "ROR  Rotate One Bit Right (Memory or Accumulator)", "C -> [76543210] -> C          N Z C I D V", "                              + + + - - -" },
    { "RTI", "RTI  Return from Interrupt", "pull SR, pull PC              N Z C I D V", "                               from stack" },
    { "RTS", "RTS  Return from Subroutine", "pull PC, PC+1 -> PC           N Z C I D V", "                              - - - - - -" },
    { "SBC", "SBC  Subtract Memory from Accumulator with Borrow", "A - M - C -> A                N Z C I D V", "                              + + + - - +" },
    { "SEC", "SEC  Set Carry Flag", "1 -> C                        N Z C I D V", "                              - - 1 - - -" },
    { "SED", "SED  Set Carry Flag", "1 -> D                        N Z C I D V", "                              - - - - 1 -" },
    { "SEI", "SEI  Set Interrupt Disable Status", "1 -> I                        N Z C I D V", "                              - - - 1 - -" },
    { "STA", "STA  Store Accumulator in Memory", "A -> M                        N Z C I D V", "                              - - - - - -" },
    { "STX", "STX  Store Index X in Memory", "X -> M                        N Z C I D V", "                              - - - - - -" },
    { "STY", "STY  Store Index Y in Memory", "Y -> M                        N Z C I D V", "                              - - - - - -" },
    { "TAX", "TAX  Transfer Accumulator to Index X", "A -> X                        N Z C I D V", "                              + + - - - -" },
    { "TAY", "TAY  Transfer Accumulator to Index Y", "A -> Y                        N Z C I D V", "                              + + - - - -" },
    { "TSX", "TSX  Transfer Stack Pointer to Index X", "SP -> X                       N Z C I D V", "                              + + - - - -" },
    { "TXA", "TXA  Transfer Index X to Accumulator", "X -> A                        N Z C I D V", "                              + + - - - -" },
    { "TXS", "TXS  Transfer Index X to Stack Register", "X -> SP                       N Z C I D V", "                              - - - - - -" },
    { "TYA", "TYA  Transfer Index Y to Accumulator", "Y -> A                        N Z C I D V", "                              + + - - - -" },
    { 0, 0, 0, 0 }
};

const char* gHELP_VIC_CONTROL1[] = 
{
    "BIT 0..2    YSCL      Y Scroll 0..7",
    "BIT 3       RSEL      On = 25 rows",
    "BIT 4       BLNK      On = Blank the screen",
    "BIT 5       BMM       On = Bitmap Mode",
    "BIT 6       ECM       On = Extended Color Mode",
    "BIT 7       RC8       High bit of raster position",0
};
const char* gHELP_VIC_CONTROL2[] =
{
    "BIT 0..2    XSCL      X Scroll 0..7",
    "BIT 3       CSEL      On = 41 cols",
    "BIT 4       MCM       On = Enable Multicolor Mode",
    "BIT 5       RST       ???",0
};
const char* gHELP_VIC_SCRMEM[] =
{
    "BIT 1..3    CBL       Char Block (0..7) * 2048",
    "BIT 4..7    SCR       Screen Loc (0.15) * 1024"
};
const char* gHELP_VIC_IRQREQ[] =
{
    "BIT 0       RIRQ      Set when raster count = stored raster count",
    "BIT 1       ISBC      Set by sprite -> background collision until reset",
    "BIT 2       ISSC      Set by sprite -> sprite collision until reset",
    "BIT 3       LPIRQ     Set by negative transition of light pen",
    "BIT 7       IRQ       Set by latch set and enabled",0
};
const char* gHELP_VIC_INTENA[] =
{
    "BIT 0       RIRQM     Set to allow raster interrupts to occur",
    "BIT 1       MISBC     Set to allow sprite -> background interrupts",
    "BIT 2       MISSC     Set to allow sprite -> sprite interrupts" ,
    "BIT 3       MLPI      Set to allow light pen interrupts",0
};




