.basicStartup

anim = $50
offset = $51
offsetSpeed = $52

param1 = $60
param2 = $61
scroll = $64

tmp1 = $62
tmp2 = $63

start:
    sei
    jsr clr
    ldx #0
    stx anim
    stx offset
    stx offsetSpeed
    lda #vic.BMM+vic.DEN
    sta vic.control1
    lda #vic.MCM
    sta vic.control2

    lda #0
    sta vic.backgroundColor0
    lda #1
    sta vic.backgroundColor1
    lda #2
    sta vic.backgroundColor2
    lda #3
    sta vic.backgroundColor3

loop:
    lda vic.rasterCounter
    bne loop
    lda vic.control1
    bmi loop

@lp:
    lda vic.rasterCounter
    beq @lp-
    lda #12
    sta vic.borderColor

    ldx anim
    lda dotsX,x
    sta param1
    lda anim
    tax
    lda dotsY,x
    sta param2
    jsr plotPoint

    ; switch to bitmap mode
    lda #vic.BMM+vic.DEN
    sta vic.control1
    lda #vic.MCM
    inc scroll
    lda scroll
    and #7
    clc
    adc #vic.MCM
    sta vic.control2
    lda #$18
    sta vic.memoryPointer

@lp:
    lda vic.rasterCounter
    cmp #140
    bne @lp-

    ; switch to text mode
    lda #0
    sta vic.control1
    sta vic.control2
    lda #$14
    sta vic.memoryPointer

    lda #6
    sta vic.borderColor

    ldx anim
    lda loopLow,x
    sta dest+1
    sta dest2+1
    lda loopHigh,x
    clc
    adc #$04
    sta dest+2
    adc #$d4
    sta dest2+2

    lda anim
    clc
    adc offset
dest:
    sta $0400
dest2:
    sta $d800
    
    inc anim
    dec offsetSpeed
    bne notYet
    inc offset
    lda #255
    sta offsetSpeed
notYet:    

    lda #11
    sta vic.borderColor
    jmp loop

plotPoint:      ; (x, y)
    lda param1
    and #7
    tax
    lda pointXmask,x
    pha
    lda param1
    lsr
    lsr
    and #$fe        ; (X/4)&0xfe is offset to x table
    tax
    lda pointX,x
    sta tmp1
    lda pointX+1,x
    sta tmp2
    ldx param2
    lda pointYlow,x
    clc
    adc tmp1
    sta pointDest1 + 1
    sta pointDest2 + 1
    lda pointYHigh,x
    adc tmp2
    sta pointDest1 + 2
    sta pointDest2 + 2
    pla
pointDest1:
    ora $ffff
pointDest2:
    sta $ffff
    rts
    
pointXmask:
    .generate.b 0,7,1<<(7-I)
pointX:
    .generate.w 0,39,$2000+I*8
pointYlow:
    .generate.b 0,199,<((I&7)+floor(I/8)*40*8)
pointYhigh:
    .generate.b 0,199,>((I&7)+floor(I/8)*40*8)

clr:
    ldx #0
@lp:
    lda #$33
    sta $0400,x
    sta $0400+250,x
    sta $0400+500,x
    sta $0400+750,x
    lda #$33
    sta $d800,x
    sta $d800+250,x
    sta $d800+500,x
    sta $d800+750,x
    lda #$0
    sta $2000,x
    sta $2000+250,x
    sta $2000+500,x
    sta $2000+750,x
    sta $2000+1000,x
    sta $2000+1250,x
    sta $2000+1500,x
    sta $2000+1750,x
    sta $2000+2000,x
    sta $2000+2250,x
    sta $2000+2500,x
    sta $2000+2750,x
    sta $2000+3000,x
    sta $2000+3250,x
    sta $2000+3500,x
    sta $2000+3750,x
    inx
    cpx #250
    bne @lp-
    rts    
        
loopLow:
    .generate.b 0,255,(floor(12+sin(I/256*PI*2)*11)*40 + floor(15+sin(I/256*PI*4)*10)) % 256
loopHigh:
    .generate.b 0,255,(floor(12+sin(I/256*PI*2)*11)*40 + floor(15+sin(I/256*PI*4)*10)) / 256

dotsX:
    .generate.b 0,255, cos(I/256*PI*2)*40 + cos(I/256*PI*4)*50 + 121
dotsY:
    .generate.b 0,255, sin(I/256*PI*2)*30 + cos(I/256*PI*16)*10 + 50
    

    