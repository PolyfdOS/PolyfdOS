/**
 * filemanager.h - File Management Commands
 * 
 * Linux-like file operations:
 * - mkdir: Create directories
 * - rmdir: Remove empty directories
 * - rm: Remove files
 * - mv: Move/rename files
 * - cp: Copy files
 * - touch: Create empty files
 */

#ifndef INCLUDE_FILEMANAGER_H
#define INCLUDE_FILEMANAGER_H

/** mkdir_command:
 *  Create a new directory
 *  Usage: mkdir <dirname>
 */
void mkdir_command(char *args, const char *current_dir);

/** rmdir_command:
 *  Remove an empty directory
 *  Usage: rmdir <dirname>
 */
void rmdir_command(char *args, const char *current_dir);

/** rm_command:
 *  Remove a file
 *  Usage: rm <filename>
 */
void rm_command(char *args, const char *current_dir);

/** mv_command:
 *  Move or rename a file/directory
 *  Usage: mv <source> <destination>
 */
void mv_command(char *args, const char *current_dir);

/** cp_command:
 *  Copy a file
 *  Usage: cp <source> <destination>
 */
void cp_command(char *args, const char *current_dir);

/** touch_command:
 *  Create an empty file or update timestamp
 *  Usage: touch <filename>
 */
void touch_command(char *args, const char *current_dir);

#endif /* INCLUDE_FILEMANAGER_H */
