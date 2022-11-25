.basicStartup

start:
    sei

    ; turn off all cia interrupts
    lda #$7f
    sta cia1.interruptControl
    sta cia2.interruptControl

    lda #$0
    sta vic.rasterCounter
    lda #vic.DEN+1
    sta vic.control1
    lda #1
    sta vic.intEnable
    
    lda #$5
    sta $1
    
    lda #0
    sta vic.backgroundColor0
    sta vic.borderColor
    
    lda #<interrupt
    sta $fffe
    lda #>interrupt
    sta $ffff
    cli

    lda #0
    clc
loop:
    sta vic.backgroundColor0
    sta vic.borderColor
    bcc loop

interrupt:
    pha
    lda vic.rasterCounter
    adc #2
    sta vic.rasterCounter
    sta vic.borderColor
    sta vic.backgroundColor0
    asl vic.intRegister
    pla
    rti
