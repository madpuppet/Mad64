.basicStartup

temp = $50

start:
    lda #0
    sta $0400
    sta $0400+39
    sta $0400+24*40
    sta $0400+24*40+39

loop:
    jsr delay
        
    lda #$19
    ora temp
    sta vic.control1

    jsr delay
    
    lda #$19
    ora temp
    sta vic.control1

    lda temp
    clc
    adc #1
    and #7
    sta temp
    jmp loop
    
delay:    
    ldx #50
@lp:
    ldy #255
@lp2:
    dey
    bne @lp2-
    dex
    bne @lp-
    rts
