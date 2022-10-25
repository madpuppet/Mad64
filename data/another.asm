*=$1000

    ldx #$ff
outer_loop:
    ldy #$ff
loop:
    sty vic.backgroundColor0
    stx vic.borderColor
    dey
    bne loop
    dex
    bne outer_loop
    rts
