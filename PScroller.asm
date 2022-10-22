;#import "../includes/c64.lib"

;BasicUpstart2(start)

nextX = $50
nextY = $51
nextCol = $52
anim = $53
anim2 = $5e
currentRaster = $56
spritePattern = $57
textCol = $58
textIn = $54
colOut = $55
textVal1 =  $5a
textVal2 =  $5b
textLines = $5c
tempVar = $5d

sprite0Ptr = $7f8
sprite1Ptr = $7f9
sprite2Ptr = $7fa
sprite3Ptr = $7fb
sprite4Ptr = $7fc
sprite5Ptr = $7fd
sprite6Ptr = $7fe
sprite7Ptr = $7ff

* = $801

start:
    jmp start

    jsr cls

    lda #$1             // enable all the sprites
    sta vic.spriteEnable
    lda #1
    sta vic.spriteMulticolor
    lda #$0                 // reset sprites to size 0
    sta vic.spriteXSize
	sta vic.spriteYSize

    lda #0
    ldx #0
    ldy #0
    jsr $1000

    lda #0
    sta textIn
    sta colOut
    sta textLines
    sta textCol
    sta anim
    sta anim2
    
   lda #0
   sta $d020
   lda #0
   sta $d021
    
    lda #120
    sta tempVar
!loop:
    jsr UpdateText
    dec tempVar
    lda tempVar
    bne !loop-

    sei
    
    ; wait for start of first frame
frameLoop:
    lda vic.control1
    bmi frameLoop
    lda vic.rasterCounter
    cmp #20
    bne frameLoop

    lda #21
    sta vic.sprite0Y
    sta nextY
    sta currentRaster

    ; we are at the start of a frame
    inc anim
    inc anim2
    inc anim2
    inc anim2
    lda anim2
    sta nextX

    ldx anim
    stx nextCol

    lda anim
    and #127
    tax
    lda colors1,x
    sta vic.spriteMulticolor0
    lda colors2,x
    sta vic.sprite0Color
    lda colors3,x
    sta vic.spriteMulticolor1
    
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs

    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
   
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs   
    
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
    
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs

    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
    
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
       
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
    
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs
    
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_xs           

    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x

    lda #$10
    sta $d011

    jsr line_c
    jsr line_xs
    jsr line_c
    jsr line_x

    lda #$18
    sta $d011

    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x
    jsr line_c
    jsr line_x

    lda #0
    sta vic.spriteMulticolor0
    sta vic.sprite0Color
    sta vic.spriteMulticolor1

    lda anim
    lsr
    and #31
    tax
    lda sprites,x
    sta sprite0Ptr
    stx spritePattern
    jsr UpdateText

    lda anim
    lsr
    tax
    lda xwave,x
    sta vic.sprite1Y
    sta vic.sprite2Y
    sta vic.sprite3Y
    sta vic.sprite4Y
    sta vic.sprite5Y
    sta vic.sprite6Y
    sta vic.sprite7Y

    
    jsr $1006
    jmp frameLoop

cls:
    ldx #0
    lda #32
cls_loop:
    sta $0400,x
    sta $0400+250,x
    sta $0400+500,x
    sta $0400+750,x
    inx
    cpx #250
    bne cls_loop
    rts

extend_borders:
   lda #$f8
!hack:
   cmp vic.rasterCounter
   bne !hack-

    lda #$10
    sta $d011

   lda #$fe
!hack:
   cmp vic.rasterCounter
   bne !hack-

    lda #$18
    sta $d011
    rts


frame_sync:
    lda vic.rasterCounter
    cmp #20
    bne frame_sync
    rts

line_x:
    lda currentRaster
!loop:
    cmp vic.rasterCounter
    beq !loop-
    inc currentRaster

    inc nextX
    ldx nextX
    lda xwave,x
    sta vic.sprite0X
    rts

line_xs:
    lda currentRaster
!loop:
    cmp vic.rasterCounter
    beq !loop-
    inc currentRaster

    inc nextX
    ldx nextX
    lda xwave,x
    sta vic.sprite0X

    lda spritePattern
    and #31
    tax
    lda sprites,x
    sta sprite0Ptr
    inc spritePattern

    lda nextY
    adc #21
    sta nextY
    sta vic.sprite0Y
   
    rts

