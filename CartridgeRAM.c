//
//  CartridgeRAM.c
//  Slot-2 RAM expansion support for GBA/NDS.
//
//  Created by Fredrik Ahlström on 2023-08-27.
//  Copyright © 2023 Fredrik Ahlström. All rights reserved.
//
// Credits: Rick "Lick" Wong, Amadeus, Chishm, Cory1492, Lazy1, Pepsiman,
// Viruseb, Damian Yerrick

#include "nds.h"
#include "CartridgeRAM.h"
#include "ASMExtra.h"

static void scSetMode(int mode) {
	*(vu16 *)0x9FFFFFE = 0xA55A;
	*(vu16 *)0x9FFFFFE = 0xA55A;
	*(vu16 *)0x9FFFFFE = mode;
	*(vu16 *)0x9FFFFFE = mode;
}

static vu16 *scUnlock() {
	scSetMode(0x5); // RAM_RW
	return (vu16 *)0x8000000;
}

static void scLock() {
	scSetMode(0x3); // MEDIA
}

//==========================================================

static void m3SetMode(u16 mode) {
	vu16 tmp;
	tmp = *(vu16 *)0x8E00002;
	tmp = *(vu16 *)0x800000E;
	tmp = *(vu16 *)0x8801FFC;
	tmp = *(vu16 *)0x800104A;
	tmp = *(vu16 *)0x8800612;
	tmp = *(vu16 *)0x8000000;
	tmp = *(vu16 *)0x8801B66;
	tmp = *(vu16 *)(0x8800000 + (mode << 1));
	tmp = *(vu16 *)0x800080E;
	tmp = *(vu16 *)0x8000000;
	tmp = *(vu16 *)0x80001E4;
	tmp = *(vu16 *)0x80001E4;
	tmp = *(vu16 *)0x8000188;
	tmp = *(vu16 *)0x8000188;
}

static vu16 *m3Unlock() {
	m3SetMode(6); // RAM_RW
	return (vu16 *)0x8000000;
}

static void m3Lock() {
	m3SetMode(3); // MEDIA
}

//==========================================================

static vu16 *operaUnlock() {
	*(vu16 *)0x8240000 = 1;
	return (vu16 *)0x9000000;
}

static void operaLock() {
	*(vu16 *)0x8240000 = 0;
}

//==========================================================

static void g6SetMode(u16 mode) {
	vu16 tmp;
	tmp = *(vu16 *)0x9000000;
	tmp = *(vu16 *)0x9FFFFE0;
	tmp = *(vu16 *)0x9FFFFEC;
	tmp = *(vu16 *)0x9FFFFEC;
	tmp = *(vu16 *)0x9FFFFEC;
	tmp = *(vu16 *)0x9FFFFFC;
	tmp = *(vu16 *)0x9FFFFFC;
	tmp = *(vu16 *)0x9FFFFFC;
	tmp = *(vu16 *)0x9FFFF4A;
	tmp = *(vu16 *)0x9FFFF4A;
	tmp = *(vu16 *)0x9FFFF4A;
	tmp = *(vu16 *)(0x9200000 + (mode << 1));
	tmp = *(vu16 *)0x9FFFFF0;
	tmp = *(vu16 *)0x9FFFFE8;
}

static vu16 *g6Unlock() {
	g6SetMode(6); // RAM_RW
	return (vu16 *)0x8000000;
}

static void g6Lock() {
	g6SetMode(3); // MEDIA
}

//==========================================================

static void ezCommand(u32 adr, u16 value) {
	*(vu16 *)0x9FE0000 = 0xD200;
	*(vu16 *)0x8000000 = 0x1500;
	*(vu16 *)0x8020000 = 0xD200;
	*(vu16 *)0x8040000 = 0x1500;
	*(vu16 *)adr = value;
	*(vu16 *)0x9FC0000 = 0x1500;
}

/**
 * Set a rom offset to the start address of cartridge space that is 0x8000000.
 * 1 page is 1Mbit here. For example if you want set mode2 256M NORFLASH 32Mb as the first start. The page is 32.
 * Bit 15 set OS mode (0 = game mode).
 * Bit 13, 12, which 64Mb bank is @ 0x9400000-0x9C00000 in OS mode.
 */
static void ezSetRomPage(u16 page) {
	ezCommand(0x9880000, page);
}

//ezSetRomPage(0x0160);       // SetRompage (0x160)

/**
 * Flash/NOR/PSRAM Write Open/Close
 * Open = 0x1500 (0xA500?, used on PCEAdvance... )
 * Close = 0xD200
 */
static void ezSetNorWrite(u16 mode) {
	ezCommand(0x9C40000, mode);
}

static vu16 *ez3Unlock() {
	ezSetNorWrite(0x1500); // Open Write
	ezSetRomPage(0x8000); // Set OS mode
	return (vu16 *)0x8400000;
}

static void ez3Lock() {
//	ezSetRomPage(0); // Set Game mode
	ezSetNorWrite(0xD200); // Close Write
}

