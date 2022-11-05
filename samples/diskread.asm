*=$c000
ldx #$24
stx $0124
lda #$01
tay
jsr rom.SETNAM
lda #$08
ldx #$60
sta $ba
stx $b9

jsr $f3d5
jsr $f219
ldy #$04

branch1:
jsr $ee13
dey
bne branch1
lda $c6
ora $90
bne branch3
jsr $ee13
tax
jsr $ee13
jsr $bdcd
branch2:
jsr $ee13
jsr rom.CHROUT
bne branch2
jsr $aad7
ldy #$02
bne branch1
branch3:
jsr $f642
jmp $f6f3