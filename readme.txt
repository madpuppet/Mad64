MAD64 RELEASE 2022.07

Mad64 is planned to be an all in one C64 development environment.
It currently is an editor that assembles your code as you type. IN future it will be able to execute and emulate a c64 while you type.


CTRL-L         Load a text file
CTRL-S         Save current text file
CTRL-Q         Quit current text file
CTRL-N         New file (will ask for a name of file, but doesn't create it on disk until you save it)

INSERT         Toggle Insert/Overwrite mode
SHIFT-INSERT   Toggle Autoindent mode
CTRL-INSERT    Toggle Space/Tabs mode

CTRL-Z         Undo
CTRL-SHIFT-Z   Redo

Hold SHIFT down while move cursor with key or mouse to select text.

CTRL-A         Select All
CTRL-X         Cut selection to copy buffer
CTRL-C         Copy selection to copy buffer
CTRL-V         Paste from copy buffer

F1             Open Settings File - saving settings file will auto-apply changed settings in most cases
CTRL-F5        Compile and Load PRG file in Vice (use settings file to set up path to VICE)


6502 ASSEMBLY DIRECTIVES

4095       - decimal
$0FFF      - hex number
%01010001  - binary number

* = $1000  - set memory ptr

loop:      - label
!loop:     - local label (can have multiple of same name)

bne loop   - branch to loop label
bne loop-  - branch to previous instance of loop
bne loop+  - branch to next instance of loop
dc.b 1,2   - declare some 8 bit values. Also accepts ".byte"
dc.w 1,2   - declare some 16 bit values (little endian).  also accepts ".word"
dc.l 1,2   - declare some 32 bit values (little endian).  also accepts ".long"
dc.t "hi"  - declare some text. also accepts ".text"

COMMANDS

.import "file"                          - import a binary file to the current mem addr. path is local to asm file location.
.generate.b (0,255,sin(I/255.0*PI))     - generate data bytes using iterator 0 to 255 accessible through I variable
.generate.w (0,255,sin(I/255.0*PI))     - generate data words using iterator 0 to 255 accessible through I variable
.basicStartup                           - generate basic header code to call next line is assembly
.basicStartup(startLabel)               - generate basic header code to call "startLabel" as entry point to assembly

EXPRESSIONS

(5 + 5) * 2  - BODMAS order of operations is followed,  brackets can be used to give higher priority
+, -, *, /   - add, subtract, multiply, divide
%            - integer modulo
<<, >>       - bitwise left & right shift
~, ^, &, |   - bitwise not, xor, and, or
||, &&, ==, <, >, <=, >=    - logical operators Or, And, Equals, Less, Greater, Less or Equal, Greater or Equal
A ? B : C    - ternary operator - if A then B else C
round(x)     - round to closet integral
floor(x)     - drop to closest integral
mod(x,y)     - floating point module x % y
sin(x)       - sin of x radians
cos(x)       - cos of x radians
tan(x)       - tan of x radians
asin(x)      - arc sin - note that invalid ranges will just return 0 to avoid NaNs
acos(x)      - arc cos
atan(x)      - arc tan
atan2(x,y)   - arc tangent in 2d
rand(a,b)    - integer random number >= a and <= b
rad(a)       - convert a from degrees to radians
deg(a)       - convert a from radians to degrees


CONSTANTS

Mad64 has all the vic, cia and sid registers pre-defined as labels.  use vic.xxx, cia1.xxx, cia2.xxx, vic.xxx
It also has some rom function address and memory locations, though this is incomplete at this date.

vic.background0,  vic.border       -  vic registers
cia1.dataPortA, cia2.timerALow     -  cia 1 & 2 registers
sid.v1_freqLow, sid_v2_freqHi      -  sid registers
rom.SETNAM, rom.LOAD               -  basic rom function addresses



