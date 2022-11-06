.basicStartup

start:
    ldx #$4
loop:
    stx $d020
    dex
    bne loop
    rts
