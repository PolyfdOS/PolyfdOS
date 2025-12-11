#include "texteditor.h"
#include "fb.h"
#include "keyboard.h"
#include "serial.h"
#include "filesystem.h"

#define MAX_LINES 15
#define MAX_LINE_LENGTH 75

/* Text buffer */
static char text_buffer[MAX_LINES][MAX_LINE_LENGTH];
static int current_line = 0;
static int current_col = 0;
static int num_lines = 1;
static char current_filename[64] = "";
static int modified = 0;

/** clear_buffer */
static void clear_buffer(void)
{
    int i, j;
    for (i = 0; i < MAX_LINES; i++) {
        for (j = 0; j < MAX_LINE_LENGTH; j++) {
            text_buffer[i][j] = '\0';
        }
    }
    current_line = 0;
    current_col = 0;
    num_lines = 1;
    modified = 0;
}

/** draw_line_num */
static void draw_line_num(int line_num)
{
    if (line_num < 10) {
        fb_putc(' ');
        fb_putc('0' + line_num);
    } else {
        fb_putc('0' + (line_num / 10));
        fb_putc('0' + (line_num % 10));
    }
    fb_puts(" | ");
}

/** draw_editor */
static void draw_editor(void)
{
    fb_clear();
    
    /* Title */
    fb_puts("================================================================\n");
    fb_puts("              TextEditor v1.0 - polyfdOS\n");
    fb_puts("================================================================\n");
    
    if (current_filename[0] != '\0') {
        fb_puts("File: ");
        fb_puts(current_filename);
        if (modified) fb_puts(" [Modified]");
        fb_puts("\n");
    } else {
        fb_puts("File: <new file>");
        if (modified) fb_puts(" [Modified]");
        fb_puts("\n");
    }
    
    fb_puts("----------------------------------------------------------------\n");
    
    /* Display text buffer */
    int i;
    for (i = 0; i < 13; i++) {
        draw_line_num(i + 1);
        if (i < num_lines) {
            fb_puts(text_buffer[i]);
        }
        fb_puts("\n");
    }
    
    /* Status bar */
    fb_puts("----------------------------------------------------------------\n");
    fb_puts("ESC/X=Exit Enter=NewLine Backspace=Delete 'r'=Remove line\n");
    fb_puts("'n'=New line 's'=Save 'c'=Clear all\n");
    fb_puts("Line: ");
    
    if (current_line + 1 < 10) {
        fb_putc('0' + (current_line + 1));
    } else {
        fb_putc('0' + ((current_line + 1) / 10));
        fb_putc('0' + ((current_line + 1) % 10));
    }
    
    fb_puts("/");
    
    if (num_lines < 10) {
        fb_putc('0' + num_lines);
    } else {
        fb_putc('0' + (num_lines / 10));
        fb_putc('0' + (num_lines % 10));
    }
    
    fb_puts("  Col: ");
    
    if (current_col + 1 < 10) {
        fb_putc('0' + (current_col + 1));
    } else {
        fb_putc('0' + ((current_col + 1) / 10));
        fb_putc('0' + ((current_col + 1) % 10));
    }
    
    fb_puts("\n");
}

/** save_file */
static void save_file(void)
{
    if (current_filename[0] == '\0') {
        fb_clear();
        fb_puts("\n\nSave file as: ");
        
        char filename[64];
        int i = 0;
        
        while (1) {
            char c = keyboard_get_char();
            if (c != 0) {
                if (c == '\n') {
                    filename[i] = '\0';
                    break;
                } else if (c == '\b' && i > 0) {
                    i--;
                    fb_putc('\b');
                } else if (c != '\b' && c >= 32 && c <= 126 && i < 63) {
                    filename[i++] = c;
                    fb_putc(c);
                }
            }
        }
        
        if (i > 0) {
            int j;
            for (j = 0; j < i && j < 63; j++) {
                current_filename[j] = filename[j];
            }
            current_filename[j] = '\0';
        }
    }
    
    /* Show save confirmation */
    fb_clear();
    fb_puts("\n\nSaving file: ");
    fb_puts(current_filename);
    fb_puts("\n\n");
    
    /* Build file content from text_buffer */
    char file_content[MAX_FILE_CONTENT];
    int content_pos = 0;
    int i, j;
    
    for (i = 0; i < num_lines && content_pos < MAX_FILE_CONTENT - 1; i++) {
        /* Copy line */
        j = 0;
        while (text_buffer[i][j] != '\0' && content_pos < MAX_FILE_CONTENT - 1) {
            file_content[content_pos++] = text_buffer[i][j++];
        }
        /* Add newline */
        if (content_pos < MAX_FILE_CONTENT - 1) {
            file_content[content_pos++] = '\n';
        }
    }
    file_content[content_pos] = '\0';
    
    /* Actually save to filesystem */
    int result = fs_create(current_filename, file_content, content_pos);
    
    /* Show content preview */
    for (i = 0; i < num_lines && i < 10; i++) {
        fb_puts(text_buffer[i]);
        fb_puts("\n");
    }
    
    fb_puts("\n");
    if (result == 0) {
        fb_puts("[OK] File saved to filesystem!\n");
    } else {
        fb_puts("[ERROR] Could not save file (filesystem full?)\n");
    }
    fb_puts("Press any key to continue...\n");
    
    modified = 0;
    
    /* Wait for key */
    while (keyboard_get_char() == 0);
    
    draw_editor();
}

