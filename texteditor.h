#ifndef INCLUDE_TEXTEDITOR_H
#define INCLUDE_TEXTEDITOR_H

/** texteditor_open:
 *  Opens the text editor with an optional filename
 *
 *  @param filename  The file to open (NULL for new file)
 *  @param current_dir  The current directory path
 */
void texteditor_open(const char *filename, const char *current_dir);

#endif /* INCLUDE_TEXTEDITOR_H */