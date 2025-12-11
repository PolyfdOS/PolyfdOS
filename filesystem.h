#ifndef INCLUDE_FILESYSTEM_H
#define INCLUDE_FILESYSTEM_H

#define MAX_FILES 50
#define MAX_FILENAME 64
#define MAX_FILE_CONTENT 2048

/** File structure */
struct file {
    char filename[MAX_FILENAME];
    char directory[MAX_FILENAME];
    char content[MAX_FILE_CONTENT];
    int size;
    int in_use;
    int is_directory;  /* 1 if directory, 0 if file */
};

/** fs_init:
 *  Initialize the filesystem
 */
void fs_init(void);

/** fs_create:
 *  Create a new file
 *
 *  @param filepath  Full path to file
 *  @param content   File content
 *  @param size      Content size
 *  @return          0 on success, -1 on error
 */
int fs_create(const char *filepath, const char *content, int size);

/** fs_read:
 *  Read a file
 *
 *  @param filepath  Full path to file
 *  @param buffer    Buffer to read into
 *  @param max_size  Maximum size to read
 *  @return          Number of bytes read, -1 on error
 */
int fs_read(const char *filepath, char *buffer, int max_size);

/** fs_delete:
 *  Delete a file
 *
 *  @param filepath  Full path to file
 *  @return          0 on success, -1 on error
 */
int fs_delete(const char *filepath);

/** fs_exists:
 *  Check if a file exists
 *
 *  @param filepath  Full path to file
 *  @return          1 if exists, 0 if not
 */
int fs_exists(const char *filepath);

/** fs_list_directory:
 *  List files in a directory
 *
 *  @param directory  Directory path
 *  @param callback   Callback function for each file
 */
void fs_list_directory(const char *directory, void (*callback)(const char *filename));

/** fs_mkdir:
 *  Create a directory
 *
 *  @param dirpath  Full path to directory
 *  @return         0 on success, -1 on error
 */
int fs_mkdir(const char *dirpath);

/** fs_is_directory:
 *  Check if path is a directory
 *
 *  @param filepath  Full path
 *  @return          1 if directory, 0 if not
 */
int fs_is_directory(const char *filepath);

#endif /* INCLUDE_FILESYSTEM_H */