.basicStartup

sprite0Ptr = $7f8
sprite1Ptr = $7f9
sprite2Ptr = $7fa
sprite3Ptr = $7fb
sprite4Ptr = $7fc
sprite5Ptr = $7fd
sprite6Ptr = $7fe
sprite7Ptr = $7ff

start:
    sei
    jsr clr
    
    lda #105
    sta $0463
    sta $054a
    sta $0622
    sta $0730
    
    ldx #0
@lp:
    txa
    clc
    adc #3
    sta vic.sprite0Color,x
    inx
    cpx #8
    bne @lp-

    ldx #0
@lp:
    txa
    and #1
    clc
    adc #36
    sta sprite0Ptr,x
    inx
    cpx #8
    bne @lp-

    lda #0
    sta vic.backgroundColor0
    lda #2
    sta vic.borderColor
    lda #$0
    sta vic.spriteMulticolor
    lda #$ff
    sta vic.spriteEnable
    lda #$0
    sta vic.spriteYSize
    lda #$0
    sta vic.spriteXSize
    lda #0
    sta vic.spritePriority

    ; wait for top of screen
loop:
    lda vic.rasterCounter
    cmp #0
    bne loop
    lda vic.control1
    bmi loop
    
    ldx #0
@lp:
    txa
    asl
    tay

    cpx #4
    bcc @skip+
    inc xpos,x
    jmp skip2
@skip:
    inc ypos,x
skip2:
    lda xpos,x
    sta vic.sprite0X,y
    lda ypos,x
    sta vic.sprite0Y,y

    inx
    cpx #8
    bne @lp-    
    
    jmp loop

*=$900
    dc.s %000000000000000000000000
    dc.s %000000011111111000000000
    dc.s %001111111111111111111100
    dc.s %011111111111111111111110
    dc.s %011111111000001111111111
    dc.s %111111110111111111111111
    dc.s %111111110111111111111111    
    dc.s %111111111000001111111111    
    dc.s %111111111111111111111111   
    dc.s %011111111111111111111110    
    dc.s %001111111111111111111100    
    dc.s %001100000000000000011000    
    dc.s %000110000000000000110000
    dc.s %000011000000000001100000    
    dc.s %000001100000000011000000    
    dc.s %000000110000000110000000    
    dc.s %000000111111111100000000    
    dc.s %000000111111111100000000    
    dc.s %000000011111111000000000    
    dc.s %000000011111111000000000    
    dc.s %000000001111110000000000
    dc.b 0
   
    dc.s %000000000000000000000000
    dc.s %001111111111111111111100
    dc.s %001111111111111111111100
    dc.s %001111110000000011111100
    dc.s %001111110000000011111100
    dc.s %001111110000000011111100
    dc.s %001111111111111111111100    
    dc.s %001111111111111111111100    
    dc.s %001010000010101000010100   
    dc.s %000101000101010000101000    
    dc.s %001010000010101000010100
    dc.s %000101000101010000101000
    dc.s %001010000010101000010100
    dc.s %000101000101010000101000
    dc.s %001111111111111111111100
    dc.s %001111111111111111111100
    dc.s %001111100000000001111100    
    dc.s %001111100000000001111100    
    dc.s %001111111111111111111100   
    dc.s %001111111111111111111100    
    dc.s %000000000000000000000000
    dc.b 0

xpos:
    dc.b 50,60,70,90,120,130,200,250
ypos:
    dc.b 130,80,50,200,77,54,160,190

*=$1000
sinewave:
.generate.b 0,255,sin(I/256*PI*16)*2

clr:
    ldx #4
@lp2:
    lda #32
    ldy #0
@lp:
    sta $0400,y
    sta $0400+250,y
    sta $0400+500,y
    sta $0400+750,y
    iny
    cpy #250
    bne @lp-
    dex
    bne @lp2-    
    rts

    
            


