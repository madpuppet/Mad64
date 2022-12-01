.basicStartup

ScanResult = $0400

start:
    lda #$7f 
    sta cia1.interruptControl
    sta cia2.interruptControl

    ldx #$ff
    stx cia1.dataDirectionA     ; set to output
    ldy #$00
    sty cia1.dataDirectionB     ; set to input

loop:
    lda #%11111110
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult
    lda #%11111101
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+1
    lda #%11111011
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+2
    lda #%11110111
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+3
    lda #%11101111
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+4
    lda #%11011111
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+5
    lda #%10111111
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+6
    lda #%01111111
    sta $dc00
    jsr delay
    ldy $dc01
    sty ScanResult+7
    jmp loop

delay:
    inc $d020
    ldy #1
@lp1:
    ldx #1
@lp2:
    dex
    bne @lp2-
    dey
    bne @lp1-
    inc $d020
    rts

    