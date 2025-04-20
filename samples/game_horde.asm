.globals
varScrX     byte
varScrY     byte
varCycle    byte
varFrame    byte

varUpdateCount      byte
varFireDownFrames   byte
varBulletDX         byte
varBulletDY         byte
varBulletTile       byte
varBulletStartX     byte
varBulletStartY     byte
varBulletDir        byte

playerX     word
playerY     word
playerVelX  word
playerVelY  word

jmpptr      word

tempA   byte
tempB   byte
tempC   byte
tempD   byte

paramA  byte
paramB  byte
paramC  byte
paramD  byte
paramE  byte
paramF  byte

testLocX        byte
testLocY        byte
bulletTraceX    byte
bulletTraceY    byte

playerFrame     byte    ; sprite frame (LRUD) for player 

frameMask       byte    ; frame masks detemine of a tile has been processed this frame

nextFrag    byte        ; cycle 0..3
fragSprite  word[4]     ; frame and color
fragPos     word[4]     ; x,y locations of frag sprites

.basicStartup

; tiles
TILE_Grass = 0
TILE_Spawner = 1
TILE_Monster = 2
TILE_Wall = 3
TILE_Gate = 4
TILE_Bomb = 5
TILE_LazerH = 6
TILE_LazerV = 7
TILE_LazerTLtoBR = 8
TILE_LazerTRtoBL = 9

; SPRITES
SPRITE_Player = 56
SPRITE_Frag = 60
SPRITE_Clear = 62

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
    inc varCycle
    lda #20
    sta varUpdateCount
enemiesLoop:
    jsr updateEnemies
    dec varUpdateCount
    bne enemiesLoop

    jsr clearBullet
    jsr updatePlayerPos
    jsr placePlayerSprite
    jsr checkForFire
    jsr updateFrags
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
    lda #12
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
    lda #5
    sta vic.borderColor
    lda #5
    sta vic.backgroundColor0
    lda #$0000001f
    sta vic.spriteEnable
    ldx #7
    lda #SPRITE_Clear
_clearSprites:
    sta sprite0Ptr,x
    dex
    bpl _clearSprites    
    
    ldx #15
    lda #0
_clearSpritePos:
    sta vic.sprite0X,x
    dex
    bpl _clearSpritePos

    ldx #6
    lda #8
_clearFrags:
    sta fragSprite,x
    dex
    dex
    bpl _clearFrags

    lda #0 
    sta vic.spriteXMSB
    sta playerFrame
    rts
    
; reset screen updater x,y and player x,y    
startLevel:
    lda #0
    sta varFrame
    sta varCycle
    sta varScrX
    sta varScrY
    sta varBulletTile
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
    inc varFrame
    lda frameMask
    eor #$80                    ; toggle the framemask
    sta frameMask               ; this is used to stop processing a tile twice
    
_skipReset:
    stx varScrX
    sty varScrY
    rts

updateEnemyJumpTable:
    dc.w updateEmpty, updateSpawner, updateMonster, updateEmpty
    dc.w updateEmpty, updateEmpty, updateEmpty, updateEmpty
    dc.w updateEmpty, updateEmpty, updateEmpty, updateEmpty
    
updateEmpty:
    jmp doneUpdateEnemies

updateSpawner:
    lda varFrame
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
    ldx varScrX
    ldy varScrY
    jsr loadColorXY
    ldx tempA
    ldy tempB
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
    ldx paramA          ; check if AB != CD
    ldy paramB
    cpx paramC
    bne _doIt
    cpy paramD
    bne _doIt
    rts
_doIt:
    jsr loadXY          ; get what we are moving
    and #$7f
    ora frameMask       ; mark it correctly for next frame
    ldx paramC
    ldy paramD
    jsr storeXY         ; store it at new location
    ldx paramA
    ldy paramB
    jsr loadColorXY     ; grab old color
    ldx paramC
    ldy paramD
    jsr storeColorXY    ; set new location to old color
    ldx paramA 
    ldy paramB
    lda #0
    jsr storeXY         ; clear old position back to grass
    ldx paramA 
    ldy paramB
    lda #12
    jsr storeColorXY    ; set grass color
    
    
