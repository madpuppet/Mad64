MAD64 RELEASE 2022.07

Mad64 is planned to be an all in one C64 development environment.
It currently is an editor that assembles your code as you type. IN future it will be able to execute and emulate a c64 while you type.

;====== FILE MANIPULATION

CTRL-L         Load a text file
CTRL-S         Save current text file
CTRL-Q         Quit current text file
CTRL-N         New file (will ask for a name of file, but doesn't create it on disk until you save it)
F1             Open Settings File - saving settings file will auto-apply changed settings in most cases
               Settings file on Windows is located  %appdata%\madpuppet\mad64\settings.ini

;====== EDIT MODES

INSERT         Toggle Insert/Overwrite mode
SHIFT-INSERT   Toggle Autoindent mode
CTRL-INSERT    Toggle Space/Tabs mode

;====== UNDO/REDO

CTRL-Z         Undo
CTRL-SHIFT-Z   Redo

;====== MARKING TEXT,   CUT & PASTE

Hold SHIFT down while move cursor with key or mouse to select text.

CTRL-A         Select All
CTRL-X         Cut selection to copy buffer
CTRL-C         Copy selection to copy buffer
CTRL-V         Paste from copy buffer
DOUBLE CLICK   on any word to select the entire word.  Useful to follow up with a CTRL-F or CTRL-R for search and replace

;====== SEARCH AND REPLACE

CTRL-F         Find text.  If a word is selected, then that word is copied as the search string
CTRL-R         Replace text.  If a word is selected, then that word is copied as the search string and the replace string is cleared
RIGHT CLICK    on the search or replace box to clear and select the box

;====== EMULATION
               
CTRL-F5        Compile and Load PRG file in Vice (use settings file to set up path to VICE)
F10            Single cycle step
SHIFT-F10      Single frame step
ALT-F10        Single rasterline step
CTRL-F10       Run continuously at normal speed

;====== ASSEMBLER SYNTAX

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

;====== ASSEMBLER DIRECTIVES

.import "file"                          - import a binary file to the current mem addr. path is local to asm file location.
.generate.b (0,255,sin(I/255.0*PI))     - generate data bytes using iterator 0 to 255 accessible through I variable
.generate.w (0,255,sin(I/255.0*PI))     - generate data words using iterator 0 to 255 accessible through I variable
.basicStartup                           - generate basic header code to call next line is assembly
.basicStartup(startLabel)               - generate basic header code to call "startLabel" as entry point to assembly

;====== ASSEMBLER EXPRESSIONS

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

;====== BUILT IN CONSTANTS

Mad64 has all the vic, cia and sid registers pre-defined as labels.  use vic.xxx, cia1.xxx, cia2.xxx, vic.xxx
It also has some rom function address and memory locations, though this is incomplete at this date.

vic.background0,  vic.border       -  vic registers
cia1.dataPortA, cia2.timerALow     -  cia 1 & 2 registers
sid.v1_freqLow, sid_v2_freqHi      -  sid registers
rom.SETNAM, rom.LOAD               -  basic rom function addresses


;====== RELEASE NOTES

2022.8

Beginning work on emulator
- can emulate a subset of 6502
- register view in log window
- F10 to single step
- resets emulation after every compile

2022.7

Many polishes to editor.
Search & Replace working
Got Linux version building via CMAKE and binary added to release.
Compiler now works on a thread so can't slow down editting even on massive files.

2022.5

First public release
Editor and compiler are functional.
Search works
Log window has contextual help.
 