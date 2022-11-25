#include "common.h"
#include "contextualHelp.h"

CommandHelp gHELP_CMD[] = {
    { ".import", ".import <filename> [offset [length]]", "Import file to memory address with optional offset and length", 0 },
    { "dc.b", "dc.b <expr> [, <expr>]", "Insert some 8 bit values at the current memory address", 0 },
    { "dc.w", "dc.w <expr> [, <expr>]", "Insert some 16 bit values at the current memory address", 0 },
    { "dc.t", "dc.t <screenCodeText>", "Insert a string of screen code characters at the current memory address", 0 },
    { ".generate.b", ".generate.b <start>, <end>, <expr>", "Generate bytes from 'start' to 'end' inclusive", "'I' variable is set to the current index" },
    { ".generate.s", ".generate.s <start>, <end>, <expr>", "Generate 3byte values in big endian from 'start' to 'end' inclusive", "'I' variable is set to the current index" },
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

const char* gHELP_CPU_MEMBANK[] =
{
    "BIT 0..2    0 == ALL RAM         3 == ALL ROM",
    "            5 == RAM and I/O     7 == ROM and I/O",
    "BIT 3       Datasette output signal level",
    "BIT 4       Datasette button status - 1=no button",
    "BIT 5       Datasette motor control- 1=on",0
};


const char* gHELP_VIC_CONTROL1[] = 
{
    "BIT 0..2    YSCL      Y Scroll -3..4",
    "BIT 3       RSEL      Off = 24 row mode (3 lines less on top, 4 less on bottom)",
    "BIT 4       DEN       On = Display Enable",
    "BIT 5       BMM       On = Bitmap Mode",
    "BIT 6       ECM       On = Extended Color Mode",
    "BIT 7       RC8       High bit of raster position",0
};
const char* gHELP_VIC_CONTROL2[] =
{
    "BIT 0..2    XSCL      X Scroll 0..7",
    "BIT 3       CSEL      Off = 38 cols (1 on left & 1 on right missing)",
    "BIT 4       MCM       On = Enable Multicolor Mode",
    "BIT 5       RST       ???",0
};
const char* gHELP_VIC_SCRMEM[] =
{
    "BIT 1..3    CBL       Char Block (0..7) * 2048",
    "BIT 4..7    SCR       Screen Loc (0.15) * 1024", 0
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

const char* gHELP_ROM_ACPTR[] = { "Input byte from serial port",0 };
const char* gHELP_ROM_CHKIN[] = { "Open channel for input", 0 };
const char* gHELP_ROM_CHKOUT[] = { "Open a channel for output", 0 };
const char* gHELP_ROM_CHRIN[] = { "Get a character from the input channel",0 };
const char* gHELP_ROM_CHROUT[] = { "Output a character",0 };
const char* gHELP_ROM_CIOUT[] = { "Transmit a byte over the serial bus",0 };
const char* gHELP_ROM_CINT[] = { "Initialize the screen editor and VIC - II Chip",0 };
const char* gHELP_ROM_CLALL[] = { "Close all open files",0 };
const char* gHELP_ROM_CLOSE[] = { "Close a logical file",0 };
const char* gHELP_ROM_CLRCHN[] = { "Clear all I / O channels",0 };
const char* gHELP_ROM_GETIN[] = { "Get a character",0 };
const char* gHELP_ROM_IOBASE[] = { "Define I / O memory page",0 };
const char* gHELP_ROM_IOINIT[] = { "Initialize I / O devices",0 };
const char* gHELP_ROM_LISTEN[] = { "Command a device on the serial bus to listen",0 };
const char* gHELP_ROM_LOAD[] = { "Load RAM from device",0 };
const char* gHELP_ROM_MEMBOT[] = { "Set bottom of memory",0 };
const char* gHELP_ROM_MEMTOP[] = { "Set the top of RAM",0 };
const char* gHELP_ROM_OPEN[] = { "Open a logical file",0 };
const char* gHELP_ROM_PLOT[] = { "Set or retrieve cursor location",0 };
const char* gHELP_ROM_RAMTAS[] = { "Perform RAM test",0 };
const char* gHELP_ROM_RDTIM[] = { "Read system clock",0 };
const char* gHELP_ROM_READST[] = { "Read status word",0 };
const char* gHELP_ROM_RESTOR[] = { "Set the top of RAM",0 };
const char* gHELP_ROM_SAVE[] = { "Save memory to a device",0 };
const char* gHELP_ROM_SCNKEY[] = { "Scan the keyboard",0 };
const char* gHELP_ROM_SCREEN[] = { "Return screen format",0 };
const char* gHELP_ROM_SECOND[] = { "Send secondary address for LISTEN",0 };
const char* gHELP_ROM_SETLFS[] = { "Set up a logical file",0 };
const char* gHELP_ROM_SETMSG[] = { "Set system message output",0 };
const char* gHELP_ROM_SETNAM[] = { "Set up file name",0 };
const char* gHELP_ROM_SETTIM[] = { "Set the system clock",0 };
const char* gHELP_ROM_SETTMO[] = { "Set IEEE bus card timeout flag",0 };
const char* gHELP_ROM_STOP[] = { "Check if STOP key is pressed",0 };
const char* gHELP_ROM_TALK[] = { "Command a device on the serial bus to talk",0 };
const char* gHELP_ROM_TKSA[] = { "Send a secondary address to a device commanded to talk",0 };
const char* gHELP_ROM_UDTIM[] = { "Update the system clock",0 };
const char* gHELP_ROM_UNLSN[] = { "Send an UNLISTEN command",0 };
const char* gHELP_ROM_UNTLK[] = { "Send an UNTALK command",0 };
const char* gHELP_ROM_VECTOR[] = { "Manage RAM vectors",0 };


const char* gHELP_CIA1_DATAPORTA[] = {
    "Monitoring / control of the 8 data lines of Port A.",
    "  Read / Write : Bit 0..7 keyboard matrix columns",
    "  Read : Joystick Port 2 : Bit 0..3 Direction(Left / Right / Up / Down), Bit 4 Fire button. 0 = activated.",
    "  Read : Lightpen: Bit 4 (as fire button), connected also with “ / LP”(Pin 9) of the VIC",
    "  Read : Paddles: Bit 2..3 Fire buttons, Bit 6..7 Switch control port 1 (% 01 = Paddles A) or 2 (% 10 = Paddles B)", 0
};

const char* gHELP_CIA1_DATAPORTB[] = {
    "Monitoring / control of the 8 data lines of Port B.",
    "  Read / Write : Bit 0..7 keyboard matrix rows",
    "  Read : Joystick Port 1 : Bit 0..3 Direction(Left / Right / Up / Down), Bit 4 Fire button. 0 = activated.",
    "  Read : Bit 6 : Timer A : Toggle / Impulse output(see register 14 bit 2)",
    "  Read : Bit 7 : Timer B : Toggle / Impulse output(see register 15 bit 2)",0
};

const char* gHELP_CIA1_INTCONTROL[] = {
    "CIA1 is connected to the IRQ - Line.",
    "  Read: (Bit0..4 = INT DATA, Origin of the interrupt)",
    "  Bit 0 : 1 = Underflow Timer A",
    "  Bit 1 : 1 = Underflow Timer B",
    "  Bit 2 : 1 = Time of day and alarm time is equal",
    "  Bit 3 : 1 = SDR full or empty, so full byte was transferred, depending of operating mode serial bus",
    "  Bit 4 : 1 = IRQ Signal occured at FLAG - pin(cassette port Data input, serial bus SRQ IN)",
    "  Bit 5..6 : always 0",
    "  Bit 7 : 1 = IRQ An interrupt occured, so at least one bit of INT MASK and INT DATA is set in both registers.",
    "Flags will be cleared after reading the register!",
    "Write : (Bit 0..4 = INT MASK, Interrupt mask)",
    "  Bit 0 : 1 = Interrupt release through timer A underflow",
    "  Bit 1 : 1 = Interrupt release through timer B underflow",
    "  Bit 2 : 1 = Interrupt release if clock = alarmtime",
    "  Bit 3 : 1 = Interrupt release if a complete byte has been received / sent.",
    "  Bit 4 : 1 = Interrupt release if a positive slope occurs at the FLAG - Pin.",
    "  Bit 5..6 : unused",
    "  Bit 7 : Source bit. 0 = set bits 0..4 are clearing the according mask bit. ",
    "           1 = set bits 0..4 are setting the according mask bit.",
    "           If all bits 0..4 are cleared, there will be no change to the mask.",0
};

