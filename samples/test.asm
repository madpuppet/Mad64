.basicStartup

temp = $50

start:
    ldx #$ff
loop:
    ldy #$ff
loop2:
    sty vic.borderColor
    dey
    bne loop2
    dex
    bne loop
    jmp start
    
