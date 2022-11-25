.basicStartup

result = $50

start:
    sed
lp:
    sec
    lda #$66
    sbc #$00
    sta $d020
    sta $d021
    jmp lp
    