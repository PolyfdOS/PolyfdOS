#include "shell.h"
#include "fb.h"
#include "keyboard.h"
#include "serial.h"
#include "snake.h"
#include "io.h"

#define COMMAND_BUFFER_SIZE 256
#define MAX_PATH_LENGTH 128

static char command_buffer[COMMAND_BUFFER_SIZE];
static unsigned int buffer_index = 0;
static char current_directory[MAX_PATH_LENGTH] = "/";

/** strcmp:
 *  Simple string comparison function
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

/** strlen:
 *  Get string length
 */
int strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/** strcpy:
 *  Copy string
 */
void strcpy(char *dest, const char *src)
{
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/** strcat:
 *  Concatenate strings
 */
void strcat(char *dest, const char *src)
{
    while (*dest) dest++;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/** int_to_str:
 *  Convert integer to string
 */
static void int_to_str(int num, char *str)
{
    int i = 0;
    int is_negative = 0;
    
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    while (num > 0) {
        str[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    /* Reverse */
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

/** cpuid:
 *  Execute CPUID instruction
 */
void cpuid(unsigned int code, unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d)
{
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

/** get_cpu_vendor:
 *  Get CPU vendor string
 */
void get_cpu_vendor(char *vendor)
{
    unsigned int ebx, ecx, edx;
    unsigned int unused;
    
    cpuid(0, &unused, &ebx, &edx, &ecx);
    
    *(unsigned int *)(vendor + 0) = ebx;
    *(unsigned int *)(vendor + 4) = edx;
    *(unsigned int *)(vendor + 8) = ecx;
    vendor[12] = '\0';
}

/** detect_memory:
 *  Detect available memory (simplified)
 */
unsigned int detect_memory(void)
{
    return 32; /* 32 MB */
}

/** shell_visit_command:
 *  Navigate filesystem (cd equivalent)
 *  Usage: visit <directory> or vst <directory>
 */
void shell_visit_command(char *args)
{
    if (args[0] == '\0') {
        /* No argument, go to home */
        strcpy(current_directory, "/home");
        fb_puts("Visited: /home\n");
        return;
    }
    
    /* Handle special directories */
    if (strcmp(args, "..") == 0) {
        /* Go up one directory */
        if (strcmp(current_directory, "/") != 0) {
            /* Find last slash */
            int len = strlen(current_directory);
            int i;
            for (i = len - 1; i >= 0; i--) {
                if (current_directory[i] == '/') {
                    if (i == 0) {
                        current_directory[1] = '\0';
                    } else {
                        current_directory[i] = '\0';
                    }
                    break;
                }
            }
            fb_puts("Visited: ");
            fb_puts(current_directory);
            fb_puts("\n");
        } else {
            fb_puts("Already at root directory\n");
        }
        return;
    }
    
    if (strcmp(args, "/") == 0) {
        strcpy(current_directory, "/");
        fb_puts("Visited: /\n");
        return;
    }
    
    if (strcmp(args, "~") == 0) {
        strcpy(current_directory, "/home");
        fb_puts("Visited: /home\n");
        return;
    }
    
    /* Simulate directory structure */
    if (strcmp(args, "home") == 0 || strcmp(args, "/home") == 0) {
        strcpy(current_directory, "/home");
        fb_puts("Visited: /home\n");
    } else if (strcmp(args, "bin") == 0 || strcmp(args, "/bin") == 0) {
        strcpy(current_directory, "/bin");
        fb_puts("Visited: /bin\n");
    } else if (strcmp(args, "usr") == 0 || strcmp(args, "/usr") == 0) {
        strcpy(current_directory, "/usr");
        fb_puts("Visited: /usr\n");
    } else if (strcmp(args, "etc") == 0 || strcmp(args, "/etc") == 0) {
        strcpy(current_directory, "/etc");
        fb_puts("Visited: /etc\n");
    } else if (strcmp(args, "dev") == 0 || strcmp(args, "/dev") == 0) {
        strcpy(current_directory, "/dev");
        fb_puts("Visited: /dev\n");
    } else if (strcmp(args, "tmp") == 0 || strcmp(args, "/tmp") == 0) {
        strcpy(current_directory, "/tmp");
        fb_puts("Visited: /tmp\n");
    } else if (strcmp(args, "var") == 0 || strcmp(args, "/var") == 0) {
        strcpy(current_directory, "/var");
        fb_puts("Visited: /var\n");
    } else {
        fb_puts("Directory not found: ");
        fb_puts(args);
        fb_puts("\n");
        fb_puts("Available directories:\n");
        fb_puts("  /home  /bin  /usr  /etc  /dev  /tmp  /var\n");
        fb_puts("Use '..' to go up, '/' for root, '~' for home\n");
    }
}

/** shell_pwd_command:
 *  Print working directory
 */
void shell_pwd_command(void)
{
    fb_puts(current_directory);
    fb_puts("\n");
}

/** shell_ls_command:
 *  List files in current directory
 */
void shell_ls_command(void)
{
    fb_puts("Contents of ");
    fb_puts(current_directory);
    fb_puts(":\n\n");
    
    if (strcmp(current_directory, "/") == 0) {
        fb_puts("  [DIR]  bin/        - System binaries\n");
        fb_puts("  [DIR]  home/       - User home directories\n");
        fb_puts("  [DIR]  usr/        - User programs\n");
        fb_puts("  [DIR]  etc/        - Configuration files\n");
        fb_puts("  [DIR]  dev/        - Device files\n");
        fb_puts("  [DIR]  tmp/        - Temporary files\n");
        fb_puts("  [DIR]  var/        - Variable data\n");
    } else if (strcmp(current_directory, "/bin") == 0) {
        fb_puts("  [EXE]  shell       - polyfdOS Shell (26 KB)\n");
        fb_puts("  [EXE]  snake       - Snake Game (12 KB)\n");
        fb_puts("  [EXE]  cat         - Concatenate files\n");
        fb_puts("  [EXE]  ls          - List directory\n");
    } else if (strcmp(current_directory, "/home") == 0) {
        fb_puts("  [DIR]  synthos/    - User directory\n");
        fb_puts("  [DIR]  guest/      - Guest directory\n");
    } else if (strcmp(current_directory, "/usr") == 0) {
        fb_puts("  [DIR]  local/      - Locally installed software\n");
        fb_puts("  [DIR]  share/      - Shared data\n");
        fb_puts("  [DIR]  include/    - Header files\n");
    } else if (strcmp(current_directory, "/etc") == 0) {
        fb_puts("  [FILE] os-release  - OS information\n");
        fb_puts("  [FILE] hostname    - System hostname\n");
        fb_puts("  [FILE] fstab       - Filesystem table\n");
    } else if (strcmp(current_directory, "/dev") == 0) {
        fb_puts("  [DEV]  keyboard    - PS/2 Keyboard\n");
        fb_puts("  [DEV]  fb0         - Framebuffer\n");
        fb_puts("  [DEV]  ttyS0       - Serial port COM1\n");
        fb_puts("  [DEV]  null        - Null device\n");
    } else if (strcmp(current_directory, "/tmp") == 0) {
        fb_puts("  (empty - temporary storage)\n");
    } else if (strcmp(current_directory, "/var") == 0) {
        fb_puts("  [DIR]  log/        - System logs\n");
        fb_puts("  [DIR]  cache/      - Cached data\n");
    } else {
        fb_puts("  (empty)\n");
    }
    
    fb_puts("\n");
}

/** shell_sysinfo_command:
 *  Display comprehensive system information
 */
void shell_sysinfo_command(void)
{
    char vendor[13];
    char buffer[32];
    
    fb_puts("==============================================\n");
    fb_puts("           polyfdOS System Information\n");
    fb_puts("==============================================\n\n");
    
    fb_puts("CPU:\n");
    fb_puts("  Vendor: ");
    get_cpu_vendor(vendor);
    fb_puts(vendor);
    fb_puts("\n");
    
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    
    fb_puts("  Family: ");
    int_to_str((eax >> 8) & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("  Model: ");
    int_to_str((eax >> 4) & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("  Stepping: ");
    int_to_str(eax & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("  Features: ");
    if (edx & (1 << 0)) fb_puts("FPU ");
    if (edx & (1 << 23)) fb_puts("MMX ");
    if (edx & (1 << 25)) fb_puts("SSE ");
    if (edx & (1 << 26)) fb_puts("SSE2 ");
    if (ecx & (1 << 0)) fb_puts("SSE3 ");
    fb_puts("\n");
    fb_puts("  Architecture: x86 (32-bit)\n");
    fb_puts("  Cores: 1\n\n");
    
    fb_puts("Memory:\n");
    unsigned int mem = detect_memory();
    fb_puts("  Total RAM: ");
    int_to_str(mem, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n");
    fb_puts("  Available: ");
    int_to_str(mem - 2, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n");
    fb_puts("  Used: 2 MB\n\n");
    
    fb_puts("Graphics:\n");
    fb_puts("  Display: VGA Text Mode\n");
    fb_puts("  Resolution: 80x25 characters\n");
    fb_puts("  Colors: 16 colors\n");
    fb_puts("  Framebuffer: 0xB8000\n\n");
    
    fb_puts("Storage:\n");
    fb_puts("  Boot Device: CD-ROM (ISO)\n");
    fb_puts("  Filesystem: Virtual (RAM-based)\n");
    fb_puts("  Kernel Size: 26 KB\n\n");
    
    fb_puts("System:\n");
    fb_puts("  OS: polyfdOS v1.3\n");
    fb_puts("  Kernel: Monolithic\n");
    fb_puts("  Architecture: x86\n");
    fb_puts("  Boot Protocol: Multiboot\n");
    fb_puts("  Bootloader: GRUB\n\n");
    
    fb_puts("==============================================\n");
}

/** shell_cpu_command:
 *  Display detailed CPU information
 */
void shell_cpu_command(void)
{
    char vendor[13];
    char buffer[32];
    
    fb_puts("=== CPU Information ===\n\n");
    
    get_cpu_vendor(vendor);
    fb_puts("Vendor: ");
    fb_puts(vendor);
    fb_puts("\n");
    
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    
    fb_puts("Family: ");
    int_to_str((eax >> 8) & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("Model: ");
    int_to_str((eax >> 4) & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("Stepping: ");
    int_to_str(eax & 0x0F, buffer);
    fb_puts(buffer);
    fb_puts("\n\n");
    
    fb_puts("Features:\n");
    if (edx & (1 << 0)) fb_puts("  [x] FPU  - Floating Point Unit\n");
    if (edx & (1 << 4)) fb_puts("  [x] TSC  - Time Stamp Counter\n");
    if (edx & (1 << 5)) fb_puts("  [x] MSR  - Model Specific Registers\n");
    if (edx & (1 << 15)) fb_puts("  [x] CMOV - Conditional Move\n");
    if (edx & (1 << 23)) fb_puts("  [x] MMX  - MMX Instructions\n");
    if (edx & (1 << 25)) fb_puts("  [x] SSE  - SSE Instructions\n");
    if (edx & (1 << 26)) fb_puts("  [x] SSE2 - SSE2 Instructions\n");
    if (ecx & (1 << 0)) fb_puts("  [x] SSE3 - SSE3 Instructions\n");
    
    fb_puts("\n======================\n");
}

/** shell_mem_command:
 *  Display detailed memory information
 */
void shell_mem_command(void)
{
    char buffer[32];
    unsigned int mem = detect_memory();
    
    fb_puts("=== Memory Information ===\n\n");
    
    fb_puts("Total RAM:     ");
    int_to_str(mem, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n");
    
    fb_puts("Used:          2 MB\n");
    fb_puts("Available:     ");
    int_to_str(mem - 2, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n\n");
    
    fb_puts("Memory Layout:\n");
    fb_puts("  0x00000000 - 0x000003FF : Real Mode IVT\n");
    fb_puts("  0x00000400 - 0x000004FF : BIOS Data Area\n");
    fb_puts("  0x000B8000 - 0x000B8FA0 : VGA Text Buffer\n");
    fb_puts("  0x00100000+             : Kernel Code/Data\n");
    
    fb_puts("\n==========================\n");
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
    fb_puts("  sysinfo  - Show system hardware info\n");
    fb_puts("  cpu      - Show detailed CPU info\n");
    fb_puts("  mem      - Show memory info\n");
    fb_puts("  visit    - Navigate directories (alias: vst)\n");
    fb_puts("  pwd      - Print working directory\n");
    fb_puts("  ls       - List directory contents\n");
    fb_puts("  play     - Play Snake game!\n");
    fb_puts("  download - Download file from internet\n");
    fb_puts("  sudo     - Execute with root privileges\n");
    fb_puts("  reboot   - Reboot the system\n");
    fb_puts("  halt     - Shutdown the system\n");
}

/** shell_echo_command:
 *  Echoes back the command arguments
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
    fb_puts("polyfdOS v1.3 - Moroccan x86 Operating System\n");
    fb_puts("A minimal OS kernel built from scratch\n");
    fb_puts("Organization - Daftyon\n");
    fb_puts("Location - Morocco\n\n");
    fb_puts("Features:\n");
    fb_puts("  - Protected mode (32-bit)\n");
    fb_puts("  - Interrupt handling (GDT/IDT/PIC)\n");
    fb_puts("  - Virtual filesystem\n");
    fb_puts("  - Hardware detection\n");
}

/** shell_download_command:
 *  Simulates downloading a file
 */
void shell_download_command(char *args)
{
    if (args[0] == '\0') {
        fb_puts("Usage: download <package-name>\n");
        return;
    }
    
    fb_puts("Initializing network...\n");
    fb_puts("[OK] Connected to packages.polyfdos.ma\n\n");
    fb_puts("Searching: ");
    fb_puts(args);
    fb_puts("\n");
    
    if (strstr(args, "python")) {
        fb_puts("[FOUND] python-3.12.0\nDownloading");
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        fb_puts(" [100%]\n[OK] Installation complete!\n");
    } else {
        fb_puts("[ERROR] Package not found\n");
    }
}

/** shell_sudo_command:
 *  Executes with root privileges
 */
void shell_sudo_command(char *args)
{
    fb_puts("[sudo] password for synthos:\n");
    fb_puts("Authenticating...\n");
    fb_puts("Access granted!\n");
    fb_puts("Running: ");
    fb_puts(args);
    fb_puts("\n");
}

/** shell_reboot_command:
 *  Reboots the system
 */
void shell_reboot_command(void)
{
    fb_puts("Rebooting polyfdOS...\n");
    __asm__ volatile("cli");
    unsigned char temp;
    do {
        temp = inb(0x64);
        if (temp & 1) inb(0x60);
    } while (temp & 2);
    outb(0x64, 0xFE);
    while(1);
}

/** shell_halt_command:
 *  Halts the system
 */
void shell_halt_command(void)
{
    fb_puts("Halting polyfdOS...\n");
    __asm__ volatile("cli; hlt");
    while(1);
}

/** shell_play_command:
 *  Launches Snake game
 */
void shell_play_command(void)
{
    snake_game();
    fb_clear();
    fb_puts("Welcome back!\n\n");
}

/** shell_execute_command:
 *  Executes a command
 */
void shell_execute_command(void)
{
    command_buffer[buffer_index] = '\0';
    
    if (buffer_index == 0) {
        fb_puts(current_directory);
        fb_puts(" > ");
        return;
    }
    
    char *cmd = command_buffer;
    char *args = command_buffer;
    
    while (*args && *args != ' ') {
        args++;
    }
    
    if (*args == ' ') {
        *args = '\0';
        args++;
    }
    
    if (strcmp(cmd, "help") == 0) {
        shell_help_command();
    } else if (strcmp(cmd, "clear") == 0) {
        shell_clear_command();
    } else if (strcmp(cmd, "echo") == 0) {
        shell_echo_command(args);
    } else if (strcmp(cmd, "about") == 0) {
        shell_about_command();
    } else if (strcmp(cmd, "sysinfo") == 0) {
        shell_sysinfo_command();
    } else if (strcmp(cmd, "cpu") == 0) {
        shell_cpu_command();
    } else if (strcmp(cmd, "mem") == 0) {
        shell_mem_command();
    } else if (strcmp(cmd, "visit") == 0 || strcmp(cmd, "vst") == 0) {
        shell_visit_command(args);
    } else if (strcmp(cmd, "pwd") == 0) {
        shell_pwd_command();
    } else if (strcmp(cmd, "ls") == 0) {
        shell_ls_command();
    } else if (strcmp(cmd, "play") == 0) {
        shell_play_command();
    } else if (strcmp(cmd, "download") == 0) {
        shell_download_command(args);
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
    
    buffer_index = 0;
    fb_puts(current_directory);
    fb_puts(" > ");
}

/** shell_init:
 *  Initializes the shell
 */
void shell_init(void)
{
    buffer_index = 0;
    fb_clear();
    
    fb_puts(" ____       _        __     _  ___  ____  \n");
    fb_puts("|  _ \\ ___ | |_   _ / _| __| |/ _ \\/ ___| \n");
    fb_puts("| |_) / _ \\| | | | | |_ / _` | | | \\___ \\ \n");
    fb_puts("|  __/ (_) | | |_| |  _| (_| | |_| |___) |\n");
    fb_puts("|_|   \\___/|_|\\__, |_|  \\__,_|\\___/|____/ \n");
    fb_puts("              |___/                       \n");
    fb_puts("\n");
    fb_puts("===============================================\n");
    fb_puts("   Moroccan x86 Operating System v1.3\n");
    fb_puts("   Developer: Synthos | Organization: Daftyon\n");
    fb_puts("===============================================\n\n");
    fb_puts("Type 'help' for available commands.\n");
    fb_puts("Try 'vst /home' to navigate directories!\n\n");
    fb_puts(current_directory);
    fb_puts(" > ");
}

/** shell_update:
 *  Updates the shell
 */
void shell_update(void)
{
    char c = keyboard_get_char();
    
    if (c != 0) {
        serial_write("GOT CHAR: ", 10);
        serial_write(&c, 1);
        serial_write("\n", 1);
        
        if (c == '\n') {
            fb_putc('\n');
            shell_execute_command();
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                fb_putc('\b');
            }
        } else if (buffer_index < COMMAND_BUFFER_SIZE - 1) {
            command_buffer[buffer_index] = c;
            buffer_index++;
            fb_putc(c);
        }
    }
}
