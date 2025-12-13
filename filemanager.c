/**
 * filemanager.c - File Management Commands Implementation
 */

#include "filemanager.h"
#include "filesystem.h"
#include "fb.h"

/** Helper: Build full path from current directory and argument */
static void build_full_path(const char *current_dir, const char *arg, char *result)
{
    int i = 0;
    
    if (arg[0] == '/') {
        /* Absolute path */
        while (arg[i] && i < 255) {
            result[i] = arg[i];
            i++;
        }
        result[i] = '\0';
    } else {
        /* Relative path */
        const char *dir = current_dir;
        while (*dir && i < 200) {
            result[i++] = *dir++;
        }
        
        /* Add slash if not root */
        if (i > 0 && result[i-1] != '/') {
            result[i++] = '/';
        }
        
        /* Add argument */
        int j = 0;
        while (arg[j] && i < 255) {
            result[i++] = arg[j++];
        }
        result[i] = '\0';
    }
}

/** mkdir_command */
void mkdir_command(char *args, const char *current_dir)
{
    char fullpath[256];
    
    if (args[0] == '\0') {
        fb_puts("Usage: mkdir <directory>\n");
        fb_puts("Example: mkdir mydir\n");
        return;
    }
    
    /* Build full path */
    build_full_path(current_dir, args, fullpath);
    
    /* Check if already exists */
    if (fs_exists(fullpath)) {
        fb_puts("mkdir: cannot create directory '");
        fb_puts(args);
        fb_puts("': File exists\n");
        return;
    }
    
    /* Create directory */
    if (fs_mkdir(fullpath) == 0) {
        fb_puts("Created directory: ");
        fb_puts(args);
        fb_puts("\n");
    } else {
        fb_puts("mkdir: failed to create directory '");
        fb_puts(args);
        fb_puts("'\n");
    }
}

/** rmdir_command */
void rmdir_command(char *args, const char *current_dir)
{
    char fullpath[256];
    
    if (args[0] == '\0') {
        fb_puts("Usage: rmdir <directory>\n");
        fb_puts("Example: rmdir mydir\n");
        return;
    }
    
    /* Build full path */
    build_full_path(current_dir, args, fullpath);
    
    /* Check if exists and is directory */
    if (!fs_exists(fullpath)) {
        fb_puts("rmdir: failed to remove '");
        fb_puts(args);
        fb_puts("': No such directory\n");
        return;
    }
    
    if (!fs_is_directory(fullpath)) {
        fb_puts("rmdir: failed to remove '");
        fb_puts(args);
        fb_puts("': Not a directory\n");
        return;
    }
    
    /* Delete directory */
    if (fs_delete(fullpath) == 0) {
        fb_puts("Removed directory: ");
        fb_puts(args);
        fb_puts("\n");
    } else {
        fb_puts("rmdir: failed to remove '");
        fb_puts(args);
        fb_puts("': Directory not empty\n");
    }
}

/** rm_command */
void rm_command(char *args, const char *current_dir)
{
    char fullpath[256];
    
    if (args[0] == '\0') {
        fb_puts("Usage: rm <file>\n");
        fb_puts("Example: rm myfile.txt\n");
        return;
    }
    
    /* Build full path */
    build_full_path(current_dir, args, fullpath);
    
    /* Check if exists */
    if (!fs_exists(fullpath)) {
        fb_puts("rm: cannot remove '");
        fb_puts(args);
        fb_puts("': No such file\n");
        return;
    }
    
    /* Check if directory */
    if (fs_is_directory(fullpath)) {
        fb_puts("rm: cannot remove '");
        fb_puts(args);
        fb_puts("': Is a directory (use rmdir)\n");
        return;
    }
    
    /* Delete file */
    if (fs_delete(fullpath) == 0) {
        fb_puts("Removed: ");
        fb_puts(args);
        fb_puts("\n");
    } else {
        fb_puts("rm: failed to remove '");
        fb_puts(args);
        fb_puts("'\n");
    }
}

