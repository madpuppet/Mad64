; Test Compiler

*=$1000

	lda #10
	sta $d020
	ldx #5
loop:
    stx $d021
    dex
    bne loop
    rts
