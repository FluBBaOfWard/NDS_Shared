#include <nds.h>

//#include <string.h>
//#include <stdio.h>
#include <time.h>

#include "EmuMenu.h"
#include "EmuSettings.h"
#include "../Gui.h"
#include "../Main.h"
#include "../FileHandling.h"
#include "../Gfx.h"
#include "../io.h"
#include "../Sound.h"
#include "AsmExtra.h"

extern const fptr fnMain[];
extern const fptr *const fnListX[];
extern const u8 menuXitems[];
extern const fptr drawuiX[];
extern const u8 menuXback[];

static void exitUI(void);
static void drawClock(void);

static const char menuTopRow[] = {   0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x82,0x83,0};
static const char menuMiddleRow[] = { 'X',0x85, ' ',0x84, 'F', 'i', 'l', 'e',0x85, ' ',0x84, 'O', 'p', 't', 'i', 'o', 'n', 's',0x85, ' ',0x84, 'A', 'b', 'o', 'u', 't',0x85,0};
static const char menuBottomRow[] = { ' ',0x8A, ' ',0x89, ' ', ' ', ' ', ' ',0x8A, ' ',0x89, ' ', ' ', ' ', ' ', ' ', ' ', ' ',0x8A, ' ',0x89, ' ', ' ', ' ', ' ', ' ',0x8A,0};
static const char menuFrontRow[] = { 0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0};

static const char tabTopFile[] = {0x81,0x82,0x82,0x82,0x82,0x83,0};
static const char tabMidFile[] = {0x84, 'F', 'i', 'l', 'e',0x85,0};
static const char tabBotFile[] = {0x89, ' ', ' ', ' ', ' ',0x8A,0};

static const char tabTopOptions[] = {0x81,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x83,0};
static const char tabMidOptions[] = {0x84, 'O', 'p', 't', 'i', 'o', 'n', 's',0x85,0};
static const char tabBotOptions[] = {0x89, ' ', ' ', ' ', ' ', ' ', ' ', ' ',0x8A,0};

static const char tabTopAbout[] = {0x81,0x82,0x82,0x82,0x82,0x82,0x83,0};
static const char tabMidAbout[] = {0x84, 'A', 'b', 'o', 'u', 't',0x85,0};
static const char tabBotAbout[] = {0x89, ' ', ' ', ' ', ' ', ' ',0x8A,0};

u8 autoA = 0;			// 0=off, 1=on, 2=R
u8 autoB = 0;
u8 g_debugSet = 0;		// Should we output debug text?
bool settingsChanged = false;

int selected = 0;
static int selectedMenu = 0;
static int selectedMain = 0;
static int lastMainMenu = 1;
static int mainUIPosition = 0;
static int menuItemRow = 0;
static int lineRepeat = 0;

static int logBufPtr = 0;
static int logBufPtrOld = 0;
static int logTimer = 0;
static time_t oldTime = 0;

static char logBuffer[32][80];
static char timeBuffer[6];

void guiRunLoop(void) {
	fnMain[selectedMenu]();
	updateTime();
}

void uiNullDefault() {
	drawText("        Touch screen or", 10, 0);
	drawText("      press L+R for menu.", 11, 0);
}


void setupMenu() {
	drawTabs();
	menuItemRow = 0;
}
void setupSubMenu(const char *menuString) {
	setupMenu();
	drawSubText(menuString, 3, 0);
}

void uiYesNo() {
	setupSubMenu("Are you sure?");
	drawSubItem("Yes ", 0);
	drawSubItem("No ", 0);
}

void drawTabs() {
	drawBText(menuTopRow, 0, 1);
	drawBText(menuMiddleRow, 1, 1);
	drawBText(menuBottomRow, 2, 1);
	drawText(menuFrontRow, 2, 0);
	if (selectedMain == 1) {
		drawItem(tabTopFile, 3, 0+32, 0);
		drawItem(tabMidFile, 3, 1+32, 0);
		drawItem(tabBotFile, 3, 2, 0);
	}
	else if (selectedMain == 2) {
		drawItem(tabTopOptions, 10, 0+32, 0);
		drawItem(tabMidOptions, 10, 1+32, 0);
		drawItem(tabBotOptions, 10, 2, 0);
	}
	else if (selectedMain == 3) {
		drawItem(tabTopAbout, 20, 0+32, 0);
		drawItem(tabMidAbout, 20, 1+32, 0);
		drawItem(tabBotAbout, 20, 2, 0);
	}
}

void uiDummy() {
}

