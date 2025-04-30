.globals
varScrPtr   word
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
playerKeys  byte

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
bulletDist      byte

playerFrame     byte    ; sprite frame (LRUD) for player 

frameMask       byte    ; frame masks detemine of a tile has been processed this frame

nextFrag    byte        ; cycle 0..3
fragSprite  word[4]     ; frame and color
fragPos     word[4]     ; x,y locations of frag sprites

playerHitpoints byte    ; 255->0
playerAmmo byte         ; 255->0
hpDamageFlash byte      ; if non zero, then use red bar

frameCounter byte       ; cycles each frame - used to have things only fire on some frames
gateTimer byte          ; when this runs out, all gates and walls open

.locals

; decode level locffals
dl_index byte
dl_tile byte
dl_color byte
dl_x byte
dl_y byte
dl_readPtr word

.basicStartup

MAX_BAR_VALUE = 35
BULLET_RANGE = 10

; tiles
TILE_Grass = 0
TILE_Spawner = 1
TILE_Monster = 2
TILE_Wall = 3
TILE_Gate = 4
TILE_Bomb = 5
TILE_Ammo = 6
TILE_Key = 7
TILE_LazerH = 8
TILE_LazerV = 9
TILE_LazerTLtoBR = 10
TILE_LazerTRtoBL = 11

; SPRITES
SPRITE_Player = 128
SPRITE_Frag = 132
SPRITE_Clear = 135
SPRITE_UI = 136

; COLORS
COLOR_Border = 0
COLOR_Background = 0
COLOR_Grass = 12

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
    jsr initUISprites
    jsr clearScreen
    jsr decodeLevel
    jsr startLevel
update:
    inc vic.borderColor
    inc varCycle
    bne _notNow
    lda gateTimer
    beq _notNow
    dec gateTimer
_notNow:
    lda playerHitpoints
    beq gameOverLoop

    jsr setBackColor
    lda #10
    sta varUpdateCount
enemiesLoop:
    jsr nextSrcXY
    jsr updateEnemies
    dec varUpdateCount
    bne enemiesLoop
    dec vic.borderColor

    jsr clearBullet
    jsr updatePlayerPos
    jsr placePlayerSprite
    jsr checkForFire
    jsr updateFrags
    jsr refreshHitpoints
    jsr refreshAmmo
    jsr drawHitpoints
    jsr drawAmmo

   inc vic.borderColor

    jsr showUI
    
    dec vic.borderColor

    jmp update

gameOverLoop:
    lda vic.rasterCounter
    bne gameOverLoop
    lda varCycle
    and #3
    tax
    lda gameOverColor,x
    sta vic.backgroundColor0
    sta vic.borderColor
    jsr showUI
    jmp update

gameOverColor:
    dc.b 2,0,2,11

setBackColor:
    lda #1
    ldx hpDamageFlash
    beq _setBack
    lda #4
_setBack:
    sta vic.sprite0Color
    rts

clearScreen:
    ldx #0
_clearLoop:
    lda #0
    sta $0400,x
    sta $0400+250,x
    sta $0400+500,x
    sta $0400+750,x
    lda #COLOR_Grass
    sta $d800,x
    sta $d800+250,x
    sta $d800+500,x
    sta $d800+750,x
    inx
    cpx #250
    bne _clearLoop
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
    lda #0
    sta vic.backgroundColor0
    lda #1
    sta vic.backgroundColor1
    lda #2
    sta vic.backgroundColor2
    lda #3
    sta vic.backgroundColor3
    lda #$ff
    sta vic.spriteEnable    
    lda #%11100000
    sta vic.spriteMulticolor
    sta vic.spriteXSize
    sta vic.spriteYSize
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
    lda #<randomTileTable
    sta varScrPtr
    lda #>randomTileTable
    sta varScrPtr+1
    lda #0
    sta varFrame
    sta varCycle
    sta varScrX
    sta varScrY
    sta varBulletTile
    sta playerKeys
    lda #20
    sta playerX+1
    lda #13
    sta playerY+1
    lda #0
    sta hpDamageFlash
    sta playerX
    sta playerY
    sta playerVelX
    sta playerVelX+1
    sta playerVelY
    sta playerVelY+1
    lda #MAX_BAR_VALUE
    sta playerHitpoints
    sta playerAmmo
    lda #5
    sta gateTimer
    rts

