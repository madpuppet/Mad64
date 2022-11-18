.basicStartup

sprite0Ptr = $7f8
sprite1Ptr = $7f9
sprite2Ptr = $7fa
sprite3Ptr = $7fb
sprite4Ptr = $7fc
sprite5Ptr = $7fd
sprite6Ptr = $7fe
sprite7Ptr = $7ff
line = $50
anim = $51
wave = $52

start:
    sei
    lda #50
    sta vic.sprite0Y
    lda #0
    sta vic.backgroundColor0
    sta vic.borderColor
    lda #10
    sta vic.spriteMulticolor0
    lda #11
    sta vic.spriteMulticolor1

    ldx #1
@lp:
    txa
    sta vic.sprite0Color,x
    asl
    tay
    asl
    clc
    adc #90
    sta vic.sprite0X,y
    sta vic.sprite0Y,y
    lda #37
    sta sprite0Ptr,x
    iny
    inx
    cpx #8
    bne @lp-

    ldx #1
    lda #36
@lp:
    sta sprite0Ptr,x
    inx
    cpx #4
    bne @lp-

    lda #$ff
    sta vic.spriteEnable
    lda #$f0
    sta vic.spriteMulticolor
    lda #36
    sta sprite0Ptr
    lda #$51
    sta vic.spriteYSize
    lda #$63
    sta vic.spriteXSize
    lda #0
    sta vic.spritePriority

    ; wait for top of screen
loop:
    lda vic.rasterCounter
    cmp #47
    bne loop
    lda vic.control1
    bmi loop
    inc wave
    lda wave
    sta anim

    ldy vic.rasterCounter
loop2:
    cpy vic.rasterCounter
    beq loop2

    ldx anim
    lda sinewave,x
    sta vic.sprite0X
    sta vic.sprite0Color
    lda sinewaveHigh,x
    sta vic.spriteXMSB
    inc anim
    iny
    cpy #100
    bne loop2

@lp:
    lda vic.control1
    bpl @lp-

    ldx #1
@lp:
    txa
    asl
    tay
    asl
    asl
    asl
    clc
    adc #90
    adc anim
    sta vic.sprite0Y,y
    sta vic.sprite0X,y
    iny
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
   
    dc.s &001111111100
    dc.s &011111111110
    dc.s &111311211111
    dc.s &113111211111
    dc.s &113311222111
    dc.s &113131121111
    dc.s &111311121111
    dc.s &011111111110
    dc.s &033333333330
    dc.s &030000000030
    dc.s &030000000030
    dc.s &003000000300
    dc.s &003000000300
    dc.s &000300003000
    dc.s &000333333000
    dc.s &002222222200
    dc.s &222222222222
    dc.s &222222222222
    dc.s &222222222222
    dc.s &022222222220
    dc.s &002222222200
    dc.b 0

    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111   
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111   
    dc.s %111111111111111111111111    
    dc.s %111111111111111111111111
    dc.b 0

*=$1000
sinewave:
.generate.b 0,255,sin(I/256*PI*16)*10+240

sinewaveHigh:
.generate.b 0,255,((sin(I/256*PI*16)*10+240)/256)