/** delete_line - Remove current line */
static void delete_line(void)
{
    if (num_lines <= 1) {
        /* Clear the only line */
        int i;
        for (i = 0; i < MAX_LINE_LENGTH; i++) {
            text_buffer[0][i] = '\0';
        }
        current_col = 0;
    } else {
        /* Shift all lines up */
        int i, j;
        for (i = current_line; i < num_lines - 1; i++) {
            for (j = 0; j < MAX_LINE_LENGTH; j++) {
                text_buffer[i][j] = text_buffer[i + 1][j];
            }
        }
        
        /* Clear the last line */
        for (j = 0; j < MAX_LINE_LENGTH; j++) {
            text_buffer[num_lines - 1][j] = '\0';
        }
        
        num_lines--;
        
        /* Adjust cursor */
        if (current_line >= num_lines) {
            current_line = num_lines - 1;
        }
        current_col = 0;
    }
    
    modified = 1;
    draw_editor();
}

/** insert_line - Insert new line at cursor */
static void insert_line(void)
{
    if (num_lines >= MAX_LINES) {
        return;
    }
    
    /* Shift all lines down */
    int i, j;
    for (i = num_lines; i > current_line + 1; i--) {
        for (j = 0; j < MAX_LINE_LENGTH; j++) {
            text_buffer[i][j] = text_buffer[i - 1][j];
        }
    }
    
    /* Clear the new line */
    for (j = 0; j < MAX_LINE_LENGTH; j++) {
        text_buffer[current_line + 1][j] = '\0';
    }
    
    num_lines++;
    current_line++;
    current_col = 0;
    modified = 1;
    draw_editor();
}

/** clear_all - Clear all text */
static void clear_all(void)
{
    fb_clear();
    fb_puts("\n\nClear all text? (y/n): ");
    
    while (1) {
        char c = keyboard_get_char();
        if (c == 'y' || c == 'Y') {
            clear_buffer();
            current_filename[0] = '\0';
            draw_editor();
            break;
        } else if (c == 'n' || c == 'N' || c == 27) {
            draw_editor();
            break;
        }
    }
}

/** delete_word - Delete word at cursor */
static void delete_word(void)
{
    int line_len = 0;
    while (text_buffer[current_line][line_len] != '\0') {
        line_len++;
    }
    
    /* Find end of current word */
    int end = current_col;
    while (end < line_len && text_buffer[current_line][end] != ' ' && 
           text_buffer[current_line][end] != '\0') {
        end++;
    }
    
    /* Shift characters left */
    int shift = end - current_col;
    if (shift > 0) {
        int i;
        for (i = current_col; i < line_len; i++) {
            if (i + shift < line_len) {
                text_buffer[current_line][i] = text_buffer[current_line][i + shift];
            } else {
                text_buffer[current_line][i] = '\0';
            }
        }
        modified = 1;
        draw_editor();
    }
}

