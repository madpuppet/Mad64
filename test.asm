; Test Compiler

forecol = $d020
backcol = $d021

    * = $1000

    ldy #$ff
outerLoop:
    ldx #$ff
loop:
    stx forecol
    stx backcol
    dex
    bne loop
    dey
    bne outerLoop
    rts
