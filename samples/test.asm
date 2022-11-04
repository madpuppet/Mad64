    .basicStartup($c000)

*=$c000    
    ldx #$ff
outer_lp:
    ldy #$80
inner_lp:
    sty vic.backgroundColor0
    dey
    bne inner_lp
    dex
    bne outer_lp
    rts
    
    