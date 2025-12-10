#include "shell.h"
#include "fb.h"
#include "keyboard.h"
#include "serial.h"
#include "snake.h"
#include "io.h"

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

/** strstr:
 *  Simple string search function
 *
 *  @param haystack  String to search in
 *  @param needle    String to search for
 *  @return          Pointer to first occurrence or 0
 */
char* strstr(const char *haystack, const char *needle)
{
    if (!*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        
        if (!*n) return (char*)haystack;
    }
    
    return 0;
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
    fb_puts("  help     - Display this help message\n");
    fb_puts("  clear    - Clear the screen\n");
    fb_puts("  echo     - Echo back your text\n");
    fb_puts("  about    - Display OS information\n");
    fb_puts("  play     - Play Snake game!\n");
    fb_puts("  download - Download file from internet\n");
    fb_puts("  ls       - List downloaded files\n");
    fb_puts("  sudo     - Execute command with root privileges\n");
    fb_puts("  reboot   - Reboot the system\n");
    fb_puts("  halt     - Shutdown the system\n");
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
    fb_puts("polyfdOS v1.2 - Moroccan x86 Operating System\n");
    fb_puts("A minimal OS kernel built from scratch\n");
    fb_puts("Built following 'The little book about OS development'\n");
    fb_puts("Developer - PolyfdoR (polyfdos)\n");
    fb_puts("Organization - Daftyon\n");
    fb_puts("Location - Morocco\n");
    fb_puts("\n");
    fb_puts("Features:\n");
    fb_puts("  - Protected mode (32-bit)\n");
    fb_puts("  - Interrupt handling (GDT/IDT/PIC)\n");
    fb_puts("  - Keyboard driver (PS/2)\n");
    fb_puts("  - Framebuffer VGA text mode\n");
    fb_puts("  - Serial port debugging\n");
    fb_puts("  - Interactive shell\n");
    fb_puts("  - Network simulation\n");
}

/** shell_download_command:
 *  Simulates downloading a file from the internet
 *  In a real implementation, this would use a network stack
 *
 *  @param args  The URL or package name to download
 */
void shell_download_command(char *args)
{
    // Simple download simulation
    if (args[0] == '\0') {
        fb_puts("Usage: download <package-name>\n");
        fb_puts("Example: download python\n");
        fb_puts("         download nodejs\n");
        fb_puts("         download gcc\n");
        return;
    }
    
    fb_puts("Initializing network interface...\n");
    fb_puts("[OK] Network interface eth0 up\n");
    fb_puts("\n");
    
    fb_puts("Resolving package repository...\n");
    fb_puts("[OK] Connected to packages.polyfdos.ma\n");
    fb_puts("\n");
    
    fb_puts("Searching for package: ");
    fb_puts(args);
    fb_puts("\n");
    
    // Simulate different packages
    if (strstr(args, "python")) {
        fb_puts("[FOUND] python-3.12.0-polyfdos-x86.tar.gz\n");
        fb_puts("Size: 45.2 MB\n");
        fb_puts("\n");
        fb_puts("Downloading");
        
        // Simulate download progress
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            // Simple delay
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        
        fb_puts(" [100%]\n");
        fb_puts("\n");
        fb_puts("[OK] Downloaded python-3.12.0\n");
        fb_puts("[OK] Verifying checksum...\n");
        fb_puts("[OK] Installing to /usr/local/python3\n");
        fb_puts("\n");
        fb_puts("Installation complete!\n");
        fb_puts("Run 'python3 --version' to verify installation.\n");
        
    } else if (strstr(args, "nodejs") || strstr(args, "node")) {
        fb_puts("[FOUND] nodejs-20.10.0-polyfdos-x86.tar.gz\n");
        fb_puts("Size: 38.7 MB\n");
        fb_puts("\n");
        fb_puts("Downloading");
        
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        
        fb_puts(" [100%]\n");
        fb_puts("\n");
        fb_puts("[OK] Downloaded nodejs-20.10.0\n");
        fb_puts("[OK] Installing to /usr/local/nodejs\n");
        fb_puts("\n");
        fb_puts("Installation complete!\n");
        fb_puts("Run 'node --version' to verify installation.\n");
        
    } else if (strstr(args, "gcc")) {
        fb_puts("[FOUND] gcc-13.2.0-polyfdos-x86.tar.gz\n");
        fb_puts("Size: 102.5 MB\n");
        fb_puts("\n");
        fb_puts("Downloading");
        
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        
        fb_puts(" [100%]\n");
        fb_puts("\n");
        fb_puts("[OK] Downloaded gcc-13.2.0\n");
        fb_puts("[OK] Installing to /usr/local/gcc\n");
        fb_puts("\n");
        fb_puts("Installation complete!\n");
        fb_puts("Run 'gcc --version' to verify installation.\n");
        
    } else if (strstr(args, "rust")) {
        fb_puts("[FOUND] rust-1.75.0-polyfdos-x86.tar.gz\n");
        fb_puts("Size: 156.8 MB\n");
        fb_puts("\n");
        fb_puts("Downloading");
        
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        
        fb_puts(" [100%]\n");
        fb_puts("\n");
        fb_puts("[OK] Downloaded rust-1.75.0\n");
        fb_puts("[OK] Installing to /usr/local/rust\n");
        fb_puts("\n");
        fb_puts("Installation complete!\n");
        fb_puts("Run 'rustc --version' to verify installation.\n");
        
    } else {
        fb_puts("[ERROR] Package not found in repository\n");
        fb_puts("\n");
        fb_puts("Available packages:\n");
        fb_puts("  - python (Python 3.12.0)\n");
        fb_puts("  - nodejs (Node.js 20.10.0)\n");
        fb_puts("  - gcc (GCC 13.2.0)\n");
        fb_puts("  - rust (Rust 1.75.0)\n");
        fb_puts("\n");
        fb_puts("Note: Full network stack coming in v2.0!\n");
    }
}

