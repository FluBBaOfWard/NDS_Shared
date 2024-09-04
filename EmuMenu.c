#include <nds.h>

//#include <string.h>
//#include <stdio.h>
#include <time.h>

#include "EmuMenu.h"
#include "EmuSettings.h"
#include "FileHelper.h"
#include "AsmExtra.h"
#include "../Main.h"
#include "../Gui.h"
#include "../FileHandling.h"
#include "../Gfx.h"				// gFlicker & gTwitch
#include "../io.h"
#include "../Sound.h"

#define MENU_MAX_DEPTH (8)

extern const fptr fnMain[];
extern const fptr *const fnListX[];
extern const u8 menuXItems[];
extern const fptr drawUIX[];

static void exitUI(void);
static void drawClock(void);
static void setSelectedMenu(int menuNr);
static void setSelectedMain(int menuNr);

static const char menuTopRow[] = {   0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x83, ' ',0x81,0x82,0x82,0x82,0x82,0x82,0x83,0};
static const char menuMiddleRow[] = { 'X',0x85, ' ',0x84, 'F', 'i', 'l', 'e',0x85, ' ',0x84, 'O', 'p', 't', 'i', 'o', 'n', 's',0x85, ' ',0x84, 'A', 'b', 'o', 'u', 't',0x85,0};
static const char menuBottomRow[] = { ' ',0x8A,0x82,0x89, ' ', ' ', ' ', ' ',0x8A,0x82,0x89, ' ', ' ', ' ', ' ', ' ', ' ', ' ',0x8A,0x82,0x89, ' ', ' ', ' ', ' ', ' ',0x8A,0x82,0x82,0x82,0x82,0x82,0};
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

u8 autoA = 0;				// 0=off, 1=on, 2=R
u8 autoB = 0;

bool gDebugSet = 0;
bool settingsChanged = false;
bool pauseEmulation = false;
bool enableExit = false;

int emuSettings = 0;
int sleepTime = 60*60*5;		// 5min
int selected = 0;

static int selectedMenu = 0;
static int selectedMain = 0;
static int lastMainMenu = 1;
static int menuItemRow = 0;
static int lineRepeat = 0;
int menuYOffset = 0;
// How deep we are in the menu tree
static int menuLevel = 0;
static char menuPath[MENU_MAX_DEPTH];
static char menuPositions[MENU_MAX_DEPTH];

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
	drawMenuText(menuString, 3, 0);
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
	setSelectedMain(1);
}
void ui2() {
	setSelectedMain(2);
}
void ui3() {
	setSelectedMain(3);
}
void ui4() {
	enterMenu(4);
}
void ui5() {
	enterMenu(5);
}
void ui6() {
	enterMenu(6);
}
void ui7() {
	enterMenu(7);
}
void ui8() {
	enterMenu(8);
}
void ui9() {
	enterMenu(9);
}
void ui10() {
	enterMenu(10);
}

void setSelectedMenu(int menuNr) {
	selectedMenu = menuNr;
	if (selectedMenu == 0) {
		exitUI();
	}
	else if (selectedMenu < 4) {
		selectedMain = selectedMenu;
	}
	cls(0);
	redrawUI();
}

void setSelectedMain(int menuNr) {
	menuLevel = 0;
	enterMenu(menuNr);
}

void enterMenu(int menuNr) {
	menuPositions[menuLevel] = selected;
	menuLevel++;
	if (menuLevel >= MENU_MAX_DEPTH) {
		menuLevel = MENU_MAX_DEPTH - 1;
	}
	if (menuPath[menuLevel] == menuNr) {
		selected = menuPositions[menuLevel];
	}
	else {
		menuPath[menuLevel] = menuNr;
		selected = 0;
	}
	setSelectedMenu(menuNr);
}

void backOutOfMenu() {
	menuPositions[menuLevel] = selected;
	menuLevel--;
	if ( menuLevel < 0) {
		menuLevel = 0;
	}
	selected = menuPositions[menuLevel];
	setSelectedMenu(menuPath[menuLevel]);
}

void openMenu() {
	enterGUI();
	setSelectedMain(lastMainMenu);
	powerOn(POWER_2D_B);
	powerOn(PM_BACKLIGHT_TOP);
	powerOn(PM_BACKLIGHT_BOTTOM);
	lcdMainOnTop();
	if (emuSettings & AUTOPAUSE_EMULATION) {
		pauseEmulation = true;
		setMuteSoundGUI();
	}
	if (emuSettings & AUTOSAVE_NVRAM) {
		saveNVRAM();
	}
}

void closeMenu() {
	menuLevel = 0;
	backOutOfMenu();
}

bool isMenuOpen() {
	return (selectedMenu != 0);
}

void exitUI() {
	if (settingsChanged && (emuSettings & AUTOSAVE_SETTINGS)) {
		saveSettings();
		settingsChanged = false;
	}
	lastMainMenu = selectedMain;
	if (lastMainMenu < 1) {
		lastMainMenu = 1;
	}

	if (emuSettings & MAIN_ON_BOTTOM) {
		lcdMainOnBottom();
	}
	if (emuSettings & POWER_SAVE_MENU) {
		powerOff(POWER_2D_B);
		if (emuSettings & MAIN_ON_BOTTOM) {
			powerOff(PM_BACKLIGHT_TOP);
		} else {
			powerOff(PM_BACKLIGHT_BOTTOM);
		}
	}
	exitGUI();
	pauseEmulation = false;
	setMuteSoundGUI();
}

