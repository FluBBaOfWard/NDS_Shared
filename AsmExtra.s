#ifdef __arm__

#include "nds_asm.h"

	.global getTime
	.global bin2BCD
	.global getRandomNumber
	.global setupSpriteScaling
	.global calculateFPS
	.global setLCDFPS
	.global fpsValue
	.global fpsText
	.global fpsNominal
	.global convertPalette
	.global debugOutputToEmulator
	.global r0OutputToEmulator
	.global debugOutput_asm
	.global enableSlot2Cache
	.global disableSlot2Cache
	.global bytecopy_
	.global memclr_
	.global memset_
	.global memorr_

	.syntax unified
	.arm

	.section .text
	.align 2
;@----------------------------------------------------------------------------
getTime:					;@ Out r0 = ??ssMMHH, r1 = ??DDMMYY
	.type getTime STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}
	bl updateTime
	ldrb r2,[r0],#1
	ldrb r3,[r0],#1
	orr r2,r2,r3,lsl#8
	ldrb r3,[r0],#1
	orr r2,r2,r3,lsl#16

	ldrb r1,[r0],#1
	ldrb r3,[r0],#1
	orr r1,r1,r3,lsl#8
	ldrb r3,[r0],#1
	orr r1,r1,r3,lsl#16
	mov r0,r2

	ldmfd sp!,{lr}
	bx lr
;@----------------------------------------------------------------------------
bin2BCD:		;@ Transform value to BCD
	.type bin2BCD STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4}

	mov r1,#0
	adr r3,bcdTable				;@ BCD
	ldr r12,=0x11111110			;@ Carry count mask value

bin2BCDLoop:
	ldr r2,[r3],#4				;@ BCD (read BCD table)
	movs r0,r0,lsr#1			;@ Test bit

	bcc bin2BCDLoopCheck
bin2BCDCalc:
	add r4,r2,r1				;@ r4 = r2 + r1
	eor r2,r1,r2				;@ r2 = r1 XOR r2
	eor r2,r4,r2				;@ r2 = r4 XOR r2
	bic r2,r12,r2				;@ r2 = 0x11111110 AND NOT r2
	orr r2,r2,r2,lsr#1			;@ r2 = r2 OR (r2 LSR 1)
	sub r1,r4,r2,lsr#2			;@ r1 = r4 -  (r2 LSR 2)

bin2BCDLoopCheck:
	bne bin2BCDLoop
	mov r0,r1

	ldmfd sp!,{r4}
	bx lr

bcdTable:
	.long 0x06666667
	.long 0x06666668
	.long 0x0666666a
	.long 0x0666666e
	.long 0x0666667c
	.long 0x06666698
	.long 0x066666ca
	.long 0x0666678e
	.long 0x066668bc
	.long 0x06666b78
	.long 0x0666768a
	.long 0x066686ae
	.long 0x0666a6fc
	.long 0x0666e7f8
	.long 0x0667c9ea
	.long 0x06698dce
	.long 0x066cbb9c
	.long 0x067976d8
	.long 0x068c87aa
	.long 0x06b8a8ee
	.long 0x076aebdc
	.long 0x086fd7b8
	.long 0x0a7fa96a
	.long 0x0e9eec6e
	.long 0x1cddd87c
	.long 0x39bbaa98
	.long 0x6d76eeca

;@----------------------------------------------------------------------------
getRandomNumber:			;@ r0 = Max value
	.type getRandomNumber STT_FUNC
;@----------------------------------------------------------------------------
	ldrd r2,r3,rndSeed0
	tst r3,r3,lsr#1				;@ Top bit into Carry
	movs r1,r2,rrx				;@ 33 bit rotate right
	adc r3,r3,r3				;@ Carry into lsb of r1
	eor r1,r1,r2,lsl#12			;@ (involved!)
	eor r2,r1,r1,lsr#20			;@ (similarly involved!)

	strd r2,r3,rndSeed0
	umull r3,r0,r2,r0
	bx lr
rndSeed0:
	.long 0x39bbaa98