placePlayerSprite:
    ; WORLD_X = X+offset
    clc
    lda #SPRITE_Player
    adc playerFrame
    sta sprite0Ptr
    lda playerX
    adc #140
    sta tempA
    lda playerX+1
    adc #1
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
    adc #160
    sta tempA
    lda playerY+1
    adc #4
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
    ; check joystick u/d/l/r to change player velocity
    lda cia1.dataPortA           ; cia chip holds joypad u/d/l/r/fire
    and #4
    bne _tryRight
    lda #0
    sta playerFrame
    lda #-40                    ; joypad left so decrease velocity
    sta paramA
    lda #-1
    sta paramB
    bne _addVelX
_tryRight:
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #8
    bne _tryUp
    lda #1
    sta playerFrame
    lda #40                     ; joypad right so increase velocity by 10
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
    and #1
    bne _tryDown
    lda #2
    sta playerFrame
    lda #-40                    ; joypad left so decrease velocity by 10
    sta paramA
    lda #-1
    sta paramB
    bne _addVelY
_tryDown:
    lda cia1.dataPortA           ; cia chip holds joypad l/r/u/d/fire
    and #2
    bne _doneVel
    lda #3
    sta playerFrame
    lda #40                     ; joypad right so increase velocity by 10
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

    ; half speed every second cycle
    lda varCycle
    and #1
    beq _doneX
    
    lda playerVelX+1
    bmi _goingLeft
    lda playerVelX
    lsr
    sta playerVelX
    jmp _doneY
_goingLeft:
    lda playerVelX          ; slow X by halving it
    lsr
    ora #$80                ; need to keep it negative
    cmp #$ff                ; if its now -1, just stop
    bne _okLeft
    lda #0
    sta playerVelX+1
_okLeft:
    sta playerVelX
    jmp _doneY
    
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
    cmp #$ff                ; if its now -1, just stop
    bne _okUp
    lda #0
    sta playerVelY+1
_okUp:
    sta playerVelY
_doneY:

    ; add x velocity to position
    clc
    lda playerVelX
    adc playerX
    sta tempA
    lda playerVelX+1
    adc playerX+1
    sta tempB

    ; clamp to screen
    lda tempB
    cmp #-1
    bne _notLeft        ; hit left border?
    lda #0              ; yes
    sta tempB
    lda #0
    sta tempA
    sta playerVelX      ; cancel velocity
    sta playerVelX+1
_notLeft:
    lda tempB
    cmp #40
    bne _notRight       ; hit right border?
    lda #39             ; yes
    sta tempB
    lda #$ff
    sta tempA
    lda #0
    sta playerVelX      ; cancel velocity
    sta playerVelX+1
_notRight:

    ; check player can move here
    ldx tempB
    stx testLocX
    ldy playerY+1
    sty testLocY
    jsr doPlayerOnXY    ; do actions for hitting tile
    bne _cantMoveX      ; returns zero if we can move here
    lda tempA
    sta playerX
    lda tempB
    sta playerX+1
_cantMoveX:

    ; add y velocity to position
    clc                 
    lda playerVelY
    adc playerY
    sta tempA
    lda playerVelY+1
    adc playerY+1
    sta tempB

    ; clamp y to screen
    lda tempB
    cmp #-1
    bne _notTop
    lda #0
    sta tempB
    lda #0
    sta tempA
    sta playerVelY
    sta playerVelY+1
_notTop:
    lda tempB
    cmp #25
    bne _notBottom
    lda #24
    sta tempB
    lda #$ff
    sta tempA
    lda #0
    sta playerVelY
    sta playerVelY+1
_notBottom:

    ; check player can move here
    ldx playerX+1
    stx testLocX
    ldy tempB
    sty testLocY
    jsr doPlayerOnXY    ; do actions for hitting tile
    bne _cantMoveY      ; returns zero if we can move here
    lda tempA
    sta playerY
    lda tempB
    sta playerY+1
_cantMoveY:
    rts

screenLine:
    .generate.w 0,24,$0400+I*40
colorLine:
    .generate.w 0,24,$d800+I*40

* = $e00
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000111111000000
    dc.s %000000000001111111000000
    dc.s %000000000000011100000000    
    dc.s %000000000111101000000000    
    dc.s %000000000000011100000000   
    dc.s %000000000001111111000000    
    dc.s %000000000000111111000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000001111110000000
    dc.s %000000000001111111000000
    dc.s %000000000000011100000000    
    dc.s %000000000000001011110000    
    dc.s %000000000000011100000000   
    dc.s %000000000001111111000000    
    dc.s %000000000001111110000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0

    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000010000000000
    dc.s %000000000000010000000000
    dc.s %000000000001010100000000
    dc.s %000000000011010110000000    
    dc.s %000000000011101110000000    
    dc.s %000000000011111110000000   
    dc.s %000000000011101110000000    
    dc.s %000000000011000110000000
    dc.s %000000000011000110000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0
    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000011000110000000
    dc.s %000000000011000110000000
    dc.s %000000000011101110000000
    dc.s %000000000011111110000000    
    dc.s %000000000011101110000000    
    dc.s %000000000011010110000000   
    dc.s %000000000001010100000000    
    dc.s %000000000000010000000000
    dc.s %000000000000010000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0

    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000100000000010000000
    dc.s %000000001100100100000000
    dc.s %000000000110111000000000
    dc.s %000010001000001000000000
    dc.s %000001100100110011000000
    dc.s %000000010000011100000000
    dc.s %000000001011100100000000
    dc.s %000000111000100111000000
    dc.s %000001000100010000110000
    dc.s %000000010010100100000000
    dc.s %000000000010100110000000
    dc.s %000000000000000001000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0

    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000010000000000000000000    
    dc.s %000001000000000000000000
    dc.s %000000100000000010000000
    dc.s %000000001000100100000000
    dc.s %000000000000001000000100
    dc.s %000010000000000000110000
    dc.s %000001000000010011000000
    dc.s %000000010000000100000000
    dc.s %000000001000000000000000
    dc.s %000000000000100110000000
    dc.s %000001000000010000010000
    dc.s %000010000000000000011000
    dc.s %000100000010100000000010
    dc.s %000000000100000001000000    
    dc.s %000000000100000001000000    
    dc.s %000000000000000000100000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0

    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000   
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0


* = $3000
    ; 0. grass
    dc.b %00000000
    dc.b %00000000
    dc.b %00100000
    dc.b %00000000
    dc.b %00000000
    dc.b %00001000
    dc.b %01000000
    dc.b %00000000
    ; 1. spawner    
    dc.b %00111000
    dc.b %01111110
    dc.b %11111111
    dc.b %11010101
    dc.b %10101011
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    ; 2. monster
    dc.b %00111100
    dc.b %01111100
    dc.b %00111000
    dc.b %11111110
    dc.b %10111010
    dc.b %01111010
    dc.b %01101100
    dc.b %11101110
    ; 3. wall
    dc.b %11110111
    dc.b %11110111
    dc.b %11110111
    dc.b %00000000
    dc.b %11111110
    dc.b %11111110
    dc.b %11111110
    dc.b %00000000
    ; 4. gate
    dc.b %00000000
    dc.b %01111110
    dc.b %11101111
    dc.b %11000111
    dc.b %11101111
    dc.b %11000111
    dc.b %01111110
    dc.b %00000000
    ; 5. bomb
    dc.b %00010000
    dc.b %00001000
    dc.b %00010000
    dc.b %01111110
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    dc.b %01111110
    ; 6. lazerH
    dc.b %00000000
    dc.b %01000100
    dc.b %00000000
    dc.b %11101110
    dc.b %10111011
    dc.b %00000000
    dc.b %00001000
    dc.b %01000000
    ; 7. lazerV
    dc.b %00011000
    dc.b %01010000
    dc.b %00011001
    dc.b %00001000
    dc.b %00011000
    dc.b %10010000
    dc.b %00011010
    dc.b %00001000
    ; 8. lazerTLtoBR
    dc.b %10000000
    dc.b %01101000
    dc.b %00100000
    dc.b %00110001
    dc.b %01001100
    dc.b %00000100
    dc.b %00000110
    dc.b %00100001
    ; 9. lazerTRtoBL
    dc.b %00010001
    dc.b %00000010
    dc.b %00001100
    dc.b %10001000
    dc.b %00110000
    dc.b %00100010
    dc.b %01100000
    dc.b %10000100

