    lda #10
    ldx #ff
@loop:
    sta $d021
    dex
    bne @loop-
@loop:
    rts
    
 