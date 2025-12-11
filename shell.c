#include "shell.h"
#include "fb.h"
#include "keyboard.h"
#include "serial.h"
#include "snake.h"
#include "io.h"
#include "texteditor.h"
#include "filesystem.h"
#include "hardware.h"

#define COMMAND_BUFFER_SIZE 256
#define MAX_PATH_LENGTH 128

static char command_buffer[COMMAND_BUFFER_SIZE];
static unsigned int buffer_index = 0;
static char current_directory[MAX_PATH_LENGTH] = "/";

/** strcmp */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/** strstr */
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

/** strlen */
int strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/** strcpy */
void strcpy(char *dest, const char *src)
{
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/** strcat */
void strcat(char *dest, const char *src)
{
    while (*dest) dest++;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/** int_to_str */
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

/** shell_visit_command */
void shell_visit_command(char *args)
{
    if (args[0] == '\0') {
        strcpy(current_directory, "/home");
        fb_puts("Visited: /home\n");
        return;
    }
    
    if (strcmp(args, "..") == 0) {
        if (strcmp(current_directory, "/") != 0) {
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
            fb_puts("Already at root\n");
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
    }
}

/** shell_pwd_command */
void shell_pwd_command(void)
{
    fb_puts(current_directory);
    fb_puts("\n");
}

/** shell_ls_command */
static int file_count;

static void ls_callback(const char *filename)
{
    fb_puts("  [FILE] ");
    fb_puts((char *)filename);
    fb_puts("\n");
    file_count++;
}

void shell_ls_command(void)
{
    int i;
    
    fb_puts("Contents of ");
    fb_puts(current_directory);
    fb_puts(":\n\n");
    
    if (strcmp(current_directory, "/") == 0) {
        char *dirs[] = {"bin", "home", "usr", "etc", "dev", "tmp", "var", 0};
        for (i = 0; dirs[i] != 0; i++) {
            fb_puts("  [DIR]  ");
            fb_puts(dirs[i]);
            fb_puts("/\n");
        }
    }
    
    fb_puts("\n");
    file_count = 0;
    fs_list_directory(current_directory, ls_callback);
    
    if (file_count == 0) {
        fb_puts("  (no files)\n");
    }
    
    fb_puts("\n");
}

/** shell_sysinfo_command - REAL HARDWARE! */
void shell_sysinfo_command(void)
{
    char vendor[13];
    char buffer[32];
    unsigned int cpu_family, cpu_model, cpu_stepping;
    unsigned int feat_edx, feat_ecx;
    
    fb_puts("==============================================\n");
    fb_puts("     polyfdOS System Information (REAL!)\n");
    fb_puts("==============================================\n\n");
    
    fb_puts("CPU:\n");
    fb_puts("  Vendor: ");
    hw_get_cpu_info(vendor, &cpu_family, &cpu_model, &cpu_stepping);
    fb_puts(vendor);
    fb_puts("\n");
    
    fb_puts("  Family: ");
    int_to_str(cpu_family, buffer);
    fb_puts(buffer);
    fb_puts("  Model: ");
    int_to_str(cpu_model, buffer);
    fb_puts(buffer);
    fb_puts("  Stepping: ");
    int_to_str(cpu_stepping, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("  Features: ");
    hw_get_cpu_features(&feat_edx, &feat_ecx);
    if (feat_edx & (1 << 0)) fb_puts("FPU ");
    if (feat_edx & (1 << 23)) fb_puts("MMX ");
    if (feat_edx & (1 << 25)) fb_puts("SSE ");
    if (feat_edx & (1 << 26)) fb_puts("SSE2 ");
    if (feat_ecx & (1 << 0)) fb_puts("SSE3 ");
    fb_puts("\n\n");
    
    fb_puts("Memory:\n");
    unsigned int mem = hw_detect_memory();
    fb_puts("  Total RAM: ");
    int_to_str(mem, buffer);
    fb_puts(buffer);
    fb_puts(" MB (REAL!)\n");
    fb_puts("  Used: ~2 MB\n");
    fb_puts("  Available: ");
    int_to_str(mem - 2, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n\n");
    
    fb_puts("Filesystem:\n");
    fb_puts("  Type: RAM-based\n");
    fb_puts("  Max Files: 50\n");
    fb_puts("  Max Size/File: 2 KB\n\n");
    
    fb_puts("System:\n");
    fb_puts("  OS: polyfdOS v1.4\n");
    fb_puts("  Bootloader: GRUB\n");
    
    fb_puts("==============================================\n");
}

/** shell_cpu_command - REAL HARDWARE! */
void shell_cpu_command(void)
{
    char vendor[13];
    char buffer[32];
    unsigned int cpu_family, cpu_model, cpu_stepping;
    unsigned int feat_edx, feat_ecx;
    
    fb_puts("=== CPU Information (REAL) ===\n\n");
    
    hw_get_cpu_info(vendor, &cpu_family, &cpu_model, &cpu_stepping);
    fb_puts("Vendor: ");
    fb_puts(vendor);
    fb_puts("\n");
    
    fb_puts("Family: ");
    int_to_str(cpu_family, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("Model: ");
    int_to_str(cpu_model, buffer);
    fb_puts(buffer);
    fb_puts("\n");
    
    fb_puts("Stepping: ");
    int_to_str(cpu_stepping, buffer);
    fb_puts(buffer);
    fb_puts("\n\n");
    
    fb_puts("Features:\n");
    hw_get_cpu_features(&feat_edx, &feat_ecx);
    if (feat_edx & (1 << 0)) fb_puts("  [x] FPU\n");
    if (feat_edx & (1 << 23)) fb_puts("  [x] MMX\n");
    if (feat_edx & (1 << 25)) fb_puts("  [x] SSE\n");
    if (feat_edx & (1 << 26)) fb_puts("  [x] SSE2\n");
    if (feat_ecx & (1 << 0)) fb_puts("  [x] SSE3\n");
    
    fb_puts("\n==============================\n");
}

/** shell_mem_command - REAL HARDWARE! */
void shell_mem_command(void)
{
    char buffer[32];
    unsigned int mem = hw_detect_memory();
    
    fb_puts("=== Memory (REAL) ===\n\n");
    
    fb_puts("Total: ");
    int_to_str(mem, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n");
    
    fb_puts("Used: ~2 MB\n");
    fb_puts("Available: ");
    int_to_str(mem - 2, buffer);
    fb_puts(buffer);
    fb_puts(" MB\n\n");
    
    fb_puts("Detection: CMOS + Probing\n");
    
    fb_puts("\n=====================\n");
}

/** shell_edit_command */
void shell_edit_command(char *args)
{
    if (args[0] == '\0') {
        texteditor_open(0, current_directory);
    } else {
        texteditor_open(args, current_directory);
    }
    fb_clear();
    fb_puts("Welcome back!\n\n");
}

/** shell_clear_command */
void shell_clear_command(void)
{
    fb_clear();
}

/** shell_help_command */
void shell_help_command(void)
{
    fb_puts("Commands:\n");
    fb_puts("  help     - Help\n");
    fb_puts("  clear    - Clear screen\n");
    fb_puts("  sysinfo  - System info (REAL!)\n");
    fb_puts("  cpu      - CPU info (REAL!)\n");
    fb_puts("  mem      - Memory info (REAL!)\n");
    fb_puts("  cd/pwd/ls- Navigation\n");
    fb_puts("  edit     - Text editor\n");
    fb_puts("  play     - Snake game\n");
    fb_puts("  reboot/halt - Power\n");
}

/** shell_echo_command */
void shell_echo_command(char *args)
{
    fb_puts(args);
    fb_puts("\n");
}

/** shell_about_command */
void shell_about_command(void)
{
    fb_puts("polyfdOS v1.4\n");
    fb_puts("Moroccan x86 OS\n");
    fb_puts("Org: Daftyon\n");
}

/** shell_download_command */
void shell_download_command(char *args)
{
    if (args[0] == '\0') {
        fb_puts("Usage: download <package>\n");
        return;
    }
    
    fb_puts("Searching: ");
    fb_puts(args);
    fb_puts("\n");
    
    if (strstr(args, "python")) {
        fb_puts("Downloading");
        int i;
        for (i = 0; i < 5; i++) {
            fb_puts(".");
            volatile int j, k;
            for (j = 0; j < 1000; j++) {
                for (k = 0; k < 10000; k++) { }
            }
        }
        fb_puts(" [OK]\n");
    } else {
        fb_puts("[ERROR] Not found\n");
    }
}

/** shell_sudo_command */
void shell_sudo_command(char *args)
{
    fb_puts("[sudo] Running: ");
    fb_puts(args);
    fb_puts("\n");
}

/** shell_reboot_command */
void shell_reboot_command(void)
{
    fb_puts("Rebooting...\n");
    __asm__ volatile("cli");
    unsigned char temp;
    do {
        temp = inb(0x64);
        if (temp & 1) inb(0x60);
    } while (temp & 2);
    outb(0x64, 0xFE);
    while(1);
}

/** shell_halt_command */
void shell_halt_command(void)
{
    fb_puts("Halting...\n");
    __asm__ volatile("cli; hlt");
    while(1);
}

/** shell_play_command */
void shell_play_command(void)
{
    snake_game();
    fb_clear();
    fb_puts("Welcome back!\n\n");
}

/** shell_execute_command */
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
    } else if (strcmp(cmd, "visit") == 0 || strcmp(cmd, "vst") == 0 || strcmp(cmd, "cd") == 0) {
        shell_visit_command(args);
    } else if (strcmp(cmd, "pwd") == 0) {
        shell_pwd_command();
    } else if (strcmp(cmd, "ls") == 0) {
        shell_ls_command();
    } else if (strcmp(cmd, "edit") == 0) {
        shell_edit_command(args);
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
        fb_puts("Unknown: ");
        fb_puts(cmd);
        fb_puts("\n");
    }
    
    buffer_index = 0;
    fb_puts(current_directory);
    fb_puts(" > ");
}

/** shell_init */
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
    fb_puts("   polyfdOS v1.4 - Real Hardware Detection!\n");
    fb_puts("   Morocco | Daftyon\n");
    fb_puts("===============================================\n\n");
    fb_puts("Type 'help' | Try 'sysinfo' for real info!\n\n");
    fb_puts(current_directory);
    fb_puts(" > ");
}

/** shell_update */
void shell_update(void)
{
    char c = keyboard_get_char();
    
    if (c != 0) {
        serial_write("CHAR: ", 6);
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