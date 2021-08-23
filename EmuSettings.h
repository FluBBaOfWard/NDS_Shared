#ifndef EMUSETTINGS_HEADER
#define EMUSETTINGS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define AUTOPAUSE_EMULATION	(1<<0)
#define POWER_SAVE_MENU		(1<<1)
#define AUTOLOAD_STATE		(1<<2)
#define AUTOSLEEP_5MIN		(0<<4)
#define AUTOSLEEP_10MIN		(1<<4)
#define AUTOSLEEP_30MIN		(2<<4)
#define AUTOSLEEP_OFF		(3<<4)
#define EMUSPEED_NORMAL		(0<<6)
#define EMUSPEED_2X			(1<<6)
#define EMUSPEED_MAX		(2<<6)
#define EMUSPEED_HALF		(3<<6)
#define MAIN_ON_BOTTOM		(1<<8)
#define AUTOSAVE_SETTINGS	(1<<9)
#define AUTOLOAD_NVRAM		(1<<10)
#define AUTOSAVE_NVRAM		(1<<11)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUSETTINGS_HEADER
