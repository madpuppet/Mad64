.basicStartup

result = $50

start:
    sei
    lda #255
    sta cia1.timerALow
    lda #255
    sta cia1.timerAHigh
    lda #1
    sta cia1.controlTimerA
    lda #1+64
    sta cia1.controlTimerB
lp:
    lda #1
    sta cia1.controlTimerA
    lda #1
    sta cia1.timerALow
    lda #0
    sta cia1.timerAHigh
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lda #1
    sta cia1.timerAHigh

    lda cia1.timerALow
    sta vic.borderColor
    jmp lp
    
    