nextSrcXY:
    ldy #0
    lda (varScrPtr),y
    sta varScrX
    iny
    lda (varScrPtr),y
    sta varScrY
    lda varScrPtr
    clc
    adc #2
    sta varScrPtr
    bcc _noOver
    inc varScrPtr+1
_noOver:
    lda varScrPtr
    cmp #<randomTileTableEnd
    bne _noEnd
    lda varScrPtr+1
    cmp #>randomTileTableEnd
    bne _noEnd
    lda #<randomTileTable
    sta varScrPtr
    lda #>randomTileTable
    sta varScrPtr+1
_noEnd:
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
    dc.w updateEmpty, updateSpawner, updateMonster, updateGate
    dc.w updateGate, updateEmpty, updateEmpty, updateEmpty
    dc.w updateEmpty, updateEmpty, updateEmpty, updateEmpty

updateGate:
    lda gateTimer
    bne _noDestroy
    ldx varScrX
    ldy varScrY
    lda #2
    jsr storeXY
_noDestroy:
    jmp doneUpdateEnemies
    
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

    ; did we hit the player
    ldx varScrX
    ldy varScrY
    cpx playerX+1
    bne _notYet
    cpy playerY+1
    bne _notYet
    jsr enemyHitPlayer
_notYet:
    jmp doneUpdateEnemies

; call this if enemy at X,Y has hit the player
; it will deduct hitpoints and frag at the spot
enemyHitPlayer:
    stx tempC
    sty tempD
    lda #TILE_Grass
    jsr storeXY
    ldx tempC
    ldy tempD
    lda #COLOR_Grass
    jsr storeColorXY        
    lda #4
    sta paramA
    jsr subtractHitpoints
    lda #0
    sta paramA
    lda #1
    sta paramB
    lda tempC
    sta paramC
    lda tempD
    sta paramD
    jmp AddFrag
   
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
    lda #COLOR_Grass
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
    lda #-30                    ; joypad left so decrease velocity
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
    lda #30                     ; joypad right so increase velocity by 10
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
    lda #-30                    ; joypad left so decrease velocity by 10
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
    lda #30                     ; joypad right so increase velocity by 10
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

* = $2000
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
    dc.s %000010001101001000000000
    dc.s %000001100111110011000000
    dc.s %000000010111111100000000
    dc.s %000000001011100100000000
    dc.s %000000111100100111000000
    dc.s %000001011100010000110000
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
    dc.s %000010000010000000110000
    dc.s %000001000000010011000000
    dc.s %000000010000000100000000
    dc.s %000010001000000000000000
    dc.s %000000000000100110000000
    dc.s %000001000000010000010000
    dc.s %000010001000001000011000
    dc.s %000100000010100001000010
    dc.s %000000000100000001000000    
    dc.s %000000000100000001000000    
    dc.s %000000000000000000100000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.s %000000000000000000000000    
    dc.b 0

    dc.s %000000000000000000000000    
    dc.s %000000000100000000000000    
    dc.s %000010000000001000000000    
    dc.s %000001000000000000001000
    dc.s %000000100010000010000000
    dc.s %000000001000100100000000
    dc.s %000000000000001001000100
    dc.s %000010000010000000110000
    dc.s %000001000000000001000000
    dc.s %000000000000000000000000
    dc.s %000010000000000000000000
    dc.s %010000000000000010000000
    dc.s %000001000000000000010000
    dc.s %000010001000001000011000
    dc.s %000100000000100000000010
    dc.s %000000000100000000000000    
    dc.s %001000000100000000000010    
    dc.s %000000000000000000001000    
    dc.s %000001000000001000000000    
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

HPSpriteStart:
    dc.s &111111111111   
    dc.s &111111111111    
    dc.s &000000000000    
AmmoSpriteStart:
    dc.s &333333333333    
    dc.s &333333333333    
    dc.s &000000000000    
    dc.s &000002200000    
    dc.s &222002000000    
    dc.s &202002200000    
    dc.s &202002000000    
    dc.s &202022200000    
    dc.s &222020200000    
    dc.s &000022200000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.b 0

    dc.s &111111111111   
    dc.s &111111111111    
    dc.s &000000000000    
    dc.s &333333333333    
    dc.s &333333333333    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &222022202220    
    dc.s &202020202020    
    dc.s &202020202020    
    dc.s &202020202020    
    dc.s &222022202220    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.b 0

    dc.s &111111111110   
    dc.s &111111111110    
    dc.s &000000000000    
    dc.s &333333333330    
    dc.s &333333333330    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &222022202220    
    dc.s &202020202020    
    dc.s &202020202020    
    dc.s &202020202020    
    dc.s &222022202220    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.s &000000000000    
    dc.b 0



