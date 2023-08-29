//
//  CartridgeRAM.h
//  Slot-2 RAM expansion support for GBA/NDS.
//
//  Created by Fredrik Ahlström on 2023-08-27.
//  Copyright © 2023 Fredrik Ahlström. All rights reserved.
//
// Credits: Amadeus, Chishm, Cory1492, Lazy1, Pepsiman, Viruseb, Rick "Lick" Wong

#ifndef CARTRIDGE_RAM
#define CARTRIDGE_RAM

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DETECT_RAM=0,
	EZ3_RAM,
	EZO_RAM,
	SC_RAM,
	M3_RAM,
	OPERA_RAM,
	G6_RAM,
} RAM_TYPE;

/** Call this before the others */
RAM_TYPE ramInit(RAM_TYPE);

/** Returns the type of the RAM device */
RAM_TYPE ramType();

/** Returns the type of the RAM device in a string */
const char *ramTypeString();

/** Returns the total amount of RAM in bytes */
u32 ramSize();

/** Unlocks the RAM and returns a pointer to the begining */
vu16 *ramUnlock();

/** Locks the RAM */
void ramLock();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CARTRIDGE_RAM
