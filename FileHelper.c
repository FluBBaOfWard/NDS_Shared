#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/dir.h>

#include "FileHelper.h"
#include "EmuMenu.h"
#include "Unzip/unzipnds.h"
#include "crc32.h"
#include "../Main.h"
#include "../FileHandling.h"

static const int directoryCacheSize = 0x8000;
static const int directoryMaxEntries = 768;

/**
 * Lists a directory, filtering according to file types.
 * @param  *dirTable: Buffer to fill.
 * @param  *dirName: Name of directory to scan.
 * @param  *fileTypes: All accepted filetypes, including dots, eg. ".txt.doc.bat".
 * @retval Number of entries in the directory.
 */
static int getDirectory(char *dirTable, const char *dirName, const char *fileTypes);

/**
 * Sort a directories table.
 * @param  **dirEntries: Buffer to sort.
 * @param  count: Number of entries to sort.
 */
static void directorySort(char **dirEntries, int count);
static void directoryBack(char *path);

int fatAvailable = 0;
static char *dTable = 0;
static char **entriesTable = 0;
/** Current number of entries in the directory table */
static int dItemCount = 0;
static int dCacheLeft = 0;
static unsigned int dirCRC32 = 0;
static char spinnerCount = 0;

char currentFilename[FILENAMEMAXLENGTH];
char currentDir[FILEPATHMAXLENGTH];

const char *const spinner[4]={"\\","|","/","-"};
//---------------------------------------------------------------------------------

int initFileHelper() {
	if ((fatAvailable = fatInitDefault())) {
		strcpy(currentDir, "/");
	}
	return fatAvailable;
}

void drawSpinner() {
	drawItem(spinner[spinnerCount&3], 15, 10, 0);
	spinnerCount++;
}

int loadROM(void *dest, const char *fileName, const int maxSize) {
	int size = 0;
	FILE *file;
	char fileExt[8];

	getFileExtension(fileExt, fileName);
	if (strstr(fileExt, ".zip")) {
		if (dest >= (void *)0x8000000 && dest < (void *)0xC000000) {
			infoOutput("Can not load zip to Exp-RAM.");
		}
		else if (loadFileTypeInZip(dest, fileName, FILEEXTENSIONS, maxSize) == 0) {
			size = cenHead.ucSize;
			strlcpy(currentFilename, zipFilename, sizeof(currentFilename));
		}
		else {
			infoOutput(zipError);
		}
	}
	else if ((file = fopen(fileName, "r"))) {
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		if (size > maxSize) {
			infoOutput("File too large!");
			size = 0;
		}
		else {
			fseek(file, 0, SEEK_SET);
			fread(dest, 1, size, file);
			strlcpy(currentFilename, fileName, sizeof(currentFilename));
		}
		fclose(file);
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(fileName);
	}
	return size;
}

