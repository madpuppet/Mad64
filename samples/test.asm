.basicStartup

start:
    ldx #$ff
loop:
    stx $d020
    dex
    bne loop
    rts
