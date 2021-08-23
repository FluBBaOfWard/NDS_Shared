#ifndef CRC32_HEADER
#define CRC32_HEADER

#ifdef __cplusplus
extern "C" {
#endif

unsigned int crc32(unsigned int crc, const unsigned char *buf, int len);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CRC32_HEADER
