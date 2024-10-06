.basicStartup

varScrX = $50
varScrY = $51
varScrPtr = $52

playerX = $60
playerY = $62
playerVelX = $64
playerVelY = $66

jmpptr = $70

tempA = $80
tempB = $81
tempC = $82
tempD = $83

paramA = $90
paramB = $91
paramC = $92
paramD = $93
paramE = $94
paramF = $95

frameMask = $a0

sprite0Ptr = $7f8
sprite1Ptr = $7f9
sprite2Ptr = $7fa
sprite3Ptr = $7fb
sprite4Ptr = $7fc
sprite5Ptr = $7fd
sprite6Ptr = $7fe
sprite7Ptr = $7ff

start:
    sei

    jsr initGraphics
    jsr clearScreen
    jsr decodeLevel
    jsr startLevel
update:
    inc $d020
    jsr updateEnemies
    jsr updateEnemies
    jsr updateEnemies
    jsr updateEnemies
    jsr updateEnemies
    jsr updateEnemies
    jsr updateEnemies

    jsr updatePlayerPos
    jsr placePlayerSprite
    dec $d020

wait:
    lda $d011
    bpl wait
wait2:
    lda $d011
    bmi wait2

    jmp update

clearScreen:
    lda #$00
    sta _clearLoc+1
    lda #$04
    sta _clearLoc+2
    ldy #24
    ldx #39
_line:
    lda #0
_clearLoc:
    sta $0400,x
    dex
    bpl _clearLoc
    ldx #39
    lda _clearLoc+1
    adc #40
    sta _clearLoc+1
    lda _clearLoc+2
    adc #0
    sta _clearLoc+2
    dey
    bpl _line

    lda #$00
    sta _clearLoc+12
    lda #$d8
    sta _clearLoc+22
    ldy #24
    ldx #39
_line2:
    lda #0
_clearLoc2:
    sta $d800,x
    dex
    bpl _clearLoc2
    ldx #39
    lda _clearLoc2+1
    adc #40
    sta _clearLoc2+1
    lda _clearLoc2+2
    adc #0
    sta _clearLoc2+2
    dey
    bpl _line2
    rts
    
initGraphics:
    lda #(6<<1)+(1<<4)
    sta vic.memoryPointer
    lda #(1<<3)
    sta vic.control2
    lda #(1<<4)+(1<<3)+3
    sta vic.control1
    lda #0
    sta vic.borderColor
    lda #5
    sta vic.backgroundColor0
    lda #56
    sta sprite0Ptr
    lda #$00000001
    sta vic.spriteEnable
    rts
    
decodeLevel:
    lda #1
    sta $400
    sta $427
    sta $7c0
    sta $7e7
    lda #11
    sta $d800
    sta $d827
    sta $dbc0
    sta $dbe7
    rts

; reset screen updater x,y and player x,y    
startLevel:
    lda #0
    sta varScrX
    sta varScrY
    lda #$80
    sta frameMask
    lda #20
    sta playerX+1
    lda #13
    sta playerY+1
    lda #0
    sta playerX
    sta playerY
    sta playerVelX
    sta playerVelX+1
    sta playerVelY
    sta playerVelY+1
    rts

; process a single enemy and then update the screen updater
updateEnemies:
    ldx varScrX
    ldy varScrY
    jsr loadXY          ; read next enemy to process
    pha
    and #$7f
    ora frameMask
    ldx varScrX
    ldy varScrY
    jsr storeXY         ; make this as processed so it will not be ignored next frame
    pla
    tax
    and #$80
    cmp frameMask
    beq doneUpdateEnemies       ; ignore tiles that were written to this frame
    txa                         ; use jump table to process this tile
    and #$7f                    ; only 127 valid types
    asl
    tay
    lda updateEnemyJumpTable,y
    sta jmpptr
    lda updateEnemyJumpTable+1,y
    sta jmpptr+1
    jmp (jmpptr)                ; this function will jump to doneUpdateEnemies when finished

