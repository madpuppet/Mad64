.basicStartup

temp = $50
delay = $51

start:
    sei

    lda #0
    sta delay
    lda #0
    sta temp

    ; fill the screen    
    ldy #4
@lp1:
    ldx #250
@lp2:
    lda #35
target:
    sta $0400
    clc
    adc #0
    inc temp
    ; next location
    lda #1
    clc
    adc target+1
    sta target+1
    lda #0
    adc target+2
    sta target+2
    dex
    bne @lp2-
    dey
    bne @lp1-

    ; main loop
    lda #10
    sta delay
    
loop:
    jsr waitTOF

    ; update yscroll
    dec delay
    bne noScroll
    inc temp
    lda #7
    sta delay
noScroll:

    ; set yscroll
    lda temp
    and #7
    clc
    adc #vic.DEN
    sta vic.control1

    lda temp
    and #7
    clc
    sta vic.control2

    jmp loop

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
    
    