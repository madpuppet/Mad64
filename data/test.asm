temp = $50

.basicStartup
    ldx #$ff
bigloop:
    lda #$20
    sta temp
outerloop:
    ldy #$00
innerloop:
    sty vic.borderColor
    lda colors,x
    sta $400,y
    sta $400+250,y
    sta $400+500,y
    sta $400+750,y
    iny
    cpy #250
    bne innerloop
    stx vic.backgroundColor0
    dec temp
    bne outerloop
    dex
    bne bigloop
    rts

colors:
    .generate.b 0, 255, (sin(I*PI*2)*16+8)

                      