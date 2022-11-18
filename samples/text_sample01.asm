.basicStartup

charDelay = $50
currChar = $51

start:
    sei
    ldx #1
    stx charDelay
    ldx #0
    stx currChar
    lda #16
    sta vic.control2

    lda #0
    sta vic.backgroundColor0
    lda #1
    sta vic.backgroundColor1
    lda #2
    sta vic.backgroundColor2
    lda #7
    sta vic.backgroundColor3


loop:
    dec charDelay
    bne notYet
    lda #20
    sta charDelay
    inc currChar
notYet:
    lda currChar

    lda #1
    sta vic.borderColor
    
@lp:
    lda currChar
    sta $0400+0,x
    sta $0400+250,x
    sta $0400+500,x
    sta $0400+750,x
    sta $d800+0,x
    sta $d800+250,x
    sta $d800+500,x
    sta $d800+750,x
    inx
    cpx #250
    bne @lp-

    lda #2
    sta vic.borderColor

@wait:
    ldx vic.control1
    bpl @wait-

@wait:
    ldx vic.rasterCounter
    beq @wait-

@wait:
    ldx vic.rasterCounter
    bne @wait-

    jmp loop
    
    rts
        






    


    