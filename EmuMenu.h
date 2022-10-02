#ifndef EMUMENU_HEADER
#define EMUMENU_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define ARRSIZE(xxxx) (sizeof((xxxx))/sizeof((xxxx)[0]))

typedef void (*fptr)(void);

/// Autofire for button A
extern u8 autoA;
/// Autofire for button B
extern u8 autoB;

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
void drawItemBackground(const char *str, int row, int sub);
void drawTabs(void);
void setupMenu(void);
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
void nullUI(void);
void subUI(void);
void uiDummy(void);
void enterMenu(int menuNr);
void openMenu(void);
void backOutOfMenu(void);
void closeMenu(void);
bool isMenuOpen(void);

void exitEmulator(void);

void uiNullDefault(void);
void uiYesNo(void);

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

void autoPauseGameSet();
void autoStateSet();
void autoSettingsSet();
void autoNVRAMSet();
void saveNVRAMSet();
void debugTextSet();
void sleepSet();
void powerSaveSet();
void screenSwapSet();
void autoASet();
void autoBSet();
void speedSet();
void flickSet();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUMENU_HEADER
