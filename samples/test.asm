.basicStartup

temp = $50

start:
    lda #$60
    sta temp
    lda #$20
    adc temp
    sta $d020
    rts
    
    