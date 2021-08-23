#ifndef FILEHELPER_HEADER
#define FILEHELPER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define FILENAMEMAXLENGTH (128)
#define FILEPATHMAXLENGTH (256)

extern int fatAvailable;

extern char currentFilename[FILENAMEMAXLENGTH];
extern char currentDir[FILEPATHMAXLENGTH];
extern const char *const spinner[4];

int initFileHelper();
void drawSpinner(void);
/// returns (uncompressed) file size.

/**
 * Loads data from a (zip)file into the destination in memory.
 * @param  *dest: Destination where to put (uncompressed) data from file.
 * @param  *fileName: Name of file to load, if it is a zip file it will uncompress the first allowed file in the zip.
 * @param  maxSize: The maximum size of destination.
 * @return File size (uncompressed).
 */
int loadROM(void *dest, const char *fileName, const int maxSize);

/**
 * Tries to find the specified folder, either at root level or under a "data" folder.
 * This also changes the currently selected directory of the file system.
 * @param  *folderName: Name of the folder to find.
 * @return Error code, 0 if no error.
 */
int findFolder(const char *folderName);

/**
 * Finds the last characters after the dot in the filename.
 * @param  *dest: Where to put the result.
 * @param  *fileName: The file name to handle.
 */
void getFileExtension(char *dest, const char *fileName);

/**
 * A file browser that shows files according to the file types specified.
 * @param  *fileTypes: All file types that are valid (".txt.bin.rom.zip", etc).
 * @return The selected files name, or NULL if none selected.
 */
const char *browseForFileType(const char *fileTypes);

/**
 * Gets the file name at the specified position.
 * @param  **dirEntries: Directory entries.
 * @param  pos: Which filename to get.
 * @return Selected file name or NULL if out of bounds.
 */
const char *directoryStringFromPos(char **dirEntries, int pos);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FILEHELPER_HEADER