void ui1() {
	setSelectedMenu(1);
}
void ui2() {
	setSelectedMenu(2);
}
void ui3() {
	setSelectedMenu(3);
}
void ui4() {
	setSelectedMenu(4);
}
void ui5() {
	setSelectedMenu(5);
}
void ui6() {
	setSelectedMenu(6);
}
void ui7() {
	setSelectedMenu(7);
}
void ui8() {
	setSelectedMenu(8);
}

void setSelectedMenu(int menuNr) {
	if (selectedMain == selectedMenu) {		// Are we going from a mainMenu to a subMenu?
		mainUIPosition = selected;
	}
	selectedMenu = menuNr;
	selected = 0;

	if (selectedMain == selectedMenu) {		// Are we going back to a mainMenu?
		selected = mainUIPosition;
	}
	if (selectedMenu == 0) {
		exitUI();
	}
	else if (selectedMenu < 4) {
		selectedMain = selectedMenu;
	}
	cls(0);
	redrawUI();
}

void openMenu() {
	enterGUI();
	setSelectedMenu(lastMainMenu);
	powerOn(POWER_2D_B);
	powerOn(PM_BACKLIGHT_TOP);
	powerOn(PM_BACKLIGHT_BOTTOM);
	lcdMainOnTop();
	if (emuSettings & AUTOPAUSE_EMULATION) {		// Should we pause when menu is open?
		pauseEmulation = true;
		setMuteSoundGUI();
	}
	if (emuSettings & AUTOSAVE_NVRAM) {
		saveNVRAM();
	}
}

void closeMenu() {
	setSelectedMenu(0);
}

void backOutOfMenu() {
	setSelectedMenu(menuXback[selectedMenu]);
}

void exitUI() {
	exitGUI();
	pauseEmulation = false;
	setMuteSoundGUI();
	if (settingsChanged && (emuSettings & AUTOSAVE_SETTINGS)) {
		saveSettings();
		settingsChanged = false;
	}
	if (emuSettings & MAIN_ON_BOTTOM) {
		lcdMainOnBottom();
	}

	lastMainMenu = selectedMain;
	if (lastMainMenu < 1) {
		lastMainMenu = 1;
	}

	if (emuSettings & POWER_SAVE_MENU) {
		powerOff(POWER_2D_B);
		if (emuSettings & MAIN_ON_BOTTOM) {
			powerOff(PM_BACKLIGHT_TOP);
		} else {
			powerOff(PM_BACKLIGHT_BOTTOM);
		}
	}
}

/// This is during emulation.
void nullUI() {
	int key = getInput();
	if ((EMUinput & (KEY_L|KEY_R)) == (KEY_L|KEY_R)) {
		openMenu();
		return;
	}

	if (g_debugSet) {
		drawItem(fpsText, 0, 0, 0);
		nullUIDebug(key);
	}
	else {
		nullUINormal(key);
	}
	updateInfoLog();
}

/// This is during menu.
void subUI() {
	const int key = getMenuInput(menuXitems[selectedMenu]);

	if (key & KEY_A) {
		fnListX[selectedMenu][selected]();
	}
	if (key & KEY_B) {
		backOutOfMenu();
	}
	if (key & KEY_L) {
		if (selectedMain > 0) {
			setSelectedMenu(selectedMain-1);
		}
	}
	if (key & KEY_R) {
		if (selectedMain < 3) {
			setSelectedMenu(selectedMain+1);
		}
	}
	if (key & (KEY_A+KEY_UP+KEY_DOWN+KEY_LEFT+KEY_RIGHT+KEY_TOUCH)) {
		redrawUI();
	}
	updateInfoLog();
}

int getMenuInput(int itemCount) {
	int keyHit = getInput();
	selected = getMenuTouch(&keyHit, selected, itemCount);
	selected = getMenuPos(keyHit, selected, itemCount);
	return keyHit;
}

//---------------------------------------------------------------------------------
int getInput() {
	int dpad;
	int keyHit = keysDown();	// Buttons pressed this loop

	EMUinput = keysHeld();

	dpad = keysDownRepeat() & (KEY_UP+KEY_DOWN+KEY_LEFT+KEY_RIGHT);
	return dpad|(keyHit & (KEY_A+KEY_B+KEY_START+KEY_L+KEY_R+KEY_TOUCH));
}

