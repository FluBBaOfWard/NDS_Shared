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
	/** EZ-FLASH III */
	EZ3_RAM,
	/** EZ-FLASH 3In1 */
	EZ3IN1_RAM,
	/** EZ-FLASH Omega */
	EZO_RAM,
	/** Super Card */
	SC_RAM,
	/** GBA Movie Player M3 */
	M3_RAM,
	/** Nintendo Memory Expansion Pack for Opera browser */
	OPERA_RAM,
	/** G6 Flash */
	G6_RAM,
	/** EverDrive GBA */
	ED_RAM,
} RAM_TYPE;

/**
 * Call this before any other functions.
 * When called with RAM_TYPE "DETECT_RAM" it will try to figure out which type
 * of expansion is in SLOT-2. It will also try to figure out how much RAM there is.
 * @param The type of RAM device you want to initialize, use "DETECT_RAM" to
 * let the code figure it out.
 * @return The type of RAM in SLOT-2, returns DETECT_RAM if none was found.
 */
RAM_TYPE cartRamInit(RAM_TYPE);

/**
 * Returns the type of the RAM device as figured out from running cartRamInit.
 * @return The type of the RAM device.
 */
RAM_TYPE cartRamType();

/** Returns the type of the RAM device in a string */
const char *cartRamName();

/** Returns the total amount of RAM in bytes */
u32 cartRamSize();

/**
 * Unlocks the RAM and returns a pointer to the beginning of the RAM.
 * @return A pointer to the RAM or NULL if there is no RAM.
 */
vu16 *cartRamUnlock();

/** Tries to lock the RAM, this may not work on all kinds of carts. */
void cartRamLock();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CARTRIDGE_RAM
