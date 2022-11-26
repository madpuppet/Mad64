.basicStartup

start:
    sei
    lda #0
    sta cia1.timerALow
    lda #5
    sta cia1.timerAHigh

    lda #10
    sta cia1.timerBLow
    lda #1
    sta cia1.timerBHigh

    ; start & latch timer A
    lda #%00010001
    sta cia1.controlTimerA
    
    ; start & latch timer B in TimerA underflow mode    
    lda #%01010001
    sta cia1.controlTimerB

    ; disable all the interrupts
    lda #$7f
    sta cia1.interruptControl
    sta cia2.interruptControl
    lda cia1.interruptControl
    lda cia2.interruptControl
    lda #0
    sta vic.intEnable
    sta vic.intRegister

    ; enable the timerA IRQ
    lda #$81
    sta cia1.interruptControl

    ; set banking to RAM + I/O
    lda #5
    sta $1

    ; point IRQ to our interrupt handler
    lda #<interrupt
    sta $fffe
    lda #>interrupt
    sta $ffff
    cli

lp:
    jmp lp

interrupt:
    pha
    lda cia1.interruptControl
    lsr
    bcc notTimerA
    ; TIMER A interrupt has gone off - inc background color
    inc vic.backgroundColor0
notTimerA:
    lsr
    bcc notTimerB
    ; TIMER B interrupt has gone off - inc border color
    inc vic.borderColor
notTimerB:
    pla
    rti


    
    