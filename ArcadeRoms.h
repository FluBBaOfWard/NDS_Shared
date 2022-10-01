#ifndef ARCADE_ROMS_HEADER
#define ARCADE_ROMS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define AC_GAME(shortName,gameName,gameRoms)  {(shortName),(gameName),(sizeof((gameRoms))/sizeof((gameRoms)[0])),gameRoms},

#define FILL0X00 "FILL0X00"
#define FILL0XFF "FILL0XFF"

typedef struct {				// (ArcadeRom)
	const char *const romName;	// Rom name.
	const int romSize;			// Rom size in bytes
	const int romCRC;			// Rom CRC32
} ArcadeRom;

typedef struct {				// (ArcadeGame)
	const char *const gameName;	// Short name (archive name)
	const char *const fullName;	// Full name
	const int romCount;
	const ArcadeRom *const roms;	// Pointer to array of ArcadeRoms
} ArcadeGame;

bool loadACRoms(u8 *dest, const ArcadeGame *gameList, int gameNr, int gameCount, bool doLoad);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ARCADE_ROMS_HEADER
