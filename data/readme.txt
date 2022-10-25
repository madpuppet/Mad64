CTRL-L         Load a text file
CTRL-S         Save current text file
CTRL-Q         Quit current text file

INSERT         Toggle Insert/Overwrite mode
SHIFT-INSERT   Toggle Autoindent mode
CTRL-INSERT    Toggle Space/Tabs mode

CTRL-Z         Undo
CTRL-SHIFT-Z   Redo

Hold SHIFT down while move cursor with key or mouse to select text.

CTRL-A         Select All
CTRL-X         Cut selection to copy buffer
CTRL-C         Copy selection to copy buffer
CTRL-V         Paste copy buffer

F1             Open Settings File
CTRL-F5        Assemble


6502 ASSEMBLY DIRECTIVES

4095       - decimal
$0FFF      - hex number
%01010001  - binary number

* = $1000  - set memory ptr

#import "file"    - import a binary file to the current mem addr. path is local to asm file location.

loop:      - label
!loop:     - local label (can have multiple of same name)

bne loop   - branch to loop label
bne loop-  - branch to previous instance of loop
bne loop+  - branch to next instance of loop
dc.b 1,2   - declare some 8 bit values. Also accepts ".byte"
dc.w 1,2   - declare some 16 bit values (little endian).  also accepts ".word"
dc.l 1,2   - declare some 32 bit values (little endian).  also accepts ".long"
dc.t "hi"  - declare some text. also excepts ".text"