line_c:
    lda currentRaster
!loop:
    cmp vic.rasterCounter
    beq !loop-
    inc currentRaster

    lda vic.rasterCounter
    sta currentRaster

    lda nextCol
    and #127
    tax
    inc nextCol
    
    lda colors1,x
    sta vic.spriteMulticolor0
    lda colors2,x
    sta vic.sprite0Color
    lda colors3,x
    sta vic.spriteMulticolor1
   
   lda currentRaster
!wait:
   cmp vic.rasterCounter
   beq !wait-
   rts

line_s:
    lda currentRaster
!loop:
    cmp vic.rasterCounter
    beq !loop-
    inc currentRaster

    lda nextY
    adc #21
    sta nextY
    sta vic.sprite0Y
    rts


UpdateText:
    lda textLines
    bne moreLines

    inc textCol
    lda textCol
    and #15
    bne colOk
    lda #1
    sta textCol
colOk:
    ; get next text character
    ldx textIn
    inc textIn
    lda scrollText,x
    bpl moreCharacters

    ldx #0
    lda scrollText,x
    inx
    stx textIn
    
moreCharacters:
    asl                         ; convert character to word offset
    tax
    lda chars,x
    sta textVal1
    lda chars+1,x
    sta textVal2
    lda #5
    sta textLines
    
    ; black line to start with
    lda colOut
    and #127
    tay
    lda #0
    sta colors1,y
    sta colors2,y
    sta colors3,y
    inc colOut
    rts
    

moreLines:
    lda colOut
    and #127
    tay
    jsr WriteCol1
    jsr WriteCol2
    jsr WriteCol3
    inc colOut
    dec textLines
    rts

WriteCol1:
    ror textVal2
    ror textVal1
    bcc !noCol+
    lda textCol
    sta colors1,y
    jmp !skip+
!noCol:
    lda #0
    sta colors1,y
!skip:
    rts
    
WriteCol2:
    ror textVal2
    ror textVal1
    bcc !noCol+
    lda textCol
    sta colors2,y
    jmp !skip+
!noCol:
    lda #0
    sta colors2,y
!skip:
    rts
    
WriteCol3:
    ror textVal2
    ror textVal1
    bcc !noCol+
    lda textCol
    sta colors3,y
    jmp !skip+
!noCol:
    lda #0
    sta colors3,y
!skip:
    rts
    
 error1:
     lda #1
     sta $d021
error1_forever:
     jmp error1_forever

 error2:
     lda #2
     sta $d021
  error2_forever:
     jmp error2_forever

    
scrollText:
    .text "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@hello@friends[@@this@is@my@vertical@scrolling@demo@which@uses@a@colour@palette@cycling@technique@to@move@pixels@up@through@a@single@mulitplexed@sprite@with@borders@removed@@@@greetings@to@hitman@of@codehq@@@looking@forward@to@visiting@you@in@japan@and@going@to@revision@next@year@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    .byte 128