/** shell_ls_command:
 *  Lists downloaded/installed packages
 */
void shell_ls_command(void)
{
    fb_puts("Installed packages:\n");
    fb_puts("\n");
    fb_puts("System packages:\n");
    fb_puts("  /bin/shell        - polyfdOS Shell v1.2\n");
    fb_puts("  /bin/snake        - Snake Game\n");
    fb_puts("\n");
    fb_puts("User packages:\n");
    fb_puts("  (use 'download' to install packages)\n");
    fb_puts("\n");
    fb_puts("Disk usage: 26 KB / 512 MB\n");
}

/** shell_sudo_command:
 *  Executes a command with root privileges (fun easter egg)
 *
 *  @param args  The command to execute
 */
void shell_sudo_command(char *args)
{
    fb_puts("[sudo] password for PolyfdoR: ");
    fb_puts("\n");
    fb_puts("Authenticating...\n");
    fb_puts("Access granted! You are now root.\n");
    fb_puts("Running: ");
    fb_puts(args);
    fb_puts("\n");
    fb_puts("Command executed successfully.\n");
}

/** shell_reboot_command:
 *  Reboots the system
 */
void shell_reboot_command(void)
{
    fb_puts("Rebooting polyfdOS...\n");
    fb_puts("Goodbye!\n");
    /* Triple fault to reboot - write to port 0x64 */
    __asm__ volatile("cli");
    unsigned char temp;
    do {
        temp = inb(0x64);
        if (temp & 1) {
            inb(0x60);
        }
    } while (temp & 2);
    outb(0x64, 0xFE);
    /* Hang if reboot fails */
    while(1);
}

/** shell_halt_command:
 *  Halts the system
 */
void shell_halt_command(void)
{
    fb_puts("Halting polyfdOS...\n");
    fb_puts("System halted. It is now safe to close the window.\n");
    __asm__ volatile("cli; hlt");
    while(1);
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
    } else if (strcmp(cmd, "download") == 0) {
        shell_download_command(args);
    } else if (strcmp(cmd, "ls") == 0) {
        shell_ls_command();
    } else if (strcmp(cmd, "sudo") == 0) {
        shell_sudo_command(args);
    } else if (strcmp(cmd, "reboot") == 0) {
        shell_reboot_command();
    } else if (strcmp(cmd, "halt") == 0) {
        shell_halt_command();
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
    fb_puts("   Moroccan x86 Operating System v1.2\n");
    fb_puts("   Developer: PolyfdoR | Organization: Daftyon\n");
    fb_puts("===============================================\n\n");
    fb_puts("Type 'help' for available commands.\n");
    fb_puts("Try 'download python' to install packages!\n\n");
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