static vu16 *ezoUnlock() {
	ez3Unlock();
	return (vu16 *)0x8800000;
}

static void ezoLock() {
	ez3Lock();
}

//==========================================================

typedef enum {
	SLOT2SPD_SLOW   = 0x00, // (4,2)
	SLOT2SPD_NORMAL = 0x14, // (3,1)
	SLOT2SPD_FAST   = 0x18, // (2,1)
	SLOT2SPD_MASK = 0x001F
} SLOT2SPD;

void setSlot2Speed(SLOT2SPD waitStates) {
  REG_EXMEMCNT = (REG_EXMEMCNT & ~SLOT2SPD_MASK)
				 | (waitStates & SLOT2SPD_MASK);
}

typedef struct {
	const RAM_TYPE type;
	vu16 *(*unlockFunc)();
	void (*lockFunc)();
	const char *name;
	SLOT2SPD speed;
} RamStruct;

static vu16 *(*unlockFunc)() = NULL;
static void (*lockFunc)() = NULL;
static u32 rSize = 0;
static RAM_TYPE rType = DETECT_RAM;

static const RamStruct ramStruct[] = {
	{DETECT_RAM, NULL, NULL, "Unknown", SLOT2SPD_SLOW},
	{EZ3_RAM, ez3Unlock, ez3Lock, "EZ3", SLOT2SPD_FAST},
	{EZO_RAM, ezoUnlock, ezoLock, "EZOmega", SLOT2SPD_FAST},
	{SC_RAM, scUnlock, scLock, "SuperCard", SLOT2SPD_SLOW},
	{M3_RAM, m3Unlock, m3Lock, "M3", SLOT2SPD_NORMAL},
	{OPERA_RAM, operaUnlock, operaLock, "Opera", SLOT2SPD_FAST},
	{G6_RAM, g6Unlock, g6Lock, "G6", SLOT2SPD_NORMAL},
};

/**
 * Tests that the first 16bit address can be written to after unlock and not be
 * written to after lock.
 * @return true if the first write succeded and the second failed.
 */
static bool ramTest() {
	vu16 *ram = unlockFunc();
	ram[0] = 0x1234;
	if (ram[0] == 0x1234) {     // Test writability
		lockFunc();
		ram[0] = 0x4321;
		if (ram[0] != 0x4321) { // Test non-writability
			return true;
		}
	}

	return false;
}

/**
 * Just tests that the first 16bit address can be written to after unlock.
 * @return true if a write succeded.
 */
static bool ramTestNoLock() {
	vu16 *ram = unlockFunc();
	ram[0] = 0x1234;
	if (ram[0] == 0x1234) {     // Test writability
		lockFunc();
		return true;
	}

	return false;
}

static void ramPrecalcSize() {
	if (unlockFunc == 0 || lockFunc == 0) {
		return;
	}
	vu16 *ram = unlockFunc();
	rSize = 0;

	ram[0] = 0x2468;
	while (ram[rSize] == 0x2468) {
		ram[rSize] = 0;
		rSize += 512;
		ram[rSize] = 0x2468;
	}
	rSize <<= 1;

	lockFunc();
}

static bool findRamType() {
	for (int i = 1; i < 7; i++) {
		unlockFunc = ramStruct[i].unlockFunc;
		lockFunc   = ramStruct[i].lockFunc;
		rType      = ramStruct[i].type;
		if ((rType == OPERA_RAM
			|| rType == EZ3_RAM
			|| rType == EZO_RAM)
			&& ramTestNoLock()) {
			return true;
		}
		else if (ramTest()) {
			return true;
		}
	}
	return false;
}

//==========================================================
RAM_TYPE cartRamInit(RAM_TYPE type) {
	sysSetCartOwner(BUS_OWNER_ARM9);

	switch(type) {
		case SC_RAM:
		case M3_RAM:
		case OPERA_RAM:
		case G6_RAM:
		case EZ3_RAM:
		case EZO_RAM:
			unlockFunc = ramStruct[(int)type].unlockFunc;
			lockFunc   = ramStruct[(int)type].lockFunc;
			rType      = type;
			break;
		case DETECT_RAM:
		default:
			if (findRamType()) {
				break;
			}
			// Fail
			unlockFunc = NULL;
			lockFunc   = NULL;
			rType      = DETECT_RAM;
			setSlot2Speed(ramStruct[rType].speed);
			return rType;
		break;
	}

	setSlot2Speed(ramStruct[rType].speed);
	ramPrecalcSize();

	return rType;
}

RAM_TYPE cartRamType() {
	return rType;
}

const char *cartRamName() {
	return ramStruct[(int)rType].name;
}

u32 cartRamSize() {
	return rSize;
}

vu16 *cartRamUnlock() {
	sysSetCartOwner(BUS_OWNER_ARM9);
	if (unlockFunc) {
		enableSlot2Cache();
		return unlockFunc();
	}
	return NULL;
}

void cartRamLock() {
	sysSetCartOwner(BUS_OWNER_ARM9);
	if (lockFunc) {
		lockFunc();
		disableSlot2Cache();
	}
}
