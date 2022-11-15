.basicStartup

sprite0Ptr = $7f8
sprite1Ptr = $7f9
sprite2Ptr = $7fa
sprite3Ptr = $7fb
sprite4Ptr = $7fc
sprite5Ptr = $7fd
sprite6Ptr = $7fe
sprite7Ptr = $7ff
ptr = $50
xpos = $51

start:
    sei
    lda #0
    sta xpos
loop:
    lda vic.control1
    bmi loop
    lda vic.rasterCounter
    bne loop

    lda #2
    sta vic.borderColor

    lda #24
    clc
    adc xpos
    tax
    stx vic.sprite0X
    stx vic.sprite1X
    inx
    stx vic.sprite2X
    inx
    stx vic.sprite3X
    inx
    stx vic.sprite4X
    inx
    stx vic.sprite5X
    inx
    stx vic.sprite6X
    inx
    stx vic.sprite7X
    ldx #50
    stx vic.sprite0Y
    stx vic.sprite1Y
    inx
    stx vic.sprite2Y
    inx
    stx vic.sprite3Y
    inx
    stx vic.sprite4Y
    inx
    stx vic.sprite5Y
    inx
    stx vic.sprite6Y
    inx
    stx vic.sprite7Y
    lda #0
    sta vic.sprite0Color
    lda #1
    sta vic.sprite1Color
    lda #$1
    sta vic.spriteEnable
    lda #36
    sta sprite0Ptr
    lda #37
    sta sprite1Ptr
    sta sprite2Ptr
    sta sprite3Ptr
    sta sprite4Ptr
    sta sprite5Ptr
    sta sprite6Ptr
    sta sprite7Ptr
    inc xpos
    lda #0
    sta vic.spritePriority
    lda #0
    sta vic.sprite0Color
    lda #$ff
    sta vic.spriteYSize
    lda #0
    sta vic.spriteXSize


wait:
    lda vic.control1
    bmi wait
wait2:
    lda vic.rasterCounter
    cmp #50
    bne wait2

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lda #5
    sta vic.borderColor
    lda #0
    sta vic.spritePriority
    jmp loop
    
    
delay:
    rts
    
*=$900
    dc.s %111111111111111111111111
    dc.s %100000000000000000000001
    dc.s %101111100000001111110001
    dc.s %100111111000000111111001
    dc.s %100011111110000011111101
    dc.s %100001111111100011111101
    dc.s %100000111111110011111101    
    dc.s %100000011111111111111101    
    dc.s %100000001111111111111101   
    dc.s %100000000111111111111101    
    dc.s %100000000111111111111101    
    dc.s %100000000111111111111101    
    dc.s %100000000111111111111101    
    dc.s %100000001111111111111101    
    dc.s %100000011111111111111101    
    dc.s %100011111111111111111101    
    dc.s %101111111111111111111101    
    dc.s %101111111111111111111101    
    dc.s %101111111111111111111101    
    dc.s %100000000000000000000001    
    dc.s %111111111111111111111111
    dc.b 0
    
    dc.s %111111111111111111111100    
    dc.s %111111111111111111111110    
    dc.s %111111110000000011111111    
    dc.s %111111110000000011111111    
    dc.s %111111110000000011111111    
    dc.s %111111110000000011111111    
    dc.s %111111110000000011111111    
    dc.s %111111110000000011111111    
    dc.s %000000000000000000000000    
    dc.s %111111111000000011111111    
    dc.s %111111111100000011111111    
    dc.s %111111111110000011111111    
    dc.s %111111110111000011111111    
    dc.s %111111110011100011111111    
    dc.s %111111110001110011111111    
    dc.s %111111110000111011111111    
    dc.s %111111110000011111111111    
    dc.s %111111110000001111111111    
    dc.s %111111110000000111111111    
    dc.s %011111111111111111111111    
    dc.s %001111111111111111111111
    dc.b 0 
