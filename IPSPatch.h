#ifndef IPSPATCH_HEADER
#define IPSPATCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Loads an IPS file and applies it to the rom.
 * @param  *rom: pointer to rom.
 * @param  *fileName: Name of ips file to use.
 * @param  size: The size of the rom.
 * @return error code.
 */
int patchRom(void *rom, const char *fileName, const int size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IPSPATCH_HEADER