rndSeed1:
	.long 0x00000000

;@----------------------------------------------------------------------------
setupSpriteScaling:			;@ r0=Scaling parameters.
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r6}
	ldmia r0,{r1-r6}			;@ Get sprite scaling params
	add r0,r1,#6

	mov r12,#2
scaleLoop:
	strh r3,[r0],#8				;@ Buffer1, buffer2.
	strh r2,[r0],#8
	strh r2,[r0],#8
	strh r5,[r0],#232
		strh r4,[r0],#8
		strh r2,[r0],#8
		strh r2,[r0],#8
		strh r5,[r0],#232
			strh r3,[r0],#8
			strh r2,[r0],#8
			strh r2,[r0],#8
			strh r6,[r0],#232
		strh r4,[r0],#8
		strh r2,[r0],#8
		strh r2,[r0],#8
		strh r6,[r0],#232
	subs r12,r12,#1
	bne scaleLoop

	ldmfd sp!,{r4-r6}
	mov r0,#OAM
	mov r2,#0x400
	b memcpy
;@----------------------------------------------------------------------------
calculateFPS:					;@ fps output, r0-r3=used.
	.type calculateFPS STT_FUNC
;@----------------------------------------------------------------------------
	ldrb r0,fpsCheck
	subs r0,r0,#1
	ldrbmi r0,fpsNominal
	strb r0,fpsCheck
	bxpl lr						;@ End if not 60 frames has passed

	ldr r0,fpsValue
	mov r1,#0
	str r1,fpsValue

	mov r1,#100
	swi 0x090000				;@ Division r0/r1, r0=result, r1=remainder.
	cmp r0,#0
	addeq r0,r0,#0x20
	addne r0,r0,#0x30
	strb r0,fpsText+4
	mov r0,r1
	mov r1,#10
	swi 0x090000				;@ Division r0/r1, r0=result, r1=remainder.
	add r1,r1,#0x30
	strb r1,fpsText+6
	ldrb r1,fpsText+4
	cmp r0,#0
	cmpeq r1,#0
	addeq r0,r0,#0x20
	addne r0,r0,#0x30
	strb r0,fpsText+5

	bx lr
;@----------------------------------------------------------------------------
setLCDFPS:					;@ Write LCD FPS, r0=in fps, r0-r1=used.
	.type setLCDFPS STT_FUNC
;@----------------------------------------------------------------------------
	mov r1,#10
	swi 0x090000				;@ Division r0/r1, r0=result, r1=remainder.
	add r0,r0,#0x30
	strb r0,fpsText+8
	add r1,r1,#0x30
	strb r1,fpsText+9
	bx lr
;@----------------------------------------------------------------------------
fpsValue:	.long 0
fpsText:	.string "FPS:   /60"
fpsCheck:	.byte 0
fpsNominal:	.byte 59
	.align 2

;@----------------------------------------------------------------------------
convertPalette:				;@ r0 = destination, r1 = source, r2 = length. r3 = gamma (0 -> 4), r12 modified.
	.type convertPalette STT_FUNC
;@ called by main.c:  void convertPalette(u16 *destination, const u8 *source, int length, int gamma);
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r6,lr}
convLoop:						;@ Map rrrrrrrrggggggggbbbbbbbb  ->  0bbbbbgggggrrrrr
	ldrb r4,[r1],#1				;@ Red
	bl gammaConvert
	mov r6,r4

	ldrb r4,[r1],#1				;@ Green
	bl gammaConvert
	orr r6,r6,r4,lsl#5

	ldrb r4,[r1],#1				;@ Blue
	bl gammaConvert
	orr r6,r6,r4,lsl#10

	strh r6,[r0],#2
	subs r2,r2,#1
	bhi convLoop

	ldmfd sp!,{r4-r6,lr}
	bx lr