//---------------------------------------------------------------------------------
int getMenuPos(int keyHit, int sel, int itemCount) {
	if (keyHit & KEY_UP) {
		sel = (sel+itemCount-1)%itemCount;
	}
	if (keyHit & KEY_DOWN) {
		sel = (sel+1)%itemCount;
	}
	if (keyHit & KEY_RIGHT) {
		sel += 10;
		if (sel > itemCount-1) {
			sel = itemCount-1;
		}
	}
	if (keyHit & KEY_LEFT) {
		sel -= 10;
		if (sel < 0) {
			sel = 0;
		}
	}
	return sel;
}

//---------------------------------------------------------------------------------
int getMenuTouch(int *keyHit, int sel, int itemCount) {
	int yPos, xPos;
	touchPosition myTouch;

	if (*keyHit & KEY_TOUCH) {
		touchRead(&myTouch);
		yPos = (myTouch.py>>3) - 5;
		if ( (yPos >= 0) && ((yPos>>1) < itemCount) && !(yPos&1) ) {
			if (sel == (yPos>>1) ) {
				*keyHit |= KEY_A;
			}
			else {
				sel = yPos>>1;
			}
		} else if (myTouch.py < 24) {
			xPos = myTouch.px>>3;
			if ( (xPos > 3) && (xPos < 8)) {
				setSelectedMenu(1);
				sel = 0;
			}
			else if ( (xPos > 10) && (xPos < 18)) {
				setSelectedMenu(2);
				sel = 0;
			}
			else if ( (xPos > 20) && (xPos < 26)) {
				setSelectedMenu(3);
				sel = 0;
			}
			else if ( xPos < 2) {
				*keyHit |= KEY_B;
			}
		}
	}
	return sel;
}

void redrawUI() {
	drawuiX[selectedMenu]();
	outputLogToScreen();
}

int drawFileList(char **dirEntries, int sel, int itemCount) {
	int i, firstItem, selectedFile;
	const char *buf;
	lineRepeat = 0;				// Reset long filename position

	firstItem = sel-11;
	if (sel > (itemCount-13)) {
		firstItem = itemCount-23;
	}
	if (firstItem < 0) {
		firstItem = 0;
	}

	for (i = 0; i < (SCREEN_HEIGHT / 8 - 1); i++) {
		buf = directoryStringFromPos(dirEntries, firstItem + i);
		if ( buf == NULL ) {
			buf = "";
		}
		if ( *buf == '~' ) {
			buf++;
		}
		selectedFile = (i == (sel-firstItem)?4:0);
		drawText(buf, i+1, selectedFile);
		drawTextBackground(buf, i+1, selectedFile);
	}
	return (sel-firstItem);
}

void drawLongFilename(char **dirEntries, int item, int row) {
	const char *buf;
	static int dir = 1;
	int xLen, ofs = 0;

	buf = directoryStringFromPos(dirEntries, item);
	if ( *buf == '~' ) buf++;
	xLen = strlen(buf)-32;
	if (xLen < 1) {
		return;
	}
	lineRepeat += dir;

	if (lineRepeat > 60) {
		ofs = (lineRepeat - 60)/8;
		if (lineRepeat > (60+xLen*8)) {
			ofs = xLen;
		}
		if (lineRepeat > (120+xLen*8)) {
			dir = -1;
		}
	}
	if (lineRepeat < 0) {
		dir = 1;
	}
	drawText(buf+ofs, row+1, 4);
}

void cls(int chrMap) {
	int i = 0;
	int len = 0x400;				// 512x256
	u32 *scr = (u32 *)map0sub;
	for (; i < len; i++) {
		scr[i] = 0x01200120;
	}
}

void drawSubText(const char *str1, int row, int hiLite) {
	char str[32];
	strlMerge(str, " ", str1, sizeof(str));
	drawText(str, row, hiLite);
}

void drawText(const char *str, int row, int hiLite) {
	u16 *here = map0sub+row*32;
	int i = 0;

//	*here = hiLite?0x012a:0x0120;
//	here++;
	hiLite = (hiLite<<12)+0x0100;
	while (str[i] >= ' ') {
		here[i] = str[i]|hiLite;
		i++;
		if (i > 31) {
			break;
		}
	}
	for (; i < 32; i++) {
		here[i] = 0x0120;
	}
}

void drawBText(const char *str, int row, int shadow) {
	u16 *here = map0sub+(row+32)*32;
	int i = 0;

	shadow = (shadow<<13)+0x0100;
	while (str[i] >= ' ') {
		here[i] = str[i]|shadow;
		i++;
		if (i > 31) {
			break;
		}
	}
	for (; i < 32; i++) {
		here[i] = 0x0120;
	}
}

