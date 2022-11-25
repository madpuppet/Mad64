; Like the previous example,  except this one uses vic interrupts to detect when collisions happen
; Interrupt routine is triggered on any collision, which increments the border and then clears the bits
; so further interrupts can happen if we are still in conflict

.basicStartup

FRAME_DELAY = 20       ; reduce this for faster update of sprites
delay = $50

start:
    sei

    jsr clr
    
    ; turn off all cia interrupts
    lda #$7f
    sta cia1.interruptControl
    sta cia2.interruptControl

    lda #105
    sta $0463
    sta $054a
    sta $0622
    sta $0730
    
    lda #10
    sta delay
    
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
    sta $0400 + vic.sprite0Ptr,x
    inx
    cpx #8
    bne @lp-

    lda #0
    sta vic.backgroundColor0
    lda #0
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

    lda #2
    sta vic.intRegister
    sta vic.intEnable
    lda #vic.DEN+3
    sta vic.control1
    lda #0
    sta vic.rasterCounter
    
    lda #<InterruptHandler
    sta $fffe
    lda #>InterruptHandler
    sta $ffff
    
    lda #$5
    sta cpu.bank
        
    cli

    ; wait for top of screen
loop:
    lda vic.rasterCounter
    cmp #0
    bne loop
    lda vic.control1
    bmi loop

    dec delay
    bne loop
    lda #FRAME_DELAY
    sta delay

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
    dec xpos,x
skip2:
    lda xpos,x
    sta vic.sprite0X,y
    lda ypos,x
    sta vic.sprite0Y,y
    inx
    cpx #8
    bne @lp-    

    jmp loop

InterruptHandler:
    inc vic.borderColor
    pha
    lda #0
    sta vic.spriteToSpriteCollision
    sta vic.spriteToDataCollision
    lda vic.spriteToSpriteCollision
    lda vic.spriteToDataCollision
    asl vic.intRegister
    pla
    rti
    
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
   
    dc.s %001000000000000000000000
    dc.s %001000000000000000000000
    dc.s %000100000000000000000000
    dc.s %000100000000000000000000
    dc.s %000010000000000000000000
    dc.s %000010000000000000000000
    dc.s %000001000000000000000000    
    dc.s %000001000000000000000000    
    dc.s %000000100000000000000000   
    dc.s %000000100000000000000000    
    dc.s %000000010000000000000000
    dc.s %000000010000000000000000
    dc.s %000000001000000000000000
    dc.s %000000001000000000000000
    dc.s %000000000100000000000000
    dc.s %000000000100000000000000
    dc.s %000000000010000000000000    
    dc.s %000000000010000000000000    
    dc.s %000000000001000000000000   
    dc.s %000000000001000000000000    
    dc.s %000000000000100000000000
    dc.b 0

xpos:
    dc.b 50,100,150,200,210,150,100,50
ypos:
    dc.b 50,100,150,200,54,103,156,208

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