* = $3400
    ; 0. grass
    dc.b %00000000
    dc.b %00000000
    dc.b %00100000
    dc.b %00000000
    dc.b %00000000
    dc.b %00001000
    dc.b %01000000
    dc.b %00000000
    ; 1. spawner    
    dc.b %00111000
    dc.b %01111110
    dc.b %11111111
    dc.b %11010101
    dc.b %10101011
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    ; 2. monster
    dc.b %00111100
    dc.b %01111100
    dc.b %00111000
    dc.b %11111110
    dc.b %10111010
    dc.b %01111010
    dc.b %01101100
    dc.b %11101110
    ; 3. wall
    dc.b %11110111
    dc.b %11110111
    dc.b %11110111
    dc.b %00000000
    dc.b %11111110
    dc.b %11111110
    dc.b %11111110
    dc.b %00000000
    ; 4. gate
    dc.b %00000000
    dc.b %01111110
    dc.b %11101111
    dc.b %11000111
    dc.b %11101111
    dc.b %11000111
    dc.b %01111110
    dc.b %00000000
    ; 5. bomb
    dc.b %00010000
    dc.b %00001000
    dc.b %00010000
    dc.b %01111110
    dc.b %11111111
    dc.b %11111111
    dc.b %11111111
    dc.b %01111110
    ; 6. lazerH
    dc.b %00000000
    dc.b %01000100
    dc.b %00000000
    dc.b %11101110
    dc.b %10111011
    dc.b %00000000
    dc.b %00001000
    dc.b %01000000
    ; 7. lazerV
    dc.b %00011000
    dc.b %01010000
    dc.b %00011001
    dc.b %00001000
    dc.b %00011000
    dc.b %10010000
    dc.b %00011010
    dc.b %00001000
    ; 8. lazerTLtoBR
    dc.b %10000000
    dc.b %01101000
    dc.b %00100000
    dc.b %00110001
    dc.b %01001100
    dc.b %00000100
    dc.b %00000110
    dc.b %00100001
    ; 9. lazerTRtoBL
    dc.b %00010001
    dc.b %00000010
    dc.b %00001100
    dc.b %10001000
    dc.b %00110000
    dc.b %00100010
    dc.b %01100000
    dc.b %10000100