void drawTextBackground(const char *str, int row, int shadow) {
	u16 *here = map0sub+(row+32)*32;
	int i = 0;

	shadow = (shadow<<12)+0x0180;
	while (str[i] >= ' ') {
		here[i] = shadow;
		i++;
		if (i > 31) {
			break;
		}
	}
	for (; i < 32; i++) {
		here[i] = 0x0120;
	}
}

void strlMerge(char *dst, const char *src1, const char *src2, int dstSize) {
	if (dst != src1) {
		strlcpy(dst, src1, dstSize);
	}
	strlcat(dst, src2, dstSize);
}

void drawMenuItem(const char *str) {
	if (selected == menuItemRow) {
		drawItemBackground(str, menuItemRow*2+5, 0);
	}
	drawSubText(str, menuItemRow*2+5, (selected == menuItemRow));
	menuItemRow++;
}

void drawSubItem(const char *str1, const char *str2) {
	char str[48];
	if (selected == menuItemRow) {
		drawItemBackground(str1, menuItemRow*2+5, 1);
	}
	strlMerge(str, "  ", str1, sizeof(str));
	if (str2) {
		strlMerge(str, str, str2, sizeof(str));
	}
	drawText(str, menuItemRow*2+5, selected==menuItemRow);
	menuItemRow++;
}

void drawItem(const char *str, int col, int row, int pal) {
	u16 *here = map0sub+col+row*32;
	int attrib = (pal<<12)|0x100;
	int i = 0;

	while (str[i] >= ' ') {
		here[i] = str[i]|attrib;
		i++;
		if ((i + col) > 31) {
			break;
		}
	}
}

void setMenuItemRow(int row) {
	menuItemRow = row;
}

void clearItemBackground(int row) {
	u32 *here = (u32 *)(map0sub+36*32);
	int i = 0;
	for (; i < 16*20; i++) {
		here[i] = 0x01200120;
	}
}

void drawItemBackground(const char *str, int row, int sub) {
	u16 *here = map0sub+(row+31)*32;
	int i = sub;
	clearItemBackground(row);

	here[i+32] = 0x84|0x2100;
	here[i+64] = 0x86|0x2100;
	here[i++]  = 0x81|0x2100;
	while (str[i] >= ' ') {
		here[i+32] = 0x80|0x2100;
		here[i+64] = 0x87|0x2100;
		here[i++]  = 0x82|0x2100;
		if (i > 29) {
			break;
		}
	}
	here[i+32] = 0x80|0x2100;
	here[i+64] = 0x87|0x2100;
	here[i++]  = 0x82|0x2100;
	here[i+32] = 0x85|0x2100;
	here[i+64] = 0x88|0x2100;
	here[i++]  = 0x83|0x2100;
	for (; i < 32; i++) {
		here[i] = 0x0120;
	}
}

void setupCompressedBackground(const void *tiles, const void *map, int rowOffset) {
	int i;
	decompress(tiles, BG_TILE_RAM_SUB(1), LZ77Vram);
	decompress(map, map0sub+32*rowOffset, LZ77Vram);
	for (i = 0; i < 32*(24-rowOffset); i++) {
		map0sub[i + 32*rowOffset] = map0sub[i + 32*rowOffset] | 0x8200;
	}
}

void int2Str(int i, char *s) {
	u32 j;
	int mod,k;

	j = bin2BCD(i);
	for (k = 28; k >= 0; k -= 4) {
		mod = (j>>k)&15;
		*(s++) = (mod+'0');
	}
	*(s++) = 0;
}

void int2HexStr(char *dest, int val) {
	int i;
	for (i = 0; i < 8; i++) {
		dest[7-i] = (val & 0xF) + ((val & 0xF) < 10) ? '0' : 'A';
		val = val>>4;
	}
	dest[8] = 0;
}

void char2HexStr(char *dest, char val) {
	dest[0] = ((val>>4) & 0xF) + ((val & 0xF) < 10) ? '0' : 'A';
	dest[1] = (val & 0xF) + ((val & 0xF) < 10) ? '0' : 'A';
	dest[2] = 0;
}

void updateInfoLog() {
	if (logBufPtrOld != logBufPtr) {
		logBufPtrOld = logBufPtr;
		outputLogToScreen();
		logTimer = 120;
	}
	if (logTimer >= 0) {
		if (logTimer == 0) {
			int i;
			for (i = 0; i < 8; i++) {
				char *str = logBuffer[((logBufPtr-7+i)&7)];
				if (str[0] != 0) {
					drawText("", i+16, 0);
					str[0] = 0;
				}
			}
			redrawUI();
		}
		logTimer--;
	}
}

