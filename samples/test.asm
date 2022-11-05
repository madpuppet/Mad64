.basicStartup
    
    ldx #$ff
outerloop:
    ldy #textEnd-text
textloop:    
    lda text,y
    sta $0400+40*0,y
    sta $0400+40*1,y
    sta $0400+40*2,y
    sta $0400+40*3,y
    sta $0400+40*4,y
    sta $0400+40*5,y
    sta $0400+40*6,y
    sta $0400+40*7,y
    stx vic.backgroundColor0
    dey
    bpl textloop
    dex
    bne outerloop
    rts

text:
    dc.t "hello world"
textEnd:        
