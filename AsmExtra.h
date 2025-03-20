#ifndef ASMEXTRA_HEADER
#define ASMEXTRA_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/// The current FPS as calculated by the calculateFPS function.
extern char fpsText[8];

/**
 * Reads the Real Time Clock.
 * @return The time as ??SSMMHH, r1=??DDMMYY.
*/
int getTime(void);

/**
 * Converts a binary number to Binary Encoded Decimal format
 * @param  value: Value to convert.
 * @return Input value converted to BCD.
 */
int bin2BCD(int value);

/**
 * Generates a "random" number between 0 and maxValue.
 * @param  maxValue: How large the distribution of values are.
 * @return A new "random" number.
 */
int getRandomNumber(int maxValue);

/// Calculates fps, needs to be called _every_ frame, eg. from the VBlank interrupt.
void calculateFPS(void);

/// Set target FPS, used when displaying FPS.
void setTargetFPS(int lcdFPS);

/**
 * Converts 24bit RGB palette to 15bit BGR palette, while running gamma conversion on it.
 * @param  *dest: Destination of result, needs to be aligned to 16bit.
 * @param  *source: Source of RGB data, 8bit aligned.
 * @param  length: How many colors to convert.
 * @param  gamma: Gamma value between 1 & 4.
 */
void convertPalette(u16 *dest, const u8 *source, int length, int gamma);

/**
 * Outputs text to the console of an NDS/GBA emulator, this works at least in No$GBA.
 * @param  *text: The text to output, max number of chars = 80.
 */
void debugOutputToEmulator(const char *text);

/**
  * Enable Data caching for SLOT-2 ( region 3)
 */
void enableSlot2Cache(void);

/**
  * Disable Data caching for SLOT-2 ( region 3)
 */
void disableSlot2Cache(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ASMEXTRA_HEADER
