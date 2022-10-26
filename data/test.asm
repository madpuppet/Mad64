.basicStartup
    lda #10
    ldx #$ff
outputLoop:
    ldy #$ff
innerLoop:
    sta vic.borderColor
    stx vic.backgroundColor0
    dey
    bne innerLoop
    dex
    bne outputLoop
    rts

.generate.b 0, 16, sin(I)*50
    