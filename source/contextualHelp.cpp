#include "common.h"
#include "contextualHelp.h"

CommandHelp gHELP_CMD[] = {
    { ".import", ".import <filename> [offset [length]]" },
    { "dc.b", "dc.b <expr> [, <expr>]" },
    { }
}



const char* gHELP_LT_Address[] =
{
    "* = <addr>",
    "Set the current assembly address.",0
};

const char* gHELP_LT_Import[] =
{
    ".import <filename> [offset [length]]",
    "Import file to memory address with optional offset and length",0
};

const char* gHELP_LT_Label[] =
{
    "[@]<label>:",
    "Declare a label at a memory address",
    "Use @ to declare a local label",
    "Branch to a local label by appending '+' to branch to the next occurance",
    "or '-' to branch to the previous occurance of the label"
};

const char* gHELP_LT_Variable[] =
{
    "<variable> = <expr>"
    "Assign a variable with a specific value or mathematical expression",0
};

const char* gHELP_LT_DataBytes[] =
{
    "dc.b <expr> [, <expr>]"
    "Insert some 8 bit values at the current memory address", 0
};

const char* gHELP_LT_DataWords[] =
{
    "dc.w <expr> [, <expr>]"
    "Insert some 16 bit values at the current memory address", 0
};

const char* gHELP_LT_DataText[] =
{
    "dc.t <string>",
    "Insert a string of screen code characters at the current memory address", 0
};

const char* gHELP_LT_GenerateBytes[] =
{
    ".generate.b <start>, <end>, <expr>"
    "Generate values for indices from 'start' to 'end' inclusive",
    "Use variable 'I' in the expression as the current index", 
    "Each value in truncated to a single byte", 0
};

const char* gHELP_LT_GenerateWords[] =
{
    ".generate.b <start>, <end>, <expr>"
    "Generate values for indices from 'start' to 'end' inclusive",
    "Use variable 'I' in the expression as the current index",
    "Each value in truncated to a 16 bit word", 0
};

const char* gHELP_LT_BasicStartup[] =
{
    ".basicStartup [ (<addr>) ]"
    "Insert basic startup code to call into the assembly assembly", 0
};

const char* gHELP_OPC_ADC[] =
{
    "ADC  Add Memory to Accumulator with Carry",
    "A + M + C -> A, C             N Z C I D V",
    "                              + + + - - +",0
};

