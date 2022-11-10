.basicStartup

temp = $50

start:
    ldx #0
forever:
    dex
    jmp forever

    sei
    lda #0
    sta vic.backgroundColor0

; START OF FRAME
@loop:
    lda vic.control1
    bmi @loop-
    lda vic.rasterCounter
    bne @loop-
    lda #1
    sta vic.borderColor

; FIRST VISIBLE BORDER LINE
@loop:
    lda vic.rasterCounter
    cmp #16
    bne @loop-
    lda #2
    sta vic.borderColor
    
; FIRST VISIBLE BACKGROUND LINE
@loop:
    lda vic.rasterCounter
    cmp #51
    bne @loop-
    lda #3
    sta vic.borderColor

; LAST VISIBLE BACKGROUND LINE    
@loop:
    lda vic.rasterCounter
    cmp #251
    bne @loop-
    lda #4
    sta vic.borderColor

; LAST VISIBLE BORDER LINE
@loop:
    lda vic.control1
    bpl @loop-
    lda vic.rasterCounter
    cmp #31
    bne @loop-
    lda #8
    sta vic.borderColor

    jmp start


    