bool loadDeviceState(const char *folderName) {
	bool err = true;
	FILE *file;
	u32 *statePtr;
	char stateName[FILENAMEMAXLENGTH];

	if (findFolder(folderName)) {
		return err;
	}
	setFileExtension(stateName, currentFilename, ".sta", sizeof(stateName));
	if ((file = fopen(stateName, "r"))) {
		int stateSize = getStateSize();
		if ((statePtr = malloc(stateSize))) {
			cls(0);
			drawText("        Loading state...", 11, 0);
			fread(statePtr, 1, stateSize, file);
			unpackState(statePtr);
			free(statePtr);
			err = false;
			infoOutput("Loaded state.");
		}
		else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
	else {
		infoOutput("Couldn't open state file:");
		infoOutput(stateName);
	}
	return err;
}

bool saveDeviceState(const char *folderName) {
	bool err = true;
	FILE *file;
	u32 *statePtr;
	char stateName[FILENAMEMAXLENGTH];

	if (findFolder(folderName)) {
		return err;
	}
	setFileExtension(stateName, currentFilename, ".sta", sizeof(stateName));
	if ((file = fopen(stateName, "w"))) {
		int stateSize = getStateSize();
		if ((statePtr = malloc(stateSize))) {
			cls(0);
			drawText("        Saving state...", 11, 0);
			packState(statePtr);
			fwrite(statePtr, 1, stateSize, file);
			free(statePtr);
			err = false;
			infoOutput("Saved state.");
		}
		else {
			infoOutput("Couldn't alloc mem for state.");
		}
		fclose(file);
	}
	else {
		infoOutput("Couldn't open state file:");
		infoOutput(stateName);
	}
	return err;
}

int findFolder(const char *folderName) {
	char tempString[FILEPATHMAXLENGTH];
	int retValue = 0;
	if (!chdir(folderName)) {
		return retValue;
	}
	chdir("/");
	strcpy(tempString, "data/");
	strlcat(tempString, folderName, sizeof(tempString));
	if (chdir(tempString)) {
		if (chdir(folderName)) {
			infoOutput("Couldn't find folder:");
			infoOutput(folderName);
			retValue = 1;
		}
	}
	return retValue;
}

void getFileExtension(char *dest, const char *fileName) {
	int i;
	const char *strExt;

	dest[0] = 0;
	if ((strExt = strrchr(fileName, '.')) == NULL) {
		return;
	}
	strlcpy(dest, strExt, 8);
	// Make file extension lower case
	for (i = 1; i < 6; i++) {
		if (dest[i]) {
			dest[i] |= 0x20;
		}
	}
}

void setFileExtension(char *dest, const char *fileName, const char *newExt, int dstSize) {
	char *strExt;

	if (dest != fileName) {
		strlcpy(dest, fileName, dstSize);
	}
	if ((strExt = strrchr(dest, '.')) != NULL) {
		strExt[0] = 0;
	}
	strlcat(dest, newExt, dstSize);
}

//---------------------------------------------------------------------------------
const char *browseForFileType(const char *fileTypes) {
	static int pos = 0;
	int row = 0, oldPos = -1;
	const char *strP;
	int pressed = 0;

	if (fatAvailable) {
		cls(0);
		if (dTable == 0) {
			dTable = malloc(directoryCacheSize);
		}
		dItemCount = getDirectory(dTable, currentDir, fileTypes);
		if (dItemCount == -1) {
			directoryBack(currentDir);
		}
		if (dItemCount > 0) {
			if (pos >= dItemCount) {
				pos = dItemCount-1;
			}
			while (1) {
				waitVBlank();
				pressed = getInput();
				pos = getMenuPos(pressed, pos, dItemCount);
				if (pressed & (KEY_A)) {
					strP = directoryStringFromPos(entriesTable, pos);
					if (!(strstr(strP, "~"))) {
						if (strcmp(strP, "..") == 0) {
							directoryBack(currentDir);
						}
						else {
							if (strcmp(currentDir, "/")) {
								strlcat(currentDir, "/", sizeof(currentDir));
							}
							strlcat(currentDir, strP, sizeof(currentDir));
						}
						dItemCount = getDirectory(dTable, currentDir, fileTypes);
						if (dItemCount == -1) {
							directoryBack(currentDir);
							dItemCount = 0;
						}
						pos = 0;
						oldPos = -1;
					}
					else {
						return strP+1;
					}
				}
				if (oldPos != pos) {
					oldPos = pos;
					row = drawFileList(entriesTable, pos, dItemCount);
					outputLogToScreen();
				}
				else {
					drawLongFilename(entriesTable, pos, row);
				}
				if (pressed & (KEY_B)) {
					break;
				}
				updateInfoLog();
			}
		}
	}
	return NULL;
}

static void directoryBack(char *path) {
	int i = strlen(path)-strlen(strrchr(path, '/'));
	path[i] = 0;
	path[i+1] = 0;
	path[0] = '/';
}

static void directoryInit(char *dirTable) {
	dItemCount = 0;
	memset(dirTable, 0, directoryCacheSize);
	dCacheLeft = directoryCacheSize - (directoryMaxEntries * 4);
	entriesTable = (char **)(dirTable + dCacheLeft);
	entriesTable[dItemCount] = dirTable;
}
/*
static void directoryFlushCache() {
	free(dTable);
	dTable = 0;
	dItemCount = 0;
	dirCRC32 = 0;
}
*/
const char *directoryStringFromPos(char **dirEntries, int pos) {
	if (pos < dItemCount) {
		return dirEntries[pos];
	}
	return NULL;
}

static const char *directoryAddDirname(char **dirEntries, const char *dirName) {
	char *dest = dirEntries[dItemCount];
	int nameLen = strlen(dirName) + 1;
	if (dItemCount < directoryMaxEntries) {
		if ((dCacheLeft -= (nameLen)) > 0) {
			memcpy(dest, dirName, nameLen);
			dItemCount++;
			dirEntries[dItemCount] = dest + nameLen;
		}
	}
	return dest;
}

static const char *directoryAddFilename(char **dirEntries, const char *fileName) {
	char *dest = dirEntries[dItemCount];
	int nameLen = strlen(fileName) + 2;
	if (dItemCount < directoryMaxEntries) {
		if ((dCacheLeft -= (nameLen)) > 0) {
			*dest = '~';
			memcpy(dest+1, fileName, nameLen - 1);
			dItemCount++;
			dirEntries[dItemCount] = dest + nameLen;
		}
	}
	return dest;
}

static void directorySort(char **dirEntries, int count) {
	int i, j, dirT;
	char *tmp;

	for (j = 0; j < count; j++) {
		dirT = 0;
		for (i = 0; i < count; i++) {
			if (strcasecmp(dirEntries[i], dirEntries[i+1]) > 0) {
				tmp = dirEntries[i];
				dirEntries[i] = dirEntries[i+1];
				dirEntries[i+1] = tmp;
				dirT = 1;
			}
		}
		drawSpinner();
		if (!dirT) {
			break;
		}
	}
}

static int getDirectory(char *dirTable, const char *dirName, const char *fileTypes) {
	int i;

	DIR *pDir;
	struct dirent *pent;
	char fileExtension[8];

	chdir("/");			// Stupid workaround.
	if (chdir(dirName) == -1) {
		return -1;
	}
	drawText(dirName, 0, 0);

	i = crc32(0, (const unsigned char *)dirName, strlen(dirName));
	i = crc32(i, (const unsigned char *)fileTypes, strlen(fileTypes));
	if (i == dirCRC32) {
		return dItemCount;
	}
	dirCRC32 = i;
	i = 0;

	directoryInit(dirTable);
	pDir = opendir(dirName);

	if (pDir){
		while ((pent = readdir(pDir)) != NULL) {
			if (strncmp("..", pent->d_name, 2)) {
				if (strncmp(".", pent->d_name, 1) == 0) {
					continue;
				}
			}
			if (pent->d_type == DT_DIR) {
				directoryAddDirname(entriesTable, pent->d_name);
			} else {
				getFileExtension(fileExtension, pent->d_name);
				if (fileExtension[0] == 0) {
					continue;
				}
				if (strstr(fileTypes, fileExtension) == 0) {
					continue;
				}
				directoryAddFilename(entriesTable, pent->d_name);
			}
			i++;
			if (i >= directoryMaxEntries) {
				break;
			}
			drawSpinner();
		}
		closedir(pDir);
		directorySort(entriesTable, dItemCount-1);
	}
	return dItemCount;
}