const char* gHELP_OPC_AND[] =
{
    "AND  And Memory with Accumulator",
    "A AND M -> A                  N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_ASL[] =
{
    "ASL  Shift Left One Bit (Memory or Accumulator)",
    "C <- [76543210] <- 0          N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_BCC[] =
{
    "BCC  Branch on Carry Clear",
    "branch on C = 0               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BCS[] =
{
    "BCS  Branch on Carry Set",
    "branch on C = 1               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BEQ[] =
{
    "BEQ  Branch on Result Zero",
    "branch on Z = 1               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BIT[] =
{
    "BIT  Test Bits in Memory with Accumulator",
    "bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V)",
    "the zero-flag is set to the result of operand AND accumulator",
    "A AND M, M7 -> N, M6 -> V     N Z C I D V",
    "                             M7 + - - - M6",0
};

const char* gHELP_OPC_BMI[] =
{
    "BMI  Branch on Result Minus",
    "branch on N = 1               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BNE[] =
{
    "BNE  Branch on Result not Zero",
    "branch on Z = 0               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BPL[] =
{
    "BPL  Branch on Result Plus",
    "branch on N = 0               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BRK[] =
{
    "BRK  Force Break",
    "BRK initiates a software interrupt similar to a hardware",
    "interrupt (IRQ). The return address pushed to the stack is",
    "PC+2, providing an extra byte of spacing for a break mark",
    "(identifying a reason for the break.)",
    "The status register will be pushed to the stack with the break",
    "flag set to 1. However, when retrieved during RTI or by a PLP",
    "instruction, the break flag will be ignored."
    "The interrupt disable flag is not set automatically.",
    "interrupt,                    N Z C I D V",
    "push PC+2, push SR            - - - 1 - -",0
};

const char* gHELP_OPC_BVC[] =
{
    "BVC  Branch on Overflow Clear",
    "branch on V = 0               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_BVS[] =
{
    "BVS  Branch on Overflow Set",
    "branch on V = 1               N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_CLC[] =
{
    "CLC  Clear Carry Flag",
    "0 -> C                        N Z C I D V",
    "                              - - 0 - - -",0
};

const char* gHELP_OPC_CLD[] =
{
    "CLD  Clear Decimal Mode",
    "0 -> D                        N Z C I D V",
    "                              - - - - 0 -",0
};

const char* gHELP_OPC_CLI[] =
{
    "CLI  Clear Interrupt Disable Bit",
    "0 -> I                        N Z C I D V",
    "                              - - - 0 - -",0
};

const char* gHELP_OPC_CLV[] =
{
    "CLV  Clear Overflow Flag",
    "0 -> V                        N Z C I D V",
    "                              - - - - - 0",0
};

const char* gHELP_OPC_CMP[] =
{
    "CMP  Compare Memory with Accumulator",
    "A - M                         N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_CPX[] =
{
    "CPX  Compare Memory and Index X",
    "X - M                         N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_CPY[] =
{
    "CPY  Compare Memory and Index Y",
    "Y - M                         N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_DEC[] =
{
    "DEC  Decrement Memory by One",
    "M - 1 -> M                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_DEX[] =
{
    "DEX  Decrement Index X by One",
    "X - 1 -> X                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_DEY[] =
{
    "DEY  Decrement Index Y by One",
    "Y - 1 -> Y                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_EOR[] =
{
    "EOR  Exclusive - OR Memory with Accumulator",
    "A EOR M -> A                  N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_INC[] =
{
    "INC  Increment Memory by One",
    "M + 1 -> M                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_INX[] =
{
    "INX  Increment Index X by One",
    "X + 1 -> X                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_INY[] =
{
    "INY  Increment Index Y by One",
    "Y + 1 -> Y                    N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_JMP[] =
{
    "JMP  Jump to New Location",
    "(PC+1) -> PCL                 N Z C I D V",
    "(PC+2) -> PCH                 - - - - - -",0
};

const char* gHELP_OPC_JSR[] =
{
    "JSR  Jump to New Location Saving Return Address",
    "push (PC+2)                   N Z C I D V",
    "(PC+1) -> PCL                 - - - - - -",
    "(PC+2) -> PCH",0
};

const char* gHELP_OPC_LDA[] =
{
    "LDS  Load Accumulator with Memory",
    "M -> A                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_LDX[] =
{
    "LDX  Load Index X with Memory",
    "M -> X                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_LDY[] =
{
    "LDY  Load Index Y with Memory",
    "M -> Y                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_LSR[] =
{
    "LSR  Shift One Bit Right (Memory or Accumulator)",
    "0 -> [76543210] -> C          N Z C I D V",
    "                              0 + + - - -",0
};

const char* gHELP_OPC_NOP[] =
{
    "NOP  No Operation",
    "---                           N Z C I D V",
    "                              0 + + - - -",0
};

const char* gHELP_OPC_ORA[] =
{
    "ORA  OR Memory with Accumulator",
    "A OR M -> A                   N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_PHA[] =
{
    "PHA  Push Accumulator on Stack",
    "push A                        N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_PHP[] =
{
    "PHP  Push Processor Status on Stack",
    "The status register will be pushed with the break",
    "flag and bit 5 set to 1",
    "push SR                       N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_PLA[] =
{
    "PLA  Pull Accumulator from Stack",
    "pull A                        N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_PLP[] =
{
    "PLP  Pull Processor Status from Stack",
    "The status register will be pulled with the break",
    "flag and bit 5 ignored",
    "pull SR                       N Z C I D V",
    "                              from stack",0
};

const char* gHELP_OPC_ROL[] =
{
    "ROL  Rotate One Bit Left (Memory or Accumulator)",
    "C <- [76543210] <- C          N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_ROR[] =
{
    "ROR  Rotate One Bit Right (Memory or Accumulator)",
    "C -> [76543210] -> C          N Z C I D V",
    "                              + + + - - -",0
};

const char* gHELP_OPC_RTI[] =
{
    "RTI  Return from Interrupt",
    "The status register is pulled with the break flag",
    "and bit 5 ignored. Then PC is pulled from the stack",
    "pull SR, pull PC              N Z C I D V",
    "                               from stack",0
};

const char* gHELP_OPC_RTS[] =
{
    "RTS  Return from Subroutine",
    "pull PC, PC+1 -> PC           N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_SBC[] =
{
    "SBC  Subtract Memory from Accumulator with Borrow",
    "A - M - C -> A                N Z C I D V",
    "                              + + + - - +",0
};

const char* gHELP_OPC_SEC[] =
{
    "SEC  Set Carry Flag",
    "1 -> C                        N Z C I D V",
    "                              - - 1 - - -",0
};

const char* gHELP_OPC_SED[] =
{
    "SED  Set Carry Flag",
    "1 -> D                        N Z C I D V",
    "                              - - - - 1 -",0
};

const char* gHELP_OPC_SEI[] =
{
    "SEI  Set Interrupt Disable Status",
    "1 -> I                        N Z C I D V",
    "                              - - - 1 - -",0
};

const char* gHELP_OPC_STA[] =
{
    "STA  Store Accumulator in Memory",
    "A -> M                        N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_STX[] =
{
    "STX  Store Index X in Memory",
    "X -> M                        N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_STY[] =
{
    "STY  Store Index Y in Memory",
    "Y -> M                        N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_TAX[] =
{
    "TAX  Transfer Accumulator to Index X",
    "A -> X                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_TAY[] =
{
    "TAY  Transfer Accumulator to Index Y",
    "A -> Y                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_TSX[] =
{
    "TSX  Transfer Stack Pointer to Index X",
    "SP -> X                       N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_TXA[] =
{
    "TXA  Transfer Index X to Accumulator",
    "X -> A                        N Z C I D V",
    "                              + + - - - -",0
};

const char* gHELP_OPC_TXS[] =
{
    "TXS  Transfer Index X to Stack Register",
    "X -> SP                       N Z C I D V",
    "                              - - - - - -",0
};

const char* gHELP_OPC_TYA[] =
{
    "TYA  Transfer Index Y to Accumulator",
    "Y -> A                        N Z C I D V",
    "                              + + - - - -",0
};