doneUpdateEnemies:
    ldx varScrX                 ; get current x,y being scanned
    ldy varScrY
    inx                         ; update to next column
    cpx #40
    bne _skipReset
    ldx #0
    iny                         ; update to start of next row
    cpy #25
    bne _skipReset
    ldy #0                      ; start frame again
    lda frameMask
    eor #$80                    ; toggle the framemask
    sta frameMask               ; this is used to stop processing a tile twice
    
_skipReset:
    stx varScrX
    sty varScrY
    rts

updateEnemyJumpTable:
    dc.w updateEmpty, updateSpawner, updateMonster
    
updateEmpty:
    jmp doneUpdateEnemies

updateSpawner:
    ldy varScrY                 ; check if we are not on left border
    sty tempB
    ldx varScrX
    beq _leftNotEmpty
    dex                         ; check if square to left is empty
    stx tempA
    jsr loadXY
    and #$7f
    beq _spawnMonster
_leftNotEmpty:    
    ldx varScrX                 ; check we are not on right border
    cpx #39
    beq _rightNotEmpty
    inx                         ; check if square to right is empty
    stx tempA
    ldy varScrY
    jsr loadXY
    and #$7f
    beq _spawnMonster
_rightNotEmpty:
    ldx varScrX
    stx tempA
    ldy varScrY
    beq _aboveNotEmpty          ; check we are not on top border
    dey
    sty tempB
    jsr loadXY
    and #$7f                    ; check square above is empty
    beq _spawnMonster
_aboveNotEmpty:
    ldy varScrY
    cpy #24
    beq _belowNotEmpty          ; check we are not on bottom row
    iny
    sty tempB
    jsr loadXY                  ; check square below is empty
    and #$7f
    beq _spawnMonster
_belowNotEmpty:
    jmp doneUpdateEnemies       ; no-where to spawn, done with this

; left is clear - spawn a monster
_spawnMonster:
    lda #2
    ldx tempA
    ldy tempB
    jsr storeXY
    ldx tempA
    ldy tempB
    lda #2
    jsr storeColorXY
    jmp doneUpdateEnemies

updateMonster:
; move monster towards player
    lda #0
    sta tempA               ; temp A is a flag to see if we can move at all
    ldx varScrX
    ldy varScrY
    cpx playerX+1
    beq _tryVert
    bcc _moveRight
    cpx #0                 ; check we are not on the left border
    beq _tryVert
    dex
    inc tempA
    bne _testHoriz
_moveRight:
    cpx #39                  ; check we can move left
    beq _tryVert
    inx
    inc tempA
_testHoriz:                 ; check horiz position is empty
    stx tempB
    jsr loadXY
    ldx tempB
    ldy varScrY
    and #$7f
    beq _tryVert            ; horiz is good, now try vert
    ldx varScrX
    ldy varScrY             ; horiz taken, revert x
_tryVert:
    cpy playerY+1
    beq _noMoveVert
    bcc _moveDown
    cpy #0                 ; check we can move down    
    beq _noMoveVert
    dey
    inc tempA
    bne _testVert
_moveDown:
    cpy #24
    beq _noMoveVert
    iny
    inc tempA
_testVert:
    stx tempB
    sty tempC
    jsr loadXY
    ldx tempB
    ldy tempC
    and #$7f
    beq _noMoveVert
    ldy varScrY             ; vert is blocked, revert it
_noMoveVert:
    lda tempA    
    beq _noMoveAtAll
    stx paramC
    sty paramD
    ldx varScrX
    stx paramA
    ldy varScrY
    sty paramB
    jsr moveABtoCD
_noMoveAtAll:
    jmp doneUpdateEnemies
   
; load byte from screen location x,y into A
loadXY:
    tya
    asl
    tay
    lda screenLine,y
    sta _lscr+1
    lda screenLine+1,y
    sta _lscr+2
