#include "shell.h"
#include "fb.h"
#include "keyboard.h"
#include "serial.h"
#include "snake.h"

#define COMMAND_BUFFER_SIZE 256

static char command_buffer[COMMAND_BUFFER_SIZE];
static unsigned int buffer_index = 0;

/** strcmp:
 *  Simple string comparison function
 *
 *  @param s1  First string
 *  @param s2  Second string
 *  @return    0 if equal, non-zero otherwise
 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/** shell_clear_command:
 *  Clears the screen
 */
void shell_clear_command(void)
{
    fb_clear();
}

/** shell_help_command:
 *  Displays help information
 */
void shell_help_command(void)
{
    fb_puts("Available commands:\n");
    fb_puts("  help  - Display this help message\n");
    fb_puts("  clear - Clear the screen\n");
    fb_puts("  echo  - Echo back your text\n");
    fb_puts("  about - Display OS information\n");
    fb_puts("  play  - Play Snake game!\n");
}

/** shell_echo_command:
 *  Echoes back the command arguments
 *
 *  @param args  The arguments to echo
 */
void shell_echo_command(char *args)
{
    fb_puts(args);
    fb_puts("\n");
}

/** shell_about_command:
 *  Displays information about the OS
 */
void shell_about_command(void)
{
    fb_puts("polyfdOS v1.0 - Moroccan x86 Operating System\n");
    fb_puts("A minimal OS kernel built from scratch\n");
    fb_puts("Built following 'The little book about OS development'\n");
    fb_puts("Developer - Synthos (polyfdos)\n");
    fb_puts("Organization - Daftyon\n");
    fb_puts("Location - Morocco\n");
    fb_puts("\n");
    fb_puts("Developer Message -\n");
    fb_puts("As a full-stack developer, I work with high-level languages\n");
    fb_puts("like JavaScript, Dart, and Go. This project is my journey\n");
    fb_puts("into low-level systems programming. It's challenging and\n");
    fb_puts("rewarding - a true learning experience from Morocco!\n");
}

/** shell_play_command:
 *  Launches the Snake game
 */
void shell_play_command(void)
{
    snake_game();
    /* After game ends, redraw shell */
    fb_clear();
    fb_puts("Welcome back to polyfdOS!\n");
    fb_puts("Type 'help' for available commands.\n\n");
}

/** shell_execute_command:
 *  Executes a command
 */
void shell_execute_command(void)
{
    /* Null-terminate the command */
    command_buffer[buffer_index] = '\0';
    
    /* Skip empty commands */
    if (buffer_index == 0) {
        fb_puts("> ");
        return;
    }
    
    /* Parse command and arguments */
    char *cmd = command_buffer;
    char *args = command_buffer;
    
    /* Find first space to separate command from arguments */
    while (*args && *args != ' ') {
        args++;
    }
    
    if (*args == ' ') {
        *args = '\0';
        args++;
    }
    
    /* Execute command */
    if (strcmp(cmd, "help") == 0) {
        shell_help_command();
    } else if (strcmp(cmd, "clear") == 0) {
        shell_clear_command();
    } else if (strcmp(cmd, "echo") == 0) {
        shell_echo_command(args);
    } else if (strcmp(cmd, "about") == 0) {
        shell_about_command();
    } else if (strcmp(cmd, "play") == 0) {
        shell_play_command();
    } else {
        fb_puts("Unknown command: ");
        fb_puts(cmd);
        fb_puts("\nType 'help' for available commands.\n");
    }
    
    /* Reset buffer and show prompt */
    buffer_index = 0;
    fb_puts("> ");
}

/** shell_init:
 *  Initializes the shell
 */
void shell_init(void)
{
    buffer_index = 0;
    fb_clear();
    
    /* ASCII Art Logo */
    fb_puts(" ____       _        __     _  ___  ____  \n");
    fb_puts("|  _ \\ ___ | |_   _ / _| __| |/ _ \\/ ___| \n");
    fb_puts("| |_) / _ \\| | | | | |_ / _` | | | \\___ \\ \n");
    fb_puts("|  __/ (_) | | |_| |  _| (_| | |_| |___) |\n");
    fb_puts("|_|   \\___/|_|\\__, |_|  \\__,_|\\___/|____/ \n");
    fb_puts("              |___/                       \n");
    fb_puts("\n");
    fb_puts("===============================================\n");
    fb_puts("   Moroccan x86 Operating System v1.0\n");
    fb_puts("   Developer: Synthos | Organization: Daftyon\n");
    fb_puts("===============================================\n\n");
    fb_puts("Type 'help' for available commands.\n\n");
    fb_puts("> ");
}

/** shell_update:
 *  Updates the shell (call this in main loop)
 */
void shell_update(void)
{
    char c = keyboard_get_char();
    
    if (c != 0) {
        serial_write("GOT CHAR: ", 10);
        serial_write(&c, 1);
        serial_write("\n", 1);
        
        if (c == '\n') {
            /* Execute command */
            fb_putc('\n');
            shell_execute_command();
        } else if (c == '\b') {
            /* Handle backspace */
            if (buffer_index > 0) {
                buffer_index--;
                fb_putc('\b');
            }
        } else if (buffer_index < COMMAND_BUFFER_SIZE - 1) {
            /* Add character to buffer */
            command_buffer[buffer_index] = c;
            buffer_index++;
            fb_putc(c);
        }
    }
}