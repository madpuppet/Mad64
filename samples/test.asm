.basicStartup

start:
    sei

    jsr clrMem
    
    lda #3
    sta cia2.dataPortA
    
    lda #0
    sta cia1.timerALow
    lda #5
    sta cia1.timerAHigh

    lda #0
    sta cia2.timerALow
    lda #5
    sta cia2.timerAHigh

    lda #%00010001
    sta cia1.controlTimerA

    lda #%00010001
    sta cia2.controlTimerA

    ; disable all the interrupts
    lda #$7f
    sta cia1.interruptControl
    sta cia2.interruptControl
    lda cia1.interruptControl
    lda cia2.interruptControl
    lda #0
    sta vic.intEnable
    sta vic.intRegister

    ; enable cia1 timerA IRQ
    lda #$81
    sta cia1.interruptControl

    ; enable cia2 timerA IRQ
    lda #$81
    sta cia2.interruptControl

    ; set banking to RAM + I/O
    lda #5
    sta $1

    ; point IRQ to our interrupt handler
    lda #<interrupt
    sta $fffe
    lda #>interrupt
    sta $ffff

    ; point IRQ to our interrupt handler
    lda #<nmiinterrupt
    sta $fffa
    lda #>nmiinterrupt
    sta $fffb
    cli

lp:
    jmp lp

interrupt:
    pha
    lda cia1.interruptControl
    inc vic.borderColor
    pla
    rti

nmiinterrupt:
    pha
    lda cia2.interruptControl
    inc vic.backgroundColor0
    pla
    rti

clrMem:
    lda #0
    sta $1

    ldy #00
@loop:
    lda #$ff
@target:
    sta $1000,y
    iny
    bne @target-
    inc @target-+2
    lda @target-+2
    bne @loop-
    
    lda #5
    sta $1
    rts
    