_lscr:
    lda $0400,x
    rts

loadColorXY:
    tya
    asl
    tay
    lda colorLine,y
    sta _loadCol+1
    lda colorLine+1,y
    sta _loadCol+2
_loadCol:
    lda $d800,x
    rts

; store A to screen location x,y, 
storeXY:
    pha
    tya
    asl
    tay
    lda screenLine,y
    sta _sscr+1
    lda screenLine+1,y
    sta _sscr+2
    pla
    ora frameMask
_sscr:
    sta $0400,x
    rts

storeColorXY:
    pha
    tya
    asl
    tay
    lda colorLine,y
    sta _storeCol+1
    lda colorLine+1,y
    sta _storeCol+2
    pla
_storeCol:
    sta $d800,x
    rts
    
; load byte from screen location x,y into A and also mark it with frameMask
markXY:
    tya
    asl
    tay
    lda screenLine,y
    sta _mscr1+1
    sta _mscr2+1
    lda screenLine+1,y
    sta _mscr1+2
    sta _mscr2+2
_mscr1:
    lda $0400,x
    and #$7f
    ora frameMask
_mscr2:
    sta $0400,x
    rts

    
; move byte from XY (AB) to XY (CD)
moveABtoCD:
    ldx paramA
    ldy paramB
    jsr loadXY
    pha
    lda #0
    ldx paramA
    ldy paramB
    jsr storeXY
    pla
    and #$7f
    ora frameMask
    ldx paramC
    ldy paramD
    jsr storeXY
    ; now color
    ldx paramA
    ldy paramB
    jsr loadColorXY
    ldx paramC
    ldy paramD
    jmp storeColorXY
    
placePlayerSprite:
    ; WORLD_X = X+offset
    clc
    lda playerX
    adc #0
    sta tempA
    lda playerX+1
    adc #4
    sta tempB       ; WORLD_X -> tempA,tempB

    ; sprite X = WORLD_X * 8 / 256
    ; 0010 0001 1000 0000 => 1 0000 1100
    ; shift high byte left 3
    ; shift low byte right 5

    lda #0              
    sta vic.spriteXMSB
    lda tempB
    asl
    asl
    asl
    sta tempC
    bcc _smallX             ; if carry set, set the MSB
    lda #1
    sta vic.spriteXMSB
_smallX:
    lda tempA
    lsr
    lsr
    lsr
    lsr
    lsr
    ora tempC
    sta vic.sprite0X

    ; WORLD_Y = Y+offset
    clc
    lda playerY
    adc #0
    sta tempA
    lda playerY+1
    adc #5
    sta tempB

    lda tempB
    asl
    asl
    asl
    sta tempC
    lda tempA
    lsr
    lsr
    lsr
    lsr
    lsr
    ora tempC
    sta vic.sprite0Y
    rts

updatePlayerPos:
    ; check joystick l/r/u/d to change player velocity
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #1
    bne _tryRight
    lda #-90                    ; joypad left so decrease velocity by 10
    sta paramA
    lda #-1
    sta paramB
    bne _addVelX
_tryRight:
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #2
    bne _tryUp
    lda #90                     ; joypad right so increase velocity by 10
    sta paramA
    lda #0
    sta paramB
_addVelX:
    clc
    lda paramA
    adc playerVelX
    sta playerVelX
    lda paramB
    adc playerVelX+1
    sta playerVelX+1
_tryUp:
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #4
    bne _tryDown
    lda #-90                    ; joypad left so decrease velocity by 10
    sta paramA
    lda #-1
    sta paramB
    bne _addVelY
_tryDown:
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #8
    bne _doneVel
    lda #90                     ; joypad right so increase velocity by 10
    sta paramA
    lda #0
    sta paramB
_addVelY:
    clc
    lda paramA
    adc playerVelY
    sta playerVelY
    lda paramB
    adc playerVelY+1
    sta playerVelY+1
