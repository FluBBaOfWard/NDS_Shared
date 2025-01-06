#ifndef EMUMENU_HEADER
#define EMUMENU_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define ARRSIZE(xxxx) (sizeof((xxxx))/sizeof((xxxx)[0]))

#define MENU_M(name,draw,list) {name, draw, ARRSIZE(list), list}

#define KEY_OPEN_MENU BIT(30)  // Key to open menu

typedef void (*fptr)(void);
typedef const char *const (*tfptr)(void);

// MenuItem
typedef struct {
	/// Text of item
	const char *const text;
	/// Function of item
	const fptr fn;
	/// Function of to get second text
	const tfptr txt2;
} MItem;

// Menu
typedef struct {
	/// Name of menu
	const char *const header;
	/// Function to draw menu
	const fptr drawFunc;
	/// Number of items in the menu
	int itemCount;
	/// List of menu items
	const MItem *items;
} Menu;

extern const char *const autoTxt[];
extern const char *const brighTxt[];

/// Autofire for button A, 0=off, 1=on, 2=R
extern u8 autoA;
/// Autofire for button B, 0=off, 1=on, 2=R
extern u8 autoB;
/// Gamma value for display
extern u8 gGammaValue;

/// Should we output debug text?
extern bool gDebugSet;
/// True if any settings have changed that are not saved.
extern bool settingsChanged;
/// True if emulation paused right now.
extern bool pauseEmulation;
/// True if exit menu is enabled.
extern bool enableExit;

/// Various common settings
extern int emuSettings;
/// Current set sleep time, default 5min.
extern int sleepTime;
/// This is the current row in the menu.
extern int selected;
/// YOffset for menus with many items.
extern int menuYOffset;

void guiRunLoop(void);
int getInput(void);
int getMenuInput(int menuItems);
int getMenuPos(int keyHit, int sel, int menuItems);
int getMenuTouch(int *keyHit, int sel, int menuItems);
void redrawUI(void);
int drawFileList(char **dirEntries, int sel, int items);
void drawLongFilename(char **dTable, int item, int row);
void drawText(const char *str, int row, int hiLite);
void drawMenuText(const char *str, int row, int hiLite);
void drawSubText(const char *str, int row, int hiLite);
void drawBText(const char *str, int row, int shadow);
void drawTextBackground(const char *str, int row, int shadow);
void strlMerge(char *dst, const char *src1, const char *src2, int dstSize);
void drawSubItem(const char *str1, const char *str2);
void drawMenuItem(const char *str1);
int drawItem(const char *str, int col, int row, int pal);

/**
 * Draw two strings on the same row after each other with a space inbetween.
 */
void drawStrings(const char *str1, const char *str2, int col, int row, int pal);

void drawItemBackground(const char *str, int row, int sub);
void drawTabs(void);
void setupMenu(void);
void setupSubMenuText(void);
void setupSubMenu(const char *menuString);
void cls(int chrMap);
void setupCompressedBackground(const void *tiles, const void *map, int rowOffset);
void int2Str(int i, char *s);
void int2HexStr(char *dest, int val);
void short2HexStr(char *dest, short val);
void char2HexStr(char *dest, char val);
void infoOutput(const char *str);
void updateInfoLog(void);
void outputLogToScreen(void);
void debugOutput(const char *str);
void uiDummy(void);
void enterMenu(int menuNr);
void openMenu(void);
void backOutOfMenu(void);
void closeMenu(void);
bool isMenuOpen(void);

void exitEmulator(void);

void uiNullDefault(void);
void uiAuto(void);

void ui1(void);
void ui2(void);
void ui3(void);
void ui4(void);
void ui5(void);
void ui6(void);
void ui7(void);
void ui8(void);
void ui9(void);
void ui10(void);

char *updateTime(void);

void autoPauseGameSet(void);
const char *getAutoPauseGameText(void);
void autoStateSet(void);
const char *getAutoStateText(void);
void autoSettingsSet(void);
const char *getAutoSettingsText(void);
void autoNVRAMSet(void);
const char *getAutoNVRAMText(void);
void saveNVRAMSet(void);
const char *getSaveNVRAMText(void);
void debugTextSet(void);
const char *getDebugText(void);
void sleepSet(void);
const char *getSleepText(void);
void powerSaveSet(void);
const char *getPowerSaveText(void);
void screenSwapSet(void);
const char *getScreenSwapText(void);
/// Change gamma (brightness)
void gammaSet(void);
const char *getGammaText(void);
void autoASet(void);
const char *getAutoAText(void);
void autoBSet(void);
const char *getAutoBText(void);
void speedSet(void);
const char *getSpeedText(void);
void flickSet(void);
const char *getFlickText(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUMENU_HEADER