decodeLevel:
    lda #1
    sta $400
    sta $427
    sta $7c0
    sta $7e7
    lda #11
    sta $d800
    lda #12
    sta $d827
    lda #13
    sta $dbc0
    lda #14
    sta $dbe7

    lda #3
    sta $400+5*40
    sta $400+5*40+1
    sta $400+5*40+2
    sta $400+5*40+3
    sta $400+7*40+4
    sta $400+7*40+5
    sta $400+7*40+6
    sta $400+5*40+7
    sta $400+5*40+8
    sta $400+5*40+9
    sta $400+5*40+10
    sta $400+5*40+11
    sta $400+5*40+13
    sta $400+5*40+14
    sta $400+5*40+15
    sta $400+5*40+16
    sta $400+5*40+17
    sta $400+5*40+18
    sta $400+6*40+18
    sta $400+7*40+18
    sta $400+8*40+18
    sta $400+9*40+18
    sta $400+10*40+18
    sta $400+11*40+18
    sta $400+12*40+18
    sta $400+13*40+18
    sta $400+14*40+18
    sta $400+14*40+19
    sta $400+14*40+20
    sta $400+14*40+22
    sta $400+14*40+23
    sta $400+14*40+24
    sta $400+14*40+25
    sta $400+14*40+26
    sta $400+14*40+27
    sta $400+14*40+28
    sta $400+14*40+29
    sta $400+14*40+30
    sta $400+14*40+31
    sta $400+14*40+32
    sta $400+14*40+33
    sta $400+14*40+34
    sta $400+14*40+35
    sta $400+14*40+36
    sta $400+14*40+37
    sta $400+14*40+38
    sta $400+14*40+39
    sta $400+16*40+18
    sta $400+16*40+19
    sta $400+16*40+20
    sta $400+17*40+22
    sta $400+17*40+23
    sta $400+17*40+24
    sta $400+17*40+25
    sta $400+17*40+26
    sta $400+17*40+27
    sta $400+17*40+28
    sta $400+17*40+29
    sta $400+17*40+30
    sta $400+17*40+31
    sta $400+17*40+33
    sta $400+17*40+34
    sta $400+17*40+35
    sta $400+17*40+36
    sta $400+17*40+37
    sta $400+17*40+38
    sta $400+17*40+39

    lda #4
    sta $400+5*40+12
    sta $400+14*40+21
    sta $400+17*40+32
    rts

doPlayerOnXY:
    ldx testLocX
    ldy testLocY
    jsr loadXY
    and #$7f
    asl
    tay
    lda stepOnActionJumpTable,y
    sta jmpptr
    lda stepOnActionJumpTable+1,y
    sta jmpptr+1
    jmp (jmpptr)                ; this function will jump to doneUpdateEnemies when finished

stepOnActionJumpTable:
    dc.w actionNone, actionBlock, actionBlock, actionBlock
    dc.w actionDestroy, actionNone, actionNone, actionNone
    dc.w actionNone, actionNone
    
actionNone:
    lda #0
    rts
    
actionBlock:
    lda #1
    rts
    
actionDestroy:
    lda #0
    ldx testLocX
    ldy testLocY
    jsr storeXY
    lda #12
    ldx testLocX
    ldy testLocY
    jmp storeColorXY

fireDirectionTable:
    dc.b -1,-1,-1,-1, -1,3,1,2, -1,5,7,6, -1,4,0,-1
directionDXTable:
    dc.b -1,1,0,0
directionDYTable:
    dc.b 0,0,-1,1
bulletTileTable:
    dc.b TILE_LazerH, TILE_LazerH
    dc.b TILE_LazerV, TILE_LazerV
    
checkForFire:
    ; first update the bullet direction
    ; ready to fire - get fire direction
    lda cia1.dataPortA           ; cia chip holds joypad u/d/l/r/fire
    and #16
    beq _buttonDown
    lda #0
    sta varFireDownFrames
_noFire:
    rts
_buttonDown:
    inc varFireDownFrames
    lda varFireDownFrames
    and #3
    cmp #1
    bne _noFire

    ldx playerFrame ; store all the bullet info
    lda bulletTileTable,x       ; we will clear bullets next frame
    sta varBulletTile
    lda directionDXTable,x
    sta varBulletDX
    lda directionDYTable,x
    sta varBulletDY
    lda playerX+1
    sta varBulletStartX
    lda playerY+1
    sta varBulletStartY

    ; now draw the bullet
    lda varBulletStartX
    sta bulletTraceX
    lda varBulletStartY
    sta bulletTraceY