/** texteditor_open */
void texteditor_open(const char *filename, const char *current_dir)
{
    clear_buffer();
    
    if (filename != 0 && filename[0] != '\0') {
        /* Build full path: directory + filename */
        int i = 0;
        int j = 0;
        
        /* Copy current directory */
        if (current_dir != 0) {
            while (current_dir[i] != '\0' && i < 50) {
                current_filename[i] = current_dir[i];
                i++;
            }
            
            /* Add slash if not root */
            if (i > 0 && current_filename[i-1] != '/') {
                current_filename[i++] = '/';
            }
        }
        
        /* Copy filename */
        while (filename[j] != '\0' && i < 63) {
            current_filename[i++] = filename[j++];
        }
        current_filename[i] = '\0';
        
        /* Try to load existing file from filesystem */
        char file_content[MAX_FILE_CONTENT];
        int bytes_read = fs_read(current_filename, file_content, MAX_FILE_CONTENT - 1);
        
        if (bytes_read > 0) {
            /* File exists! Load its content */
            file_content[bytes_read] = '\0';
            int line = 0;
            int col = 0;
            
            for (i = 0; i < bytes_read && line < MAX_LINES; i++) {
                if (file_content[i] == '\n') {
                    text_buffer[line][col] = '\0';
                    line++;
                    col = 0;
                } else if (col < MAX_LINE_LENGTH - 1) {
                    text_buffer[line][col++] = file_content[i];
                }
            }
            
            if (col > 0) {
                text_buffer[line][col] = '\0';
                line++;
            }
            
            num_lines = line > 0 ? line : 1;
        } else {
            /* New file - show sample content */
            char *msg = "Welcome to TextEditor!\nType to edit...\n";
            int line = 0;
            int col = 0;
            for (i = 0; msg[i] != '\0' && line < MAX_LINES; i++) {
                if (msg[i] == '\n') {
                    text_buffer[line][col] = '\0';
                    line++;
                    col = 0;
                } else if (col < MAX_LINE_LENGTH - 1) {
                    text_buffer[line][col++] = msg[i];
                }
            }
            if (col > 0) {
                text_buffer[line][col] = '\0';
                line++;
            }
            num_lines = line > 0 ? line : 1;
        }
    }
    
    draw_editor();
    
    /* Show help */
    fb_clear();
    fb_puts("\n\n");
    fb_puts("====== TextEditor Commands ======\n\n");
    fb_puts("ESC or X   - Exit editor\n");
    fb_puts("Enter      - New line\n");
    fb_puts("Backspace  - Delete character\n");
    fb_puts("'s'        - Save file\n");
    fb_puts("'r'        - Remove current line\n");
    fb_puts("'n'        - Insert new line\n");
    fb_puts("'c'        - Clear all text\n");
    fb_puts("'d'        - Delete word\n");
    fb_puts("\n");
    fb_puts("Press any key to start editing...\n");
    
    /* Wait for key */
    while (keyboard_get_char() == 0);
    
    draw_editor();
    
    /* Editor main loop */
    int running = 1;
    while (running) {
        char c = keyboard_get_char();
        
        if (c != 0) {
            if (c == 27 || c == 'x' || c == 'X') {  /* ESC or X key - Exit */
                if (modified) {
                    fb_clear();
                    fb_puts("\n\nFile has unsaved changes!\n\n");
                    fb_puts("Press 's' to save and exit\n");
                    fb_puts("Press 'q' to quit without saving\n");
                    fb_puts("Press any other key to cancel\n");
                    
                    while (1) {
                        char confirm = keyboard_get_char();
                        if (confirm == 's' || confirm == 'S') {
                            save_file();
                            running = 0;
                            break;
                        } else if (confirm == 'q' || confirm == 'Q') {
                            running = 0;
                            break;
                        } else if (confirm != 0) {
                            draw_editor();
                            break;
                        }
                    }
                } else {
                    running = 0;
                }
            } else if (c == 's' || c == 'S') {  /* Save */
                save_file();
            } else if (c == 'r' || c == 'R') {  /* Remove line */
                delete_line();
            } else if (c == 'n' || c == 'N') {  /* New line */
                insert_line();
            } else if (c == 'c' || c == 'C') {  /* Clear all */
                clear_all();
            } else if (c == 'd' || c == 'D') {  /* Delete word */
                delete_word();
            } else if (c == '\n') {  /* Enter - New line */
                if (num_lines < MAX_LINES) {
                    current_line++;
                    if (current_line >= num_lines) {
                        num_lines = current_line + 1;
                    }
                    current_col = 0;
                    modified = 1;
                    draw_editor();
                }
            } else if (c == '\b') {  /* Backspace */
                if (current_col > 0) {
                    current_col--;
                    /* Shift characters left */
                    int i;
                    for (i = current_col; text_buffer[current_line][i] != '\0'; i++) {
                        text_buffer[current_line][i] = text_buffer[current_line][i + 1];
                    }
                    modified = 1;
                    draw_editor();
                } else if (current_line > 0) {
                    /* Backspace at start of line */
                    current_line--;
                    current_col = 0;
                    while (text_buffer[current_line][current_col] != '\0' && 
                           current_col < MAX_LINE_LENGTH - 1) {
                        current_col++;
                    }
                    draw_editor();
                }
            } else if (c >= 32 && c <= 126) {  /* Printable characters */
                if (current_col < MAX_LINE_LENGTH - 1) {
                    /* Insert character */
                    int i;
                    int line_len = 0;
                    while (text_buffer[current_line][line_len] != '\0' && 
                           line_len < MAX_LINE_LENGTH - 1) {
                        line_len++;
                    }
                    
                    /* Shift characters right */
                    if (line_len < MAX_LINE_LENGTH - 1) {
                        for (i = line_len; i > current_col; i--) {
                            text_buffer[current_line][i] = text_buffer[current_line][i - 1];
                        }
                        text_buffer[current_line][current_col] = c;
                        text_buffer[current_line][line_len + 1] = '\0';
                        current_col++;
                        modified = 1;
                        draw_editor();
                    }
                }
            }
        }
    }
    
    /* Exit editor */
    fb_clear();
}