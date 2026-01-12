#include <nds.h>
#include <stdio.h>

#include "ArcadeRoms.h"
#include "EmuMenu.h"
#include "FileHelper.h"
#include "Unzip/unzipnds.h"

bool loadACRoms(u8 *dest, const ArcadeGame *gameList, int gameNr, int gameCount, bool doLoad) {
	int i, j;
	bool found;
	const ArcadeGame *game = &gameList[gameNr];
	char zipName[32];
	char zipSubName[32];
	FILE *file;

	const int romCount = game->romCount;
	strlMerge(zipName, game->gameName, ".zip", sizeof(zipName));

	chdir("/");			// Stupid workaround.
	if (chdir(currentDir) == -1) {
		return true;
	}

	for (i=0; i<romCount; i++) {
		found = false;
		drawSpinner();
		const char *romName = game->roms[i].romName;
		const int romSize = game->roms[i].romSize;
		const u32 romCRC = game->roms[i].romCRC;
		if (strcmp(romName, FILL0XFF) == 0) {
			if (doLoad) {
				memset(dest, 0xFF, romSize);
				dest += romSize;
			}
			continue;
		}
		if (strcmp(romName, FILL0X00) == 0) {
			if (doLoad) {
				memset(dest, 0x00, romSize);
				dest += romSize;
			}
			continue;
		}
		if (strcmp(romName, ROM_REGION) == 0) {
			if (doLoad) {
				*((u8 **)romCRC) = dest;
			}
			continue;
		}
		if ((file = fopen(romName, "r"))) {
			if (doLoad) {
				fread(dest, 1, romSize, file);
				dest += romSize;
			}
			fclose(file);
			found = true;
		}
		else if (!findFileWithCRC32InZip(zipName, romCRC)) {
			if (doLoad) {
				loadFileWithCRC32InZip(dest, zipName, romCRC, romSize);
				dest += romSize;
			}
			found = true;
		}
		else {
			for (j=0; j<gameCount; j++) {
				strlMerge(zipSubName, gameList[j].gameName, ".zip", sizeof(zipSubName));
				if (!findFileWithCRC32InZip(zipSubName, romCRC)) {
					if (doLoad) {
						loadFileWithCRC32InZip(dest, zipSubName, romCRC, romSize);
						dest += romSize;
					}
					found = true;
					break;
				}
			}
		}
		if (!found) {
			infoOutput("Couldn't open file:");
			infoOutput(romName);
			return true;
		}
	}
	return false;
}