_bulletLoop:
    clc
    lda bulletTraceX
    adc varBulletDX
    sta bulletTraceX
    tax
    clc
    lda bulletTraceY
    adc varBulletDY
    sta bulletTraceY
    tay

    ; check bullet is not offscreen
    cpx #40
    bcs _doneBullet
    cpy #25
    bcs _doneBullet

    jsr loadXY
    and #$7f
    beq _emptySpotForBullet
    ; handle tile hit
    asl
    tay
    lda onLazerHitActionTable,y
    sta jmpptr
    lda onLazerHitActionTable+1,y
    sta jmpptr+1
    jmp (jmpptr)                ; this function will jump to doneUpdateEnemies when finished

_doneBullet:
    rts

onLazerHitActionTable:
    dc.w hitNone, hitDestroy, hitDestroy, hitNone
    dc.w hitNone, hitNone, hitNone, hitNone
    dc.w hitNone, hitNone
    
hitNone:
    rts

hitDestroy:
    lda #0
    sta paramA
    ldx bulletTraceX
    ldy bulletTraceY    
    stx paramC
    sty paramD
    jsr loadColorXY
    sta paramB
    jsr AddFrag
    
    ldx bulletTraceX
    ldy bulletTraceY
    lda #0
    jsr storeXY
    lda #12
    ldx bulletTraceX
    ldy bulletTraceY
    jsr storeColorXY
    rts

_emptySpotForBullet:
    lda varBulletTile
    ldx bulletTraceX
    ldy bulletTraceY
    jsr storeXY
    lda #1
    ldx bulletTraceX
    ldy bulletTraceY
    jsr storeColorXY
    jmp _bulletLoop    

clearBullet:
    lda varBulletTile
    bne _validPath
    rts
_validPath:
    lda varBulletStartX
    sta bulletTraceX
    lda varBulletStartY
    sta bulletTraceY
_clearBulletLoop:
    clc
    lda bulletTraceX
    adc varBulletDX
    sta bulletTraceX
    tax
    clc
    lda bulletTraceY
    adc varBulletDY
    sta bulletTraceY
    tay
    ; check bullet is not offscreen
    cpx #40
    bcs _doneCBullet
    cpy #25
    bcs _doneCBullet

    jsr loadXY
    and #$7f
    cmp varBulletTile
    bne _doneCBullet

    ; clear bullet back to grass
    lda #0
    ldx bulletTraceX
    ldy bulletTraceY
    jsr storeXY
    lda #12
    ldx bulletTraceX
    ldy bulletTraceY
    jsr storeColorXY
    jmp _clearBulletLoop

_doneCBullet:
    lda #0
    sta varBulletTile
    rts

AddFrag:
    inc nextFrag
    lda nextFrag 
    and #3
    sta nextFrag
    asl
    tax
    lda paramA
    sta fragSprite,x
    lda paramB
    sta fragSprite+1,x        
    lda paramC
    sta fragPos,x
    lda paramD
    sta fragPos+1,x
    rts

updateFrags:
    lda #0
    sta paramF    
    lda #0
_updateFragLoop:
    pha
    tay
    asl
    tax
    lda fragSprite,x
    lsr
    lsr
    clc
    adc #SPRITE_Frag
    sta sprite1Ptr,y
    cmp #SPRITE_Clear
    beq _notThisBaby
    inc fragSprite,x
    lda fragSprite+1,x
    sta vic.sprite1Color,y

    ldy fragPos,x
    lda charToSpriteXLow,y
    sta vic.sprite1X,x
    lda paramF
    ora charToSpriteXHigh,y
    asl
    sta paramF
    ldy fragPos+1,x
    lda charToSpriteY,y
    sta vic.sprite1Y,x    
_notThisBaby:
    pla
    clc
    adc #1
    cmp #4
    bne _updateFragLoop
    lda vic.spriteXMSB
    and #1
    ora paramF
    sta vic.spriteXMSB
    rts
    
    
charToSpriteXLow:
    .generate.b 0,39,I*8+16
charToSpriteXHigh:
    .generate.b 0,39,(I*8+16)>>8
charToSpriteY:
    .generate.b 0,39,I*8+45