* = $3000
    ; 0. grass
     dc.b %01010000
     dc.b %10100000
     dc.b %00000101
     dc.b %00001010
     dc.b %10100000
     dc.b %01010000
     dc.b %00001010
     dc.b %00000101

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
    ; 6. ammo
    dc.b %00000000
    dc.b %00100100
    dc.b %01100110
    dc.b %01100110
    dc.b %01100110
    dc.b %01100110
    dc.b %01110111
    dc.b %00000000
    ; 6. key
    dc.b %00000000
    dc.b %00111110
    dc.b %00110000
    dc.b %00111100
    dc.b %00110000
    dc.b %01111110
    dc.b %01100110
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
    dc.b %01010000
    dc.b %10100000
    dc.b %00000101
    dc.b %00001010
    dc.b %10100000
    dc.b %01010000
    dc.b %00001010
    dc.b %00000101
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
    ; 6. ammo
    dc.b %00000000
    dc.b %00100100
    dc.b %01100110
    dc.b %01100110
    dc.b %01100110
    dc.b %01100110
    dc.b %01110111
    dc.b %00000000
    ; 6. key
    dc.b %00000000
    dc.b %00111110
    dc.b %00110000
    dc.b %00111100
    dc.b %00110000
    dc.b %01111110
    dc.b %01100110
    dc.b %01111110

    ; 7. lazerH
    dc.b %00000000
    dc.b %01000100
    dc.b %00000000
    dc.b %11101110
    dc.b %10111011
    dc.b %00000000
    dc.b %00001000
    dc.b %01000000
    ; 8. lazerV
    dc.b %00011000
    dc.b %01010000
    dc.b %00011001
    dc.b %00001000
    dc.b %00011000
    dc.b %10010000
    dc.b %00011010
    dc.b %00001000
    ; 9. lazerTLtoBR
    dc.b %10000000
    dc.b %01101000
    dc.b %00100000
    dc.b %00110001
    dc.b %01001100
    dc.b %00000100
    dc.b %00000110
    dc.b %00100001
    ; 10. lazerTRtoBL
    dc.b %00010001
    dc.b %00000010
    dc.b %00001100
    dc.b %10001000
    dc.b %00110000
    dc.b %00100010
    dc.b %01100000
    dc.b %10000100

scoreDigitsOffsets:
    dc.b 0,5,10,15,20,25,30,35,40,45
scoreDigits:
    dc.b &2220
    dc.b &2020
    dc.b &2020
    dc.b &2020
    dc.b &2220

    dc.b &0200
    dc.b &2200
    dc.b &0200
    dc.b &0200
    dc.b &2220

    dc.b &2220
    dc.b &0020
    dc.b &2220
    dc.b &2000
    dc.b &2220

    dc.b &2220
    dc.b &0020
    dc.b &2220
    dc.b &0020
    dc.b &2220

    dc.b &2020
    dc.b &2020
    dc.b &2220
    dc.b &0020
    dc.b &0020

    dc.b &2220
    dc.b &2000
    dc.b &2220
    dc.b &0020
    dc.b &2220

    dc.b &2220
    dc.b &2000
    dc.b &2220
    dc.b &2020
    dc.b &2220

    dc.b &2220
    dc.b &0020
    dc.b &0020
    dc.b &0020
    dc.b &0020

    dc.b &2220
    dc.b &2020
    dc.b &2220
    dc.b &2020
    dc.b &2220

    dc.b &2220
    dc.b &2020
    dc.b &2220
    dc.b &0020
    dc.b &0020



levelData00:
    dc.b TILE_Spawner,14,0,0,39,1,26,6,255
    dc.b TILE_Spawner,5,39,14,39,17,25,10,255
    dc.b TILE_Spawner,7,39,24,30,24,25,24,20,24,255
    dc.b TILE_Spawner,3,0,5,0,10,0,15,0,20,8,16,255
    dc.b TILE_Wall
    dc.b 8,5,5,5,6,5,7,5,8,5,9,5,10,5,11,5,12,6,5,7,5,8,5,9,5,10,5,11,5,12,5
    dc.b 13,5,14,5,15,5,16,5,17,5,18,5,19,5,20,5,21,5,22,5,23,5,24,5
    dc.b 24,6,24,7,24,8,24,9
    dc.b 10,10,10,11,10,12,10,13,10,14,10,15,10,16,10,17,10,18,10,19,10,20
    dc.b 10,21,10,22,10,23,10,24
    dc.b 24,10,24,11,24,12,24,13,24,14
    dc.b 0,8,1,8,2,8,3,8,4,8
    dc.b 25,8,26,8,27,8,28,8,29,8,30,8,31,8,32,8,33,8,34,8,35,8,36,8,37,8,38,8,39,8
    dc.b 24,11,24,12,24,13,24,14,24,15,24,16,24,17,24,18,24,19,24,20
    dc.b 25,20,26,20,27,20,28,20,29,20,30,20,31,20,32,20,33,20,34,20,35,20
    dc.b 36,20,37,20,38,20,39,20,5,13,5,14,5,15,5,16,5,17,5,18,6,18,7,18,8,18,9,18
    dc.b 255
    dc.b TILE_Gate,7,10,20,24,16,16,5,255
    dc.b TILE_Key,7,7,16,37,15,3,10,37,6,255
    dc.b TILE_Ammo,3,14,20,7,1,6,6,26,6,255
    dc.b 255

decodeLevel:
    lda #$00
    sta frameMask
    lda #<levelData00
    sta dl_readPtr
    lda #>levelData00
    sta dl_readPtr+1

    ldy #0
    sty dl_index
_decodeTileLoop:
    lda (dl_readPtr),y
    bmi _doneLevelDecode
    sta dl_tile
    iny
    lda (dl_readPtr),y
    sta dl_color
    iny
_decodeTileLocLoop:
    lda (dl_readPtr),y    
    bmi _doneTileDecode
    sta dl_x
    tax
    iny
    lda (dl_readPtr),y
    sta dl_y
    iny
    sty dl_index
    tay
    lda dl_tile
    jsr storeXY
    lda dl_color
    ldx dl_x
    ldy dl_y
    jsr storeColorXY
    ldy dl_index
    jmp _decodeTileLocLoop
_doneTileDecode:
    iny
    tya
    clc
    adc dl_readPtr
    sta dl_readPtr
    lda #0
    adc dl_readPtr+1
    sta dl_readPtr+1
    ldy #0
    sta dl_index
    beq _decodeTileLoop    
_doneLevelDecode:
    rts    
_loadLevelByte:
    lda $1000
    inc _loadLevelByte+1
    bne _noLLBOverflow
    inc _loadLevelByte+2
_noLLBOverflow:
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
    ; this function will jump to doneUpdateEnemies when finished
    ; note that TempA and TempB are used to keep player positions, so don't overwrite
    jmp (jmpptr)
    
stepOnActionJumpTable:
    dc.w actionNone, actionBlock, actionFragDamage, actionBlock
    dc.w actionOpenGate, actionNone, actionCollectAmmo, actionCollectKey
    dc.w actionNone, actionNone
    
actionNone:
    lda #0
    rts
    
actionBlock:
    lda #1
    rts

actionFragDamage:
    ldx testLocX
    ldy testLocY
    jsr enemyHitPlayer
    lda #0
    rts
    
destroyTestLoc:
    lda #0
    ldx testLocX
    ldy testLocY
    jsr storeXY
    lda #12
    ldx testLocX
    ldy testLocY
    jsr storeColorXY
    lda #0
    rts

actionOpenGate:
    lda playerKeys
    beq _gateLocked
    dec playerKeys
    jmp destroyTestLoc
_gateLocked:
    lda #1
    rts

actionCollectAmmo:
    lda #35
    sta playerAmmo
    jmp destroyTestLoc

actionCollectKey:
    inc playerKeys
    jmp destroyTestLoc


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

    ; any ammo left?
    lda playerAmmo
    beq _noFire
    dec playerAmmo

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
    lda #BULLET_RANGE
    sta bulletDist

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

    ; restrict range of bullet
    dec bulletDist
    beq _doneBullet

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
    lda #3
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
    beq _isBlankSprite
    inc fragSprite,x
    lda fragSprite+1,x
    sta vic.sprite1Color,y
_isBlankSprite:

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
    pla
    sec
    sbc #1
    bpl _updateFragLoop
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

