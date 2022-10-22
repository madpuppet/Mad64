; Test Compiler

forecol = $d020
backcol = $d021
colors1 = $50

    * = $1000

    lda colors1,x

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