chars:
    ; space
    ; 000
    ; 000
    ; 000
    ; 000
    ; 000
    .word 0

    ; A
    ; 010
    ; 101
    ; 111
    ; 101
    ; 101
    .word %101101111101010

    ; B
    ; 110
    ; 101
    ; 110
    ; 101
    ; 110
    .word %011101011101011

    ; C
    ; 110
    ; 101
    ; 100
    ; 101
    ; 110
    .word %011101001101011

    ; D
    ; 110
    ; 101
    ; 101
    ; 101
    ; 110
    .word %011101101101011

    ; E
    ; 111
    ; 100
    ; 110
    ; 100
    ; 111
    .word %111001011001111
    
    ; F
    ; 111
    ; 100
    ; 110
    ; 100
    ; 100
    .word %001001011001111
    
    ; G
    ; 011
    ; 101
    ; 100
    ; 111
    ; 010
    .word %010111001101110
    
    ; H
    ; 101
    ; 101
    ; 111
    ; 101
    ; 101
    .word %101101111101101
    
    ; I
    ; 010
    ; 010
    ; 010
    ; 010
    ; 010
    .word %010010010010010
    
    ; J
    ; 111
    ; 001
    ; 001
    ; 101
    ; 010
    .word %010101100100111
    
    ; K
    ; 101
    ; 110
    ; 100
    ; 110
    ; 101
    .word %101011001011101
    
    ; L
    ; 100
    ; 100
    ; 100
    ; 100
    ; 111
    .word %111001001001001
    
    ; M
    ; 101
    ; 111
    ; 101
    ; 101
    ; 101
    .word %101101101111101
    
    ; N
    ; 110
    ; 101
    ; 101
    ; 101
    ; 101
    .word %101101101101011

    ; O
    ; 010
    ; 101
    ; 101
    ; 101
    ; 010
    .word %010101101101010    
    
    ; P
    ; 110
    ; 101
    ; 110
    ; 100
    ; 100
    .word %001001011101011
    
    ; Q
    ; 010
    ; 101
    ; 101
    ; 111
    ; 011
    .word %110111101101010
    
    ; R
    ; 110
    ; 101
    ; 110
    ; 101
    ; 101
    .word %101101011101011
    
    ; S
    ; 011
    ; 100
    ; 010
    ; 001
    ; 110
    .word %011100010001110
    
    ; T
    ; 111
    ; 010
    ; 010
    ; 010
    ; 010
    .word %010010010010111
    
    ; U
    ; 101
    ; 101
    ; 101
    ; 101
    ; 010
    .word %010101101101101
    
    ; V
    ; 101
    ; 101
    ; 101
    ; 010
    ; 010
    .word %010010101101101
    
    ; W
    ; 101
    ; 101
    ; 111
    ; 111
    ; 101
    .word %101111111101101
    
    ; X
    ; 101
    ; 101
    ; 010
    ; 101
    ; 101
    .word %101101010101101
    
    ; Y
    ; 101
    ; 101
    ; 011
    ; 001
    ; 110
    .word %011100110101101
    
    ; Z
    ; 111
    ; 001
    ; 010
    ; 100
    ; 111
    .word %1110001010100111

   *=$1000-$7e
   .import binary "music.sid"

   *=$2000
colors1:
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0
    .byte 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0

colors2:
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0
    .byte 2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,0

colors3:
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
    .byte 3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,0
   
sprites:
    .byte 128+64+5, 128+64+4, 128+64+4, 128+64+3, 128+64+3, 128+64+2, 128+64+1, 128+64+6
    .byte 128+64+7, 128+64+8, 128+64+9, 128+64+10, 128+64+11, 128+64+10, 128+64+9, 128+64+8
    .byte 128+64+7, 128+64+6, 128+64+1, 128+64+2, 128+64+3, 128+64+3, 128+64+4, 128+64+4
    .byte 128+64+5, 128+64+5, 128+64+5, 128+64+5, 128+64+5, 128+64+5, 128+64+5, 128+64+5

    *=$2200
xwave:
    ; Make data for a sine wave
    .for(var i=0;i<256;i++) .byte round(70 + 16*sin(toRadians(360*i/256)) + 16*sin(toRadians(360*i/128)))

   
	* = $3000  "Sprites"

    ; sprite 0
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000
	.byte %00000000, %01010101, %00000000,0

    ; sprite 1
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000
	.byte %00000000, %01101011, %00000000,0

    ; sprite 2
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000
	.byte %00000001, %01101011, %11000000,0

    ; sprite 3
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000
	.byte %00000101, %01101011, %11110000,0

    ; sprite 4
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100
	.byte %00010101, %10101010, %11111100,0

    ; sprite 5
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111
	.byte %01010101, %10101010, %11111111,0

    ; sprite 6
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000
	.byte %00000000, %11111111, %00000000,0

    ; sprite 7
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000
	.byte %00000000, %11101001, %00000000,0

    ; sprite 8
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000
	.byte %00000011, %11101001, %01000000,0

    ; sprite 9
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000
	.byte %00001111, %11101001, %01010000,0

    ; sprite 10
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100
	.byte %00111111, %10101010, %01010100,0

    ; sprite 11
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101
	.byte %11111111, %10101010, %01010101,0

    ; sprite 12
     .import binary "logo.raw"

	 