showUI:
_waitForBeforeBorder:
    lda vic.rasterCounter
    cmp #249
    bne _waitForBeforeBorder
    lda vic.control1
    and #$f7
    sta vic.control1

    ;; we can set the bar colour while we wait
    ldx hpDamageFlash
    bne _flashRed
    ldx #8
    stx vic.spriteMulticolor0
    bne _waitForBorder
_flashRed:
    dec hpDamageFlash
    ldx #2
    stx vic.spriteMulticolor0

_waitForBorder:
    ldx vic.rasterCounter
    cpx #255
    bne _waitForBorder
    ora #8
    sta vic.control1
    rts

initUISprites:
    lda #SPRITE_UI
    sta sprite5Ptr
    lda #SPRITE_UI+1
    sta sprite6Ptr
    lda #SPRITE_UI+2
    sta sprite7Ptr
    lda #110
    sta vic.sprite5X
    lda #158
    sta vic.sprite6X
    lda #206
    sta vic.sprite7X
    lda #254
    sta vic.sprite5Y
    sta vic.sprite6Y
    sta vic.sprite7Y
    lda #8
    sta vic.spriteMulticolor0
    lda #7
    sta vic.spriteMulticolor1
    lda #5
    sta vic.sprite5Color
    lda #14
    sta vic.sprite6Color
    sta vic.sprite7Color
    rts

drawHitpoints:
    lda #<HPSpriteStart
    sta paramA
    lda #>HPSpriteStart
    sta paramB
    lda playerHitpoints
    sta paramC
    lda #<hpData
    sta paramD
    lda #>hpData
    sta paramE
    jmp drawBar
    
drawAmmo:
    lda #<AmmoSpriteStart
    sta paramA
    lda #>AmmoSpriteStart
    sta paramB
    lda playerAmmo
    sta paramC
    lda #<ammoData
    sta paramD
    lda #>ammoData
    sta paramE
    jmp drawBar

hpData:
    dc.b &0000,&1000,&1100,&1110,&1111
ammoData:
    dc.b &0000,&3000,&3300,&3330,&3333
barOffsets:
    dc.b 1,1,62,1,1,62,1,1

; draw a sprite bar (value 0..255 -> bar graphics)
; params:
;  paramA,paramB -> address of sprite storage for UI Sprite 1
;  paramC        -> value 0..255
;  paramD,paramE -> table for values 0..4
drawBar:
    lda paramC
    cmp #35
    bcc _noClamp
    lda #35
_noClamp:
    sta paramC
    ldx #0          ; 9 bytes to write (twice for double thickness)
_barByteLoop:
    lda paramC
    cmp #4
    bcs _fullSegment
    ; partial segment
    tay
    lda (paramD),y
    jmp _storeSegment
_fullSegment:
    ldy #4
    lda (paramD),y
_storeSegment:
    ldy #0
    sta (paramA),y
    ldy #3
    sta (paramA),y
    
    ; now reduce total by 4, clamping to 0
    lda paramC
    sec
    sbc #4
    bpl _noClampBar
    lda #0
_noClampBar:
    sta paramC
    
    ; update the store offset
    lda barOffsets,x
    clc
    adc paramA
    sta paramA
    
    inx
    cpx #9
    bne _barByteLoop
    rts
    
refreshAmmo:
    lda playerAmmo
    cmp #MAX_BAR_VALUE
    beq _noRefreshAmmo
    lda varCycle
    and #$1f
    bne _noRefreshAmmo
    inc playerAmmo
_noRefreshAmmo:
    rts

refreshHitpoints:
    lda playerHitpoints
    cmp #MAX_BAR_VALUE
    beq _noRefreshHitpoints
    lda varCycle
    and #$3f
    bne _noRefreshHitpoints
    inc playerHitpoints
_noRefreshHitpoints:
    rts

; add ammo amount - amount in A
addAmmo:
    clc
    adc playerAmmo
    cmp #MAX_BAR_VALUE
    bcc _doneAddAmmo
    lda #MAX_BAR_VALUE
_doneAddAmmo:
    rts

addHitpoints:
    clc
    adc playerhitpoints
    cmp #MAX_BAR_VALUE
    bcc _doneAddHitpoints
    lda #MAX_BAR_VALUE
_doneAddHitpoints:
    rts

subtractHitpoints:
    lda #10
    sta hpDamageFlash
    lda playerHitpoints
    sec
    sbc paramA    
    bcs _noUnderflow
    lda #0
_noUnderflow:
    sta playerHitpoints
    rts

