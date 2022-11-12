* = $50
temp:
    dc.b 0
lookup:
    dc.b 0,0

.basicStartup

start:
    sei

startLoop:
    ldx #0
    stx vic.borderColor
    stx vic.backgroundColor0
loop:
    lda temp
    and #1
    clc
    adc #30
    sta $0400,x
    sta $0400+960,x
    txa
    and #5
    clc
    adc #1
    sta $d800,x
    sta $d800+960,x
    inx
    cpx #40
    bne loop

    ldx #0
sides:
    lda line,x
    sta lookup
    lda line+1,x
    sta lookup+1

    ldy #0
    sta (lookup),y
    ldy #39
    sta (lookup),y
    inx
    inx
    cpx #50
    bne sides

    inc temp
    jmp startLoop
    
line:
.generate.w 0,25,$0400+I*40
    
    
