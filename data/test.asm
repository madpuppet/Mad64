; Test Compiler

    * = $1000
    ldy #$ff
outerLoop:
    ldx #$ff
loop:
    stx vic.backgroundColor0
    stx vic.borderColor
    dex
    bne loop
    dey
    bne outerLoop>
    inc vic.borderColor
    rts