;@----------------------------------------------------------------------------
gammaConvert:	;@ Takes value in r4(0-0xFF), gamma in r3(0-4),returns new value in r4=0x1F
;@----------------------------------------------------------------------------
	rsb r5,r4,#0x100
	mul r12,r5,r5
	rsbs r5,r12,#0x10000
	rsb r12,r3,#4
	orr r4,r4,r4,lsl#8
	mul r5,r3,r5
	mla r4,r12,r4,r5
	mov r4,r4,lsr#13

	bx lr
;@----------------------------------------------------------------------------
debugOutputToEmulator:		;@ r0 ptr to string
	.type debugOutputToEmulator STT_FUNC
;@----------------------------------------------------------------------------
	mov r12,#80					;@ Max length.
	adr r2,debugString
debugLoop:
	ldrb r1,[r0],#1
	strb r1,[r2],#1
	cmp r1,#0
	subsne r12,r12,#1
	bne debugLoop
	mov r1,#0xA					;@ New line
	strb r1,[r2,#-1]
	mov r1,#0					;@ END
	strb r1,[r2]
dStringEnd:
	mov r12,r12
	b debugContinue
	.short 0x6464,0x0000
debugString:
	.space 84
debugContinue:
	bx lr
;@----------------------------------------------------------------------------
r0OutputToEmulator:			;@ r0 = value
	.type r0OutputToEmulator STT_FUNC
;@----------------------------------------------------------------------------
	mov r12,r12
	b r0Continue
	.short 0x6464,0x0000
	.string "r0=%r0%\n"
	.align 2
r0Continue:
	bx lr
;@----------------------------------------------------------------------------
debugOutput_asm:			;@ Input = r1. ptr to str.
;@----------------------------------------------------------------------------
	stmfd sp!,{r0-r3,lr}
	ldr r0,=gDebugSet
	ldrb r0,[r0]
	cmp r0,#0
	ldmfdeq sp!,{r0-r3,pc}
	mov r0,r1
	blx debugOutput
	ldmfd sp!,{r0-r3,pc}

;@----------------------------------------------------------------------------
enableSlot2Cache:
	.type enableSlot2Cache STT_FUNC
;@----------------------------------------------------------------------------
	// Enable Data Cache for region 3 (SLOT-2, probably specific to DevkitARM).
	mrc	p15,0,r0,c2,c0,0
	orr r0,r0,#(1 << 3)
	mcr	p15,0,r0,c2,c0,0
	bx lr
;@----------------------------------------------------------------------------
disableSlot2Cache:
	.type disableSlot2Cache STT_FUNC
;@----------------------------------------------------------------------------
	// Disable Data Cache for region 3 (SLOT-2, probably specific to DevkitARM).
	mrc	p15,0,r0,c2,c0,0
	bic r0,r0,#(1 << 3)
	mcr	p15,0,r0,c2,c0,0
	bx lr
;@----------------------------------------------------------------------------
bytecopy_:					;@ void bytecopy(u8 *dst, u8 *src, int count)
;@----------------------------------------------------------------------------
	subs r2,r2,#1
	ldrbpl r3,[r1,r2]
	strbpl r3,[r0,r2]
	bhi bytecopy_
	bx lr
;@----------------------------------------------------------------------------

	.section .itcm
	.align 2
;@----------------------------------------------------------------------------
memclr_:					;@ r0=Dest r1=word count
;@	exit with r0 unchanged, r2=r1, r1=0
;@----------------------------------------------------------------------------
	mov r2,r1
	mov r1,#0
;@----------------------------------------------------------------------------
memset_:					;@ r0=Dest r1=data r2=word count
;@	exit with r0 & r1 unchanged, r2=0
;@----------------------------------------------------------------------------
	subs r2,r2,#1
	strpl r1,[r0,r2,lsl#2]
	bhi memset_
	bx lr
;@----------------------------------------------------------------------------
memorr_:					;@ r0=Dest r1=data r2=word count
;@	exit with r0 & r1 unchanged, r2=0, r3 trashed
;@----------------------------------------------------------------------------
	subs r2,r2,#1
	ldrpl r3,[r0,r2,lsl#2]
	orrpl r3,r3,r1
	strpl r3,[r0,r2,lsl#2]
	bhi memorr_
	bx lr
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