/** mv_command */
void mv_command(char *args, const char *current_dir)
{
    char source[256], dest[256];
    char content[2048];
    int bytes_read;
    int i = 0, j = 0;
    
    if (args[0] == '\0') {
        fb_puts("Usage: mv <source> <destination>\n");
        fb_puts("Example: mv oldname.txt newname.txt\n");
        return;
    }
    
    /* Parse source and destination */
    /* Skip leading spaces */
    while (args[i] == ' ') i++;
    
    /* Get source */
    j = 0;
    while (args[i] && args[i] != ' ' && j < 200) {
        source[j++] = args[i++];
    }
    source[j] = '\0';
    
    /* Skip spaces */
    while (args[i] == ' ') i++;
    
    /* Get destination */
    j = 0;
    while (args[i] && j < 200) {
        dest[j++] = args[i++];
    }
    dest[j] = '\0';
    
    if (source[0] == '\0' || dest[0] == '\0') {
        fb_puts("Usage: mv <source> <destination>\n");
        return;
    }
    
    /* Build full paths */
    char source_path[256], dest_path[256];
    build_full_path(current_dir, source, source_path);
    build_full_path(current_dir, dest, dest_path);
    
    /* Check if source exists */
    if (!fs_exists(source_path)) {
        fb_puts("mv: cannot stat '");
        fb_puts(source);
        fb_puts("': No such file or directory\n");
        return;
    }
    
    /* Check if destination exists */
    if (fs_exists(dest_path)) {
        fb_puts("mv: cannot move to '");
        fb_puts(dest);
        fb_puts("': File exists\n");
        return;
    }
    
    /* If it's a directory, just rename it */
    if (fs_is_directory(source_path)) {
        /* Create new directory */
        if (fs_mkdir(dest_path) != 0) {
            fb_puts("mv: failed to create destination directory\n");
            return;
        }
        /* Delete old directory */
        fs_delete(source_path);
        fb_puts("Moved: ");
        fb_puts(source);
        fb_puts(" -> ");
        fb_puts(dest);
        fb_puts("\n");
        return;
    }
    
    /* For files: read, write to new location, delete old */
    bytes_read = fs_read(source_path, content, sizeof(content));
    
    if (bytes_read < 0) {
        fb_puts("mv: failed to read '");
        fb_puts(source);
        fb_puts("'\n");
        return;
    }
    
    /* Write to destination */
    if (fs_create(dest_path, content, bytes_read) != 0) {
        fb_puts("mv: failed to write to '");
        fb_puts(dest);
        fb_puts("'\n");
        return;
    }
    
    /* Delete source */
    fs_delete(source_path);
    
    fb_puts("Moved: ");
    fb_puts(source);
    fb_puts(" -> ");
    fb_puts(dest);
    fb_puts("\n");
}

/** cp_command */
void cp_command(char *args, const char *current_dir)
{
    char source[256], dest[256];
    char content[2048];
    int bytes_read;
    int i = 0, j = 0;
    
    if (args[0] == '\0') {
        fb_puts("Usage: cp <source> <destination>\n");
        fb_puts("Example: cp file.txt backup.txt\n");
        return;
    }
    
    /* Parse source and destination */
    while (args[i] == ' ') i++;
    
    j = 0;
    while (args[i] && args[i] != ' ' && j < 200) {
        source[j++] = args[i++];
    }
    source[j] = '\0';
    
    while (args[i] == ' ') i++;
    
    j = 0;
    while (args[i] && j < 200) {
        dest[j++] = args[i++];
    }
    dest[j] = '\0';
    
    if (source[0] == '\0' || dest[0] == '\0') {
        fb_puts("Usage: cp <source> <destination>\n");
        return;
    }
    
    /* Build full paths */
    char source_path[256], dest_path[256];
    build_full_path(current_dir, source, source_path);
    build_full_path(current_dir, dest, dest_path);
    
    /* Check if source exists */
    if (!fs_exists(source_path)) {
        fb_puts("cp: cannot stat '");
        fb_puts(source);
        fb_puts("': No such file\n");
        return;
    }
    
    /* Check if source is directory */
    if (fs_is_directory(source_path)) {
        fb_puts("cp: cannot copy directory '");
        fb_puts(source);
        fb_puts("' (use cp -r)\n");
        return;
    }
    
    /* Read source file */
    bytes_read = fs_read(source_path, content, sizeof(content));
    
    if (bytes_read < 0) {
        fb_puts("cp: failed to read '");
        fb_puts(source);
        fb_puts("'\n");
        return;
    }
    
    /* Write to destination */
    if (fs_create(dest_path, content, bytes_read) != 0) {
        fb_puts("cp: failed to write to '");
        fb_puts(dest);
        fb_puts("'\n");
        return;
    }
    
    fb_puts("Copied: ");
    fb_puts(source);
    fb_puts(" -> ");
    fb_puts(dest);
    fb_puts("\n");
}

/** touch_command */
void touch_command(char *args, const char *current_dir)
{
    char fullpath[256];
    
    if (args[0] == '\0') {
        fb_puts("Usage: touch <filename>\n");
        fb_puts("Example: touch newfile.txt\n");
        return;
    }
    
    /* Build full path */
    build_full_path(current_dir, args, fullpath);
    
    /* Check if already exists */
    if (fs_exists(fullpath)) {
        fb_puts("File already exists: ");
        fb_puts(args);
        fb_puts("\n");
        return;
    }
    
    /* Create empty file */
    if (fs_create(fullpath, "", 0) == 0) {
        fb_puts("Created: ");
        fb_puts(args);
        fb_puts("\n");
    } else {
        fb_puts("touch: failed to create '");
        fb_puts(args);
        fb_puts("'\n");
    }
}