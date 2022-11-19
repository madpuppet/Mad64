.basicStartup

temp = $50

start:
    sei
    lda #0
    sta $0400
    sta $0400+39
    lda #6
    sta $0400+24*40
    sta $0400+24*40+39
    lda #7
    sta $0400+23*40
    sta $0400+23*40+39

    lda #0
    sta temp

@l1:
    jsr waitTOF
    lda #0
    sta vic.borderColor
    lda #vic.DEN+vic.RSEL+0
    sta vic.control1
    
    lda #99
    jsr waitRL
    lda #vic.DEN+vic.RSEL+7
    sta vic.control1
    lda #1
    sta vic.borderColor

    lda temp
    clc
    adc #1
    and #7
    sta temp

    jmp @l1-



loop:
    jsr waitTOF
    lda #0
    sta vic.borderColor

waitRasterline:
    lda vic.rasterCounter
    cmp #100
    bne waitRasterline

    lda #14
    sta vic.borderColor
        
    lda #vic.BLNK+vic.RSEL
    ora temp
    sta vic.control1

waitRasterline2:
    lda vic.rasterCounter
    cmp #150
    bne waitRasterline2
    lda #6
    sta vic.borderColor
    lda #vic.BLNK+vic.RSEL
    sta vic.control1

    lda temp
    clc
    adc #1
    and #7
    sta temp
    jmp loop
    
delay:    
    ldx #150
@lp:
    ldy #255
@lp2:
    dey
    bne @lp2-
    dex
    bne @lp-
    rts

waitTOF:
    lda vic.rasterCounter
    bne waitTOF
    lda vic.control1
    bmi waitTOF
    rts
waitNextRL:
    lda vic.rasterCounter
waitRL:
    cmp vic.rasterCounter
    bne waitRL
    rts
    
MarkLines:
    rts
    
    