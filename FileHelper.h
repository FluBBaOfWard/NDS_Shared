#ifndef FILEHELPER_HEADER
#define FILEHELPER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define FILENAME_MAX_LENGTH (128)
#define FILEPATH_MAX_LENGTH (256)

extern int fatAvailable;

extern char currentFilename[FILENAME_MAX_LENGTH];
extern char currentDir[FILEPATH_MAX_LENGTH];
extern const char *const spinner[4];

int initFileHelper();
void drawSpinner(void);

/**
 * Loads data from a (zip)file into the destination in memory.
 * @param  *dest: Destination where to put (uncompressed) data from file.
 * @param  *fileName: Name of file to load, if it is a zip file it will uncompress the first allowed file in the zip.
 * @param  maxSize: The maximum size of destination.
 * @return File size (uncompressed).
 */
int loadROM(void *dest, const char *fileName, const int maxSize);

/**
 * Loads a saved state file for the currently running game.
 * Handles filename creation, file opening/closing, memory handling, error output.
 * How the state is put into the device should be handled by a unpackState function.
 * @param  *folderName: Name of the folder where state is loaded from.
 * @return True if there was an error.
 */
bool loadDeviceState(const char *folderName);

/**
 * Saves a state file for the currently running game.
 * Handles filename creation, file opening/closing, memory handling, error output.
 * How the state gets from the device should be handled by a packState function.
 * @param  *folderName: Name of the folder where state is saved to.
 * @return True if there was an error.
 */
bool saveDeviceState(const char *folderName);

/**
 * Tries to find the specified folder, either at root level or under a "data" folder.
 * This also changes the currently selected directory of the file system.
 * @param  *folderName: Name of the folder to find.
 * @return Error code, 0 if no error.
 */
int findFolder(const char *folderName);

/**
 * Gets the last dot and the characters after, in the fileName.
 * @param  *dest: Where to put the result. Must be (at least) 8 bytes.
 * @param  *fileName: The file name to handle.
 */
void getFileExtension(char *dest, const char *fileName);

/**
 * Sets or replaces the filename extension.
 * @param  *dest: The destination where fileName plus extension ends up, can be the same as fileName.
 * @param  *fileName: The original file name.
 * @param  *newExt: File extension to set (including leading dot).
 * @param  dstSize: Total size of destination buffer.
 */
void setFileExtension(char *dest, const char *fileName, const char *newExt, int dstSize);

/**
 * Truncates a filename to fit destination but always includes the filename extension.
 * @param  *dest: The destination where fileName plus extension ends up, can be the same as fileName.
 * @param  *fileName: The original file name with extension.
 * @param  dstSize: Total size of destination buffer.
 */
void truncateFileName(char *dest, const char *fileName, int dstSize);

/**
 * A file browser that shows files according to the file types specified.
 * @param  *fileTypes: All file types that are valid (".txt.bin.rom.zip", etc).
 * @return The selected files name, or NULL if none selected.
 */
const char *browseForFileType(const char *fileTypes);

/**
 * Inits the directory structs, use browseAddFilename to add entries
 * that later can be browsed.
 * @param dirName: Label at the top of the screen.
 */
void initBrowse(const char *dirName);

/**
 * A file browser that shows files in the dirEntries.
 * @return The selected files name, or NULL if none selected.
 */
const char *browseDirectory();

/**
 * Gets the file name at the specified position.
 * @param  **dirEntries: Directory entries.
 * @param  pos: Which filename to get.
 * @return Selected file name or NULL if out of bounds.
 */
const char *directoryStringFromPos(char **dirEntries, int pos);

const char *browseAddFilename(const char *fileName);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FILEHELPER_HEADER
