.basicStartup

    temp = $50

start:
    ldx #0
@lp:
    lda temp
    sta vic.backgroundColor0
    stx vic.borderColor
    inx
    bne @lp-
    inc temp
    jmp start

