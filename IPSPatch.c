#include <nds.h>
#include <fat.h>

#include <stdio.h>

#include "EmuMenu.h"

int patchRom(char *rom, const char *fileName, const int size) {
	FILE *file;
	char buffer[8];

	if ((file = fopen(fileName, "r"))) {
		fread(buffer, 1, 5, file);
		buffer[5] = 0;
		if (!strcmp(buffer, "PATCH")) {
			int ofs;
			int len;
			while (fread(buffer, 1, 3, file) == 3) {
				ofs = (buffer[0]<<16) + (buffer[1]<<8) + buffer[2];
				if (ofs == 0x464f45 || ofs > size) { // EOF?
					break;
				}
				fread(buffer, 1, 2, file);
				len = (buffer[0]<<8) + buffer[1];
				if (len == 0) {
					fread(buffer, 1, 2, file);
					len = (buffer[0]<<8) + buffer[1];
					fread(buffer, 1, 1, file);
					memset(&rom[ofs], buffer[0], len);
				}
				else {
					fread(&rom[ofs], 1, len, file);
				}
			}
		}
		else {
			infoOutput("Not an IPS file:");
			infoOutput(fileName);
		}
		fclose(file);
	}
	else {
		infoOutput("Couldn't open file:");
		infoOutput(fileName);
	}
	return 1;
}