/// This is during emulation.
void nullUI() {
	int key = getInput();
	if (EMUinput & KEY_OPEN_MENU) {
		openMenu();
		return;
	}

	if (gDebugSet) {
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
	const int key = getMenuInput(menuXItems[selectedMenu]);

	if (key & KEY_A) {
		fnListX[selectedMenu][selected]();
	}
	if (key & KEY_B) {
		backOutOfMenu();
	}
	if (key & KEY_L) {
		if (selectedMain > 0) {
			setSelectedMain(selectedMain-1);
		}
	}
	if (key & KEY_R) {
		if (selectedMain < 3) {
			setSelectedMain(selectedMain+1);
		}
	}
	if (key & (KEY_A|KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT|KEY_TOUCH)) {
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
	EMUinput = convertInput(keysHeld());

	int keyHit = keysDown();	// Buttons pressed this loop
	int dpad = keysDownRepeat() & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT);
	return dpad|(keyHit & (KEY_A|KEY_B|KEY_X|KEY_Y|KEY_START|KEY_L|KEY_R|KEY_TOUCH));
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
				setSelectedMain(1);
				sel = 0;
			}
			else if ( (xPos > 10) && (xPos < 18)) {
				setSelectedMain(2);
				sel = 0;
			}
			else if ( (xPos > 20) && (xPos < 26)) {
				setSelectedMain(3);
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
	// Fix up vertical position of items
	int itemCount = menuXItems[selectedMenu];
	if (itemCount > 9 && selected > 4) {
		menuYOffset = (selected - 4);
		if (menuYOffset > (itemCount - 9)) {
			menuYOffset = (itemCount - 9);
		}
	}
	else {
		menuYOffset = 0;
	}
	drawUIX[selectedMenu]();
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

int drawItem(const char *str, int col, int row, int pal) {
	u16 *here = map0sub+col+row*32;
	int i = 0;

	int attrib = (pal<<12)|0x100;
	while (str[i] >= ' ') {
		here[i] = str[i]|attrib;
		i++;
		if ((i + col) > 31) {
			break;
		}
	}
	return i+col;
}

void drawTextLine(const char *str, int col, int row, int hiLite) {
	u16 *here = map0sub+row*32;
	int i = drawItem(str, col, row, hiLite);

	for (; i < 32; i++) {
		here[i] = 0x0120;
	}
}

void drawText(const char *str, int row, int hiLite) {
	drawTextLine(str, 0, row, hiLite);
}

void drawMenuText(const char *str, int row, int hiLite) {
	drawTextLine(str, 1, row, hiLite);
}

void drawSubText(const char *str, int row, int hiLite) {
	drawTextLine(str, 2, row, hiLite);
}

void drawStrings(const char *str1, const char *str2, int col, int row, int pal) {
	char str[48];
	if (str2) {
		strlMerge(str, str1, " ", sizeof(str));
		strlMerge(str, str, str2, sizeof(str));
	}
	else {
		strlcpy(str, str1, sizeof(str));
	}
	drawTextLine(str, col, row, pal);
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
	int drawRow = (menuItemRow-menuYOffset)*2+5;
	if (drawRow > 4 && drawRow < 24) {
		bool hilite = (selected == menuItemRow);
		if (hilite) {
			drawItemBackground(str, drawRow, 0);
		}
		drawMenuText(str, drawRow, hilite);
		if (drawRow == 21) {
			drawMenuText("", 23, false);
		}
	}
	menuItemRow++;
}

void drawSubItem(const char *str1, const char *str2) {
	int drawRow = (menuItemRow-menuYOffset)*2+5;
	if (drawRow > 4 && drawRow < 24) {
		bool hilite = (selected == menuItemRow);
		if (hilite) {
			drawItemBackground(str1, drawRow, 1);
		}
		drawStrings(str1, str2, 2, drawRow, hilite);
		if (drawRow == 21) {
			drawMenuText("", 23, false);
		}
	}
	menuItemRow++;
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
	while (str[i-sub] >= ' ') {
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
		mod = (j>>k) & 15;
		*(s++) = (mod + '0');
	}
	*(s++) = 0;
}

void int2HexStr(char *dest, int val) {
	int i;
	for (i = 0; i < 8; i++) {
		dest[7-i] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
		val = val>>4;
	}
	dest[8] = 0;
}

void short2HexStr(char *dest, short val) {
	int i;
	for (i = 0; i < 4; i++) {
		dest[3-i] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
		val = val>>4;
	}
	dest[4] = 0;
}

void char2HexStr(char *dest, char val) {
	dest[0] = ((val>>4) & 0xF) + ((((val>>4) & 0xF) < 10) ? '0' : '7');
	dest[1] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
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
	if (gDebugSet) {
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
		if (emuSettings & SHOW_UI_CLOCK) {
			drawClock();
		}
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

void saveNVRAMSet() {
	emuSettings ^= AUTOSAVE_NVRAM;
	settingsChanged = true;
}

void debugTextSet() {
	gDebugSet ^= true;
}

void sleepSet() {
	int i;
	i = (emuSettings+0x10) & AUTOSLEEP_MASK;
	emuSettings = (emuSettings & ~AUTOSLEEP_MASK) | i;
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
	i = (emuSettings+0x40) & EMUSPEED_MASK;
	emuSettings = (emuSettings & ~EMUSPEED_MASK) | i;
	setEmuSpeed(i>>6);
}

void flickSet() {
	gFlicker++;
	if (gFlicker > 1) {
		gFlicker = 0;
		gTwitch = 0;
	}
}
