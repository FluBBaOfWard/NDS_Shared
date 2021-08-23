#ifndef ARCADE_ROMS_HEADER
#define ARCADE_ROMS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define FILL0X00 "FILL0X00"
#define FILL0XFF "FILL0XFF"

typedef struct {				// (ArcadeRom)
	const char *romName;		// Rom name.
	const int romSize;			// Rom size in bytes
	const int romCRC;			// Rom CRC32
} ArcadeRom;

typedef struct {				// (ArcadeGame)
	const char *gameName;		// short name (archive name)
	const char *fullName;		// Full name
	const int romCount;
	const ArcadeRom *roms;		// Pointer to array of ArcadeRoms
} ArcadeGame;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ARCADE_ROMS_HEADER