_doneVel:
    
    ; clamp X Velocity
    lda playerVelX+1
    cmp #-2
    bne _notXSmall
    lda #-128
    sta playerVelX
    lda #-1
    sta playerVelX+1
_notXSmall:
    cmp #1
    bne _notXBig
    lda #127
    sta playerVelX
    lda #0
    sta playerVelX+1
_notXBig:

    ; clamp Y Velocity
    lda playerVelY+1
    cmp #-2
    bne _notYSmall
    lda #-128
    sta playerVelY
    lda #-1
    sta playerVelY+1
_notYSmall:
    cmp #1
    bne _notYBig
    lda #127
    sta playerVelY
    lda #0
    sta playerVelY+1
_notYBig:

    ; half speed
    lda playerVelX+1
    bmi _goingLeft
    lda playerVelX
    lsr
    sta playerVelX
    jmp _doneX
_goingLeft:
    lda playerVelX
    lsr
    ora #$80
    sta playerVelX
_doneX:
    lda playerVelY+1
    bmi _goingUp
    lda playerVelY
    lsr
    sta playerVelY
    jmp _doneY
_goingUp:
    lda playerVelY
    lsr
    ora #$80
    sta playerVelY
_doneY:

    clc                 ; add x velocity to position
    lda playerVelX
    adc playerX
    sta playerX
    lda playerVelX+1
    adc playerX+1
    sta playerX+1

    clc                 ; add y velocity to position
    lda playerVelY
    adc playerY
    sta playerY
    lda playerVelY+1
    adc playerY+1
    sta playerY+1

    ; clamp sprite to the screen
    lda playerX+1
    cmp #-1
    bne _notLeft
    lda #0
    sta playerX+1
    lda #0
    sta playerX
    sta playerVelX
    sta playerVelX+1
_notLeft:
    lda playerX+1
    cmp #40
    bne _notRight
    lda #39
    sta playerX+1
    lda #$ff
    sta playerX
    lda #0
    sta playerVelX
    sta playerVelX+1
_notRight:
    ; clamp sprite to the screen
    lda playerY+1
    cmp #-1
    bne _notTop
    lda #0
    sta playerY+1
    lda #0
    sta playerY
    sta playerVelY
    sta playerVelY+1
_notTop:
    lda playerY+1
    cmp #25
    bne _notBottom
    lda #24
    sta playerY+1
    lda #$ff
    sta playerY
    lda #0
    sta playerVelY
    sta playerVelY+1
_notBottom:
    rts

screenLine:
    .generate.w 0,24,$0400+I*40
colorLine:
    .generate.w 0,24,$d800+I*40
    dc.b

* = $e00
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000111111000000000
    dc.s %000000001111111100000000    
    dc.s %000001001111111100100000    
    dc.s %000011000011100001100000   
    dc.s %000001111111111111100000    
    dc.s %000000111111111110000000    
    dc.s %000000001111110000000000    
    dc.s %000000001111110000000000
    dc.s %000000011110111000000000    
    dc.s %000000011110111000000000    
    dc.s %000000011100111000000000    
    dc.s %000000111000111100000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.b 0


* = $3000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    
    dc.b %00111000
    dc.b %01111110
    dc.b %11111111
    dc.b %11010101
    dc.b %10101011
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    
    dc.b %00111100
    dc.b %01111100
    dc.b %00111000
    dc.b %11111110
    dc.b %10111010
    dc.b %01111010
    dc.b %01101100
    dc.b %11101110
* = $3400
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    dc.b %00000000
    
    dc.b %00111000
    dc.b %01111110
    dc.b %11111111
    dc.b %11010101
    dc.b %10101011
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    
    dc.b %00111100
    dc.b %01111100
    dc.b %00111000
    dc.b %01111110
    dc.b %11111001
    dc.b %01111101
    dc.b %11100110
    dc.b %11100111

    