void outputLogToScreen() {
	int i;
	for (i = 0; i < 8; i++) {
		char *str = logBuffer[((logBufPtr-7+i)&7)];
		if (str[0] != 0) {
			drawText(str, i+16, 0);
		}
	}
}

void infoOutput(const char *str) {
	logBufPtr++;
	strlcpy(logBuffer[logBufPtr&7], str, 32);
}

void debugOutput(const char *str) {
	if (g_debugSet) {
		infoOutput(str);
	}
	debugOutputToEmulator(str);
}

void exitEmulator() {
	if (enableExit) {
		exit(0);
	}
}

char *updateTime() {
	time_t unixTime = time(NULL);
	if (oldTime != unixTime) {
		oldTime = unixTime;
		struct tm *timeStruct = gmtime((const time_t *)&unixTime);
		timeBuffer[0] = bin2BCD(timeStruct->tm_hour);
		timeBuffer[1] = bin2BCD(timeStruct->tm_min);
		timeBuffer[2] = bin2BCD(timeStruct->tm_sec);
		timeBuffer[3] = bin2BCD(timeStruct->tm_year);
		timeBuffer[4] = bin2BCD(timeStruct->tm_mon + 1);
		timeBuffer[5] = bin2BCD(timeStruct->tm_mday);
		drawClock();
	}
	return timeBuffer;
}

void drawClock() {
	char str[9];

	strlcpy(str, "00:00:00", sizeof(str));
	str[0] = ((timeBuffer[0] >> 4) & 3) + '0';	//Hours.
	str[1] = (timeBuffer[0] & 15) + '0';
	str[3] = ((timeBuffer[1] >> 4) & 15) + '0';	//Minutes.
	str[4] = (timeBuffer[1] & 15) + '0';
	str[6] = ((timeBuffer[2] >> 4) & 15) + '0';	//Seconds.
	str[7] = (timeBuffer[2] & 15) + '0';

	drawItem(str, 23, 0, 0);
}

//---------------------------------------------

void autoPauseGameSet() {
	emuSettings ^= AUTOPAUSE_EMULATION;
	settingsChanged = true;
	pauseEmulation = (emuSettings & AUTOPAUSE_EMULATION);
	setMuteSoundGUI();
}

void autoStateSet() {
	emuSettings ^= AUTOLOAD_STATE;
	settingsChanged = true;
}

void autoSettingsSet() {
	emuSettings ^= AUTOSAVE_SETTINGS;
	settingsChanged = true;
}

void autoNVRAMSet() {
	emuSettings ^= AUTOLOAD_NVRAM;
	settingsChanged = true;
}

void debugTextSet() {
	g_debugSet ^= true;
}

void sleepSet() {
	int i;
	i = (emuSettings+0x10) & 0x30;
	emuSettings = (emuSettings & ~0x30) | i;
	if (i == AUTOSLEEP_5MIN) {
		sleepTime = 60*60*5;		// 5 min
	}
	else if (i == AUTOSLEEP_10MIN) {
		sleepTime = 60*60*10;		// 10 min
	}
	else if (i == AUTOSLEEP_30MIN) {
		sleepTime = 60*60*30;		// 30 min
	}
	else if (i == AUTOSLEEP_OFF) {
		sleepTime = 0x7F000000;		// 360 days...
	}
	settingsChanged = true;
}

void powerSaveSet() {
	emuSettings ^= POWER_SAVE_MENU;
	settingsChanged = true;
}

void screenSwapSet() {
	emuSettings ^= MAIN_ON_BOTTOM;
	settingsChanged = true;
}

void autoASet() {
	autoA++;
	joyCfg |= KEY_A+(KEY_A<<16);
	if (autoA == 1) {
		joyCfg &= ~KEY_A;
	} else if (autoA == 2) {
		joyCfg &= ~(KEY_A<<16);
	} else {
		autoA = 0;
	}
}

void autoBSet() {
	autoB++;
	joyCfg |= KEY_B+(KEY_B<<16);
	if (autoB == 1) {
		joyCfg &= ~KEY_B;
	} else if (autoB == 2) {
		joyCfg &= ~(KEY_B<<16);
	} else {
		autoB = 0;
	}
}

void speedSet() {
	int i;
	i = (emuSettings+0x40) & 0xC0;
	emuSettings = (emuSettings & ~0xC0) | i;
	setEmuSpeed(i>>6);
}

void flickSet() {
	g_flicker++;
	if (g_flicker > 1){
		g_flicker = 0;
		g_twitch = 0;
	}
}
