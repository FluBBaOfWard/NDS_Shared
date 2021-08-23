#ifndef EMUBASE_AC
#define EMUBASE_AC

#ifdef __cplusplus
extern "C" {
#endif

// This is the config struct for Arcade games, they share the same path for roms.
typedef struct {				//(config struct)
	char magic[4];				//="CFG",0
	int emuSettings;
	int sleepTime;				// autoSleepTime
	char currentPath[256];
	u8 scaling;					// From Gfx.s
	u8 flicker;					// From Gfx.s
	u8 gammaValue;				// From Gfx.s
	u8 gfxReserved[5];			// From Gfx.s
	u8 controller;				// From io.s
	u8 ioReserved[3];			// From io.s
	int coinCounter0;			// From io.s
	int coinCounter1;			// From io.s
	u8 dipSwitchBT0;			// From io.s Black Tiger
	u8 dipSwitchBT1;			// From io.s
	u8 dipSwitchBT2;			// From io.s
	u8 dipSwitchBT3;			// From io.s
	u8 dipSwitchDD0;			// From io.s Double Dribble
	u8 dipSwitchDD1;			// From io.s
	u8 dipSwitchDD2;			// From io.s
	u8 dipSwitchDD3;			// From io.s
	u8 dipSwitchFN0;			// From io.s, Finalizer
	u8 dipSwitchFN1;			// From io.s
	u8 dipSwitchFN2;			// From io.s
	u8 dipSwitchFN3;			// From io.s
	u8 dipSwitchGG0;			// From io.s GhostsN Goblins
	u8 dipSwitchGG1;			// From io.s
	u8 dipSwitchGG2;			// From io.s
	u8 dipSwitchGG3;			// From io.s
	u8 dipSwitchGB0;			// From io.s Green Beret
	u8 dipSwitchGB1;			// From io.s
	u8 dipSwitchGB2;			// From io.s
	u8 dipSwitchGB3;			// From io.s
	u8 dipSwitchIH0;			// From io.s Iron Horse
	u8 dipSwitchIH1;			// From io.s
	u8 dipSwitchIH2;			// From io.s
	u8 dipSwitchIH3;			// From io.s
	u8 dipSwitchJK0;			// From io.s Jackal
	u8 dipSwitchJK1;			// From io.s
	u8 dipSwitchJK2;			// From io.s
	u8 dipSwitchJK3;			// From io.s
	u8 dipSwitchJB0;			// From io.s Jail Break
	u8 dipSwitchJB1;			// From io.s
	u8 dipSwitchJB2;			// From io.s
	u8 dipSwitchJB3;			// From io.s
	u8 dipSwitchPO0;			// From io.s Punch Out
	u8 dipSwitchPO1;			// From io.s
	u8 dipSwitchPO2;			// From io.s
	u8 dipSwitchPO3;			// From io.s
	u8 dipSwitchRE0;			// From io.s Renegade
	u8 dipSwitchRE1;			// From io.s
	u8 dipSwitchRE2;			// From io.s
	u8 dipSwitchRE3;			// From io.s
	u8 dipSwitchSS0;			// From io.s Son Son
	u8 dipSwitchSS1;			// From io.s
	u8 dipSwitchSS2;			// From io.s
	u8 dipSwitchSS3;			// From io.s
	u8 dipSwitchYA0;			// From io.s Yie Ar Kung-Fu
	u8 dipSwitchYA1;			// From io.s
	u8 dipSwitchYA2;			// From io.s
	u8 dipSwitchYA3;			// From io.s
} ConfigData;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUBASE_AC