randomTileTable:
    dc.b 17,4, 0,11, 23,1, 18,18, 8,2, 20,4, 16,24, 27,8, 34,23, 21,6, 31,17, 24,5, 28,15, 36,4, 4,6, 28,10, 1,1, 20,2, 33,14, 17,2
    dc.b 33,18, 34,17, 29,19, 16,8, 31,13, 35,11, 5,4, 7,17, 11,15, 30,17, 21,12, 19,7, 22,11, 24,8, 24,7, 7,15, 5,1, 8,12, 31,9, 7,5
    dc.b 11,8, 3,0, 30,18, 19,3, 25,15, 23,22, 4,9, 25,19, 10,18, 11,18, 14,17, 9,5, 10,0, 37,9, 20,24, 25,5, 39,1, 21,14, 14,10, 15,3
    dc.b 5,22, 36,1, 37,16, 30,19, 36,18, 13,19, 32,21, 19,23, 6,7, 21,18, 5,21, 12,12, 13,11, 25,22, 18,15, 23,11, 37,6, 17,18, 9,3, 26,14
    dc.b 24,17, 29,12, 28,14, 16,13, 3,24, 29,23, 34,10, 33,0, 22,18, 18,6, 7,2, 26,0, 8,21, 18,24, 33,1, 1,15, 34,19, 20,6, 14,23, 4,2
    dc.b 0,2, 9,17, 10,14, 38,10, 23,4, 22,8, 9,10, 3,18, 27,21, 37,18, 17,9, 26,7, 12,14, 24,4, 31,15, 6,12, 14,1, 1,23, 37,14, 19,15
    dc.b 25,6, 15,19, 7,23, 15,9, 24,1, 11,24, 1,8, 9,0, 7,9, 29,22, 4,7, 31,14, 17,8, 2,4, 26,10, 7,13, 8,10, 5,0, 2,17, 0,24
    dc.b 18,13, 38,14, 12,8, 7,6, 34,8, 4,23, 35,7, 2,20, 24,14, 9,18, 1,21, 28,12, 33,6, 10,10, 23,23, 30,15, 18,4, 13,22, 37,22, 18,2
    dc.b 14,7, 11,6, 16,17, 0,1, 14,16, 11,5, 13,9, 4,3, 29,15, 36,20, 18,23, 8,3, 1,6, 29,13, 6,9, 15,6, 5,24, 22,6, 31,8, 20,16
    dc.b 34,11, 3,23, 22,1, 2,3, 11,7, 16,11, 1,14, 19,4, 9,20, 36,24, 27,19, 4,12, 28,11, 23,19, 14,21, 27,0, 8,6, 24,6, 32,0, 28,18
    dc.b 24,24, 0,14, 11,14, 35,6, 7,20, 15,11, 22,21, 15,10, 29,9, 33,9, 35,23, 5,18, 39,9, 33,8, 10,23, 32,14, 23,13, 29,21, 24,20, 19,20
    dc.b 18,14, 21,19, 19,9, 37,12, 3,6, 18,3, 33,5, 24,2, 10,5, 15,24, 9,8, 12,0, 30,16, 28,20, 23,15, 14,5, 0,19, 14,20, 19,6, 12,1
    dc.b 19,21, 33,19, 18,19, 24,9, 30,12, 29,7, 28,5, 36,22, 3,16, 17,22, 19,18, 25,10, 10,11, 25,24, 12,9, 19,8, 7,19, 8,1, 6,11, 30,10
    dc.b 35,24, 14,9, 31,4, 5,23, 12,11, 9,16, 37,23, 22,0, 3,22, 12,2, 23,6, 22,19, 20,23, 10,13, 14,2, 17,11, 5,10, 13,0, 37,5, 7,10
    dc.b 33,16, 4,24, 28,23, 16,6, 25,1, 29,3, 26,22, 11,16, 23,14, 34,1, 36,21, 16,7, 18,16, 16,19, 32,10, 0,12, 15,20, 6,6, 38,15, 33,4
    dc.b 6,1, 21,10, 20,9, 15,1, 20,3, 8,23, 8,16, 30,13, 5,17, 33,10, 28,21, 38,17, 15,13, 17,17, 32,1, 24,12, 27,9, 26,21, 21,21, 21,0
    dc.b 33,11, 35,22, 4,4, 12,10, 23,8, 39,3, 5,20, 2,10, 3,5, 22,20, 9,9, 28,2, 11,9, 24,0, 2,8, 10,8, 32,22, 9,19, 35,15, 2,13
    dc.b 36,16, 30,24, 17,20, 4,11, 20,5, 29,14, 29,24, 0,8, 2,5, 10,9, 22,4, 22,15, 26,2, 5,11, 9,11, 34,18, 21,16, 7,24, 38,2, 35,1
    dc.b 2,7, 26,4, 31,6, 11,1, 26,6, 22,5, 38,21, 10,12, 18,0, 35,2, 23,3, 7,21, 38,22, 38,13, 24,23, 0,18, 12,6, 22,7, 34,5, 36,2
    dc.b 36,10, 0,9, 24,15, 37,8, 25,7, 33,12, 22,10, 0,17, 27,20, 13,21, 11,12, 12,4, 25,3, 5,5, 12,3, 17,6, 3,12, 13,10, 12,22, 30,4
    dc.b 26,23, 10,6, 29,17, 7,14, 27,2, 3,19, 27,10, 15,8, 32,20, 23,9, 8,22, 36,12, 8,18, 36,23, 12,16, 39,5, 1,3, 2,21, 7,4, 14,11
    dc.b 19,0, 10,20, 32,11, 16,0, 27,17, 26,16, 9,12, 11,22, 20,1, 17,19, 35,18, 11,4, 6,15, 29,20, 32,7, 35,16, 33,20, 34,15, 22,24, 2,24
    dc.b 16,22, 34,13, 3,9, 39,19, 3,1, 38,23, 38,4, 22,22, 9,14, 36,19, 9,13, 39,21, 13,20, 35,3, 34,7, 28,19, 10,15, 24,13, 15,21, 27,4
    dc.b 25,8, 31,23, 5,19, 37,2, 5,16, 35,8, 2,9, 26,20, 3,2, 13,8, 17,15, 26,8, 14,19, 7,11, 19,11, 5,8, 32,18, 7,1, 16,9, 18,22  
    dc.b 39,24, 21,13, 16,14, 36,14, 6,24, 17,12, 4,5, 16,23, 4,21, 2,11, 36,6, 31,0, 16,20, 32,24, 15,15, 32,4, 12,23, 31,16, 2,16, 1,13
    dc.b 11,2, 9,7, 39,14, 14,15, 39,22, 5,13, 8,0, 23,12, 34,14, 31,12, 8,24, 2,2, 20,17, 5,3, 18,1, 17,1, 10,21, 20,19, 1,24, 21,7
    dc.b 10,22, 17,23, 4,18, 33,21, 0,20, 1,18, 19,19, 20,14, 15,14, 36,11, 3,7, 32,17, 27,11, 7,16, 16,18, 25,20, 30,21, 27,24, 2,0, 12,7
    dc.b 38,19, 8,19, 23,7, 6,17, 13,13, 36,7, 15,0, 4,22, 13,12, 36,15, 28,1, 3,15, 34,12, 21,22, 4,20, 6,0, 25,0, 16,1, 13,23, 20,15
    dc.b 6,3, 19,14, 17,24, 26,5, 10,17, 19,2, 11,21, 20,11, 3,11, 27,13, 11,23, 5,14, 25,4, 27,22, 35,14, 24,11, 13,14, 20,21, 18,8, 10,24
    dc.b 9,24, 27,12, 17,7, 7,3, 21,4, 24,18, 27,14, 30,23, 32,19, 12,21, 23,17, 10,4, 21,5, 31,2, 16,15, 0,6, 30,5, 34,9, 30,7, 31,21
    dc.b 35,12, 11,10, 0,4, 30,0, 3,4, 14,8, 35,10, 0,5, 16,4, 30,2, 29,6, 7,18, 4,14, 17,16, 1,4, 27,1, 23,5, 39,20, 1,0, 36,9
    dc.b 0,21, 30,22, 11,13, 19,24, 17,14, 34,22, 33,2, 16,3, 37,4, 37,19, 4,13, 14,13, 4,0, 27,3, 38,12, 33,24, 18,10, 27,18, 19,10, 11,19
    dc.b 23,10, 30,8, 39,10, 21,1, 1,11, 27,23, 35,20, 6,4, 27,5, 38,8, 37,7, 35,4, 8,9, 28,9, 21,2, 32,16, 29,5, 39,6, 38,11, 10,3
    dc.b 26,15, 28,7, 6,23, 12,15, 8,5, 16,5, 3,8, 38,5, 12,5, 37,1, 14,24, 9,21, 20,22, 34,0, 8,8, 37,20, 6,21, 33,22, 37,3, 22,16
    dc.b 18,12, 15,12, 27,16, 10,19, 15,7, 5,2, 34,20, 6,22, 13,15, 25,16, 28,4, 26,24, 3,13, 2,23, 1,22, 26,19, 26,1, 29,11, 20,12, 36,17
    dc.b 6,18, 18,11, 12,20, 3,17, 2,22, 12,19, 29,1, 31,18, 34,6, 29,10, 30,1, 13,7, 26,3, 4,8, 9,22, 39,12, 28,24, 21,24, 37,15, 1,20
    dc.b 39,23, 31,3, 34,4, 30,6, 13,2, 39,16, 3,14, 32,8, 19,22, 1,19, 2,19, 32,23, 0,3, 29,18, 10,2, 7,22, 13,24, 2,1, 8,15, 31,7
    dc.b 37,24, 29,8, 25,21, 28,8, 20,20, 0,15, 1,2, 13,4, 18,5, 18,20, 31,22, 15,22, 34,16, 30,9, 33,13, 14,22, 37,13, 34,3, 13,16, 4,17
    dc.b 38,0, 2,14, 34,24, 2,15, 1,7, 14,4, 24,22, 26,18, 21,11, 13,18, 15,16, 10,7, 23,24, 15,2, 11,11, 8,17, 0,13, 34,2, 4,1, 20,10
    dc.b 36,0, 8,14, 8,20, 20,8, 21,3, 22,9, 24,3, 37,17, 24,16, 19,5, 23,2, 26,17, 18,17, 6,16, 5,6, 33,15, 1,12, 35,17, 29,16, 16,10
    dc.b 27,7, 39,7, 25,13, 24,10, 14,3, 4,16, 2,12, 12,24, 26,9, 6,19, 9,23, 17,3, 38,7, 6,14, 26,13, 6,2, 30,11, 25,2, 35,19, 23,16
    dc.b 18,9, 37,11, 28,16, 21,23, 27,6, 16,21, 19,13, 18,7, 15,17, 11,20, 25,17, 30,14, 38,1, 0,22, 3,20, 8,4, 12,13, 39,2, 21,8, 9,6
    dc.b 18,21, 0,10, 31,11, 5,12, 29,0, 28,17, 25,23, 36,3, 5,15, 33,7, 35,0, 25,12, 11,17, 31,1, 32,12, 28,13, 15,18, 29,4, 6,10, 1,9
    dc.b 11,3, 26,12, 21,20, 38,9, 22,12, 20,7, 39,4, 31,5, 13,17, 32,15, 9,2, 19,16, 4,15, 12,17, 17,0, 22,3, 30,20, 14,12, 16,2, 35,13
    dc.b 32,5, 36,8, 34,21, 5,9, 1,17, 36,13, 30,3, 3,10, 1,16, 9,15, 0,16, 39,17, 39,8, 39,11, 19,12, 13,1, 14,0, 28,6, 8,7, 38,3
    dc.b 39,18, 39,0, 28,3, 23,18, 7,0, 8,11, 0,0, 31,10, 25,18, 6,8, 38,20, 32,13, 3,3, 11,0, 0,7, 15,23, 6,13, 14,14, 31,20, 35,5
    dc.b 16,12, 14,6, 39,15, 22,23, 21,17, 31,24, 1,10, 38,16, 6,20, 28,0, 22,17, 13,5, 22,13, 13,6, 7,12, 7,8, 0,23, 33,23, 38,18, 4,10
    dc.b 26,11, 9,1, 33,3, 24,21, 39,13, 35,9, 2,18, 37,10, 10,1, 22,14, 37,0, 32,3, 19,1, 33,17, 5,7, 20,18, 19,17, 29,2, 25,14, 32,6
    dc.b 28,22, 17,10, 17,13, 13,3, 10,16, 9,4, 2,6, 22,2, 27,15, 15,5, 23,20, 31,19, 37,21, 15,4, 36,5, 7,7, 21,15, 23,0, 8,13, 20,13
    dc.b 23,21, 32,2, 1,5, 3,21, 21,9, 14,18, 16,16, 17,5, 17,21, 12,18, 24,19, 20,0, 6,5, 38,6, 35,21, 25,11, 4,19, 32,9, 25,9, 38,24
randomTileTableEnd:

