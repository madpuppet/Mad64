.basicStartup

start:
    lda #$1b
    sta vic.control1

    lda #128+32
    sta $0400
    sta $0400+39
    sta $0400+1000-40
    sta $0400+39+1000-40
    lda #1
    sta $d800
    sta $d800+39
    sta $d800+1000-40
    sta $d800+39+1000-40
    jmp start
    