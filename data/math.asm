    .basicStartup

    ldx #$ff
outerLoop:
    ldy #$ff
innerLoop:
    sty vic.backgroundColor0
    stx vic.borderColor
    dey
    bne innerLoop
    dex
    bne outerLoop
    rts
    
