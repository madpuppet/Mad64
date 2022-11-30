
.basicStartup

ScanResult = $0400

start:
    rts
    sei
    lda #$81
    sta cia1.interruptControl
    lda #50
    sta cia1.timerALow
    sta cia1.timerAHigh
    
    lda #<interrupt
    sta $314
    lda #>interrupt
    sta $315
    cli
    rts

*=$2000

interrupt:
    inc $d020

    ldx #$ff
    stx cia1.dataDirectionA     ; set to output
    ldy #$00
    sty cia1.dataDirectionB     ; set to input

loop:
    lda #%11111110
    sta $dc00
    ldy $dc01
    sty ScanResult
    lda #%11111101
    sta $dc00
    ldy $dc01
    sty ScanResult+1
    lda #%11111011
    sta $dc00
    ldy $dc01
    sty ScanResult+2
    lda #%11110111
    sta $dc00
    ldy $dc01
    sty ScanResult+3
    lda #%11101111
    sta $dc00
    ldy $dc01
    sty ScanResult+4
    lda #%11011111
    sta $dc00
    ldy $dc01
    sty ScanResult+5
    lda #%10111111
    sta $dc00
    ldy $dc01
    sty ScanResult+6
    lda #%01111111
    sta $dc00
    ldy $dc01
    sty ScanResult+7

    jmp $ea31
    

    