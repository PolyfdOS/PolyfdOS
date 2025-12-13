/**
 * sysfiles.c - Real System Files Implementation
 */

#include "sysfiles.h"
#include "filesystem.h"
#include "hardware.h"

/** Helper to convert int to string */
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

/** Helper to append string to buffer */
static void append_str(char *dest, const char *src, int *pos)
{
    while (*src && *pos < 2048) {
        dest[(*pos)++] = *src++;
    }
}

/** sysfiles_populate_etc */
void sysfiles_populate_etc(void)
{
    char buffer[2048];
    int pos;
    
    /* /etc/os-release */
    pos = 0;
    append_str(buffer, "NAME=\"polyfdOS\"\n", &pos);
    append_str(buffer, "VERSION=\"1.4\"\n", &pos);
    append_str(buffer, "ID=polyfdos\n", &pos);
    append_str(buffer, "VERSION_ID=\"1.4\"\n", &pos);
    append_str(buffer, "PRETTY_NAME=\"polyfdOS 1.4\"\n", &pos);
    append_str(buffer, "HOME_URL=\"https://github.com/Daftyon\"\n", &pos);
    append_str(buffer, "BUG_REPORT_URL=\"https://github.com/Daftyon/polyfdOS/issues\"\n", &pos);
    append_str(buffer, "LOGO=polyfdos\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/os-release", buffer, pos);
    
    /* /etc/hostname */
    pos = 0;
    append_str(buffer, "polyfdos-morocco\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/hostname", buffer, pos);
    
    /* /etc/fstab */
    pos = 0;
    append_str(buffer, "# /etc/fstab: static filesystem information\n", &pos);
    append_str(buffer, "#\n", &pos);
    append_str(buffer, "# <filesystem> <mount> <type> <options> <dump> <pass>\n", &pos);
    append_str(buffer, "ramfs          /      ramfs  defaults    0      0\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/fstab", buffer, pos);
    
    /* /etc/shells */
    pos = 0;
    append_str(buffer, "# /etc/shells: valid login shells\n", &pos);
    append_str(buffer, "/bin/sh\n", &pos);
    append_str(buffer, "/bin/bash\n", &pos);
    append_str(buffer, "/bin/polysh\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/shells", buffer, pos);
    
    /* /etc/issue */
    pos = 0;
    append_str(buffer, "polyfdOS 1.4 \\n \\l\n\n", &pos);
    append_str(buffer, "Moroccan x86 Operating System\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/issue", buffer, pos);
    
    /* /etc/motd (message of the day) */
    pos = 0;
    append_str(buffer, "Welcome to polyfdOS - Moroccan x86 OS\n", &pos);
    append_str(buffer, "Organization: Daftyon\n", &pos);
    append_str(buffer, "Type 'help' for available commands\n", &pos);
    buffer[pos] = '\0';
    fs_create("/etc/motd", buffer, pos);
}

/** sysfiles_populate_proc */
void sysfiles_populate_proc(void)
{
    char buffer[2048];
    char num_str[32];
    int pos;
    unsigned int cpu_family, cpu_model, cpu_stepping;
    unsigned int feat_edx, feat_ecx;
    char vendor[13];
    
    /* /proc/cpuinfo */
    pos = 0;
    hw_get_cpu_info(vendor, &cpu_family, &cpu_model, &cpu_stepping);
    hw_get_cpu_features(&feat_edx, &feat_ecx);
    
    append_str(buffer, "processor\t: 0\n", &pos);
    append_str(buffer, "vendor_id\t: ", &pos);
    append_str(buffer, vendor, &pos);
    append_str(buffer, "\n", &pos);
    
    append_str(buffer, "cpu family\t: ", &pos);
    int_to_str(cpu_family, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, "\n", &pos);
    
    append_str(buffer, "model\t\t: ", &pos);
    int_to_str(cpu_model, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, "\n", &pos);
    
    append_str(buffer, "stepping\t: ", &pos);
    int_to_str(cpu_stepping, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, "\n", &pos);
    
    append_str(buffer, "flags\t\t: ", &pos);
    if (feat_edx & (1 << 0)) append_str(buffer, "fpu ", &pos);
    if (feat_edx & (1 << 23)) append_str(buffer, "mmx ", &pos);
    if (feat_edx & (1 << 25)) append_str(buffer, "sse ", &pos);
    if (feat_edx & (1 << 26)) append_str(buffer, "sse2 ", &pos);
    if (feat_ecx & (1 << 0)) append_str(buffer, "sse3 ", &pos);
    append_str(buffer, "\n", &pos);
    
    buffer[pos] = '\0';
    fs_create("/proc/cpuinfo", buffer, pos);
    
    /* /proc/meminfo */
    pos = 0;
    unsigned int mem = hw_detect_memory();
    unsigned int mem_kb = mem * 1024;
    
    append_str(buffer, "MemTotal:       ", &pos);
    int_to_str(mem_kb, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, " kB\n", &pos);
    
    append_str(buffer, "MemFree:        ", &pos);
    int_to_str(mem_kb - 2048, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, " kB\n", &pos);
    
    append_str(buffer, "MemAvailable:   ", &pos);
    int_to_str(mem_kb - 2048, num_str);
    append_str(buffer, num_str, &pos);
    append_str(buffer, " kB\n", &pos);
    
    buffer[pos] = '\0';
    fs_create("/proc/meminfo", buffer, pos);
    
    /* /proc/version */
    pos = 0;
    append_str(buffer, "polyfdOS version 1.4 (Daftyon) ", &pos);
    append_str(buffer, "(gcc version 11.4.0) ", &pos);
    append_str(buffer, "#1 SMP Morocco\n", &pos);
    buffer[pos] = '\0';
    fs_create("/proc/version", buffer, pos);
    
    /* /proc/uptime */
    pos = 0;
    append_str(buffer, "0.00 0.00\n", &pos);  /* Will be updated dynamically */
    buffer[pos] = '\0';
    fs_create("/proc/uptime", buffer, pos);
}

/** sysfiles_populate_dev */
void sysfiles_populate_dev(void)
{
    char buffer[256];
    int pos;
    
    /* /dev/null - empty device */
    pos = 0;
    buffer[pos] = '\0';
    fs_create("/dev/null", buffer, 0);
    
    /* /dev/zero - zero device */
    pos = 0;
    buffer[pos] = '\0';
    fs_create("/dev/zero", buffer, 0);
    
    /* /dev/random - random device (placeholder) */
    pos = 0;
    append_str(buffer, "Random device\n", &pos);
    buffer[pos] = '\0';
    fs_create("/dev/random", buffer, pos);
    
    /* /dev/keyboard - keyboard device */
    pos = 0;
    append_str(buffer, "PS/2 Keyboard Device\n", &pos);
    buffer[pos] = '\0';
    fs_create("/dev/keyboard", buffer, pos);
    
    /* /dev/fb0 - framebuffer device */
    pos = 0;
    append_str(buffer, "VGA Text Mode Framebuffer\n", &pos);
    append_str(buffer, "Address: 0xB8000\n", &pos);
    append_str(buffer, "Resolution: 80x25\n", &pos);
    buffer[pos] = '\0';
    fs_create("/dev/fb0", buffer, pos);
    
    /* /dev/ttyS0 - serial console */
    pos = 0;
    append_str(buffer, "Serial Console COM1\n", &pos);
    append_str(buffer, "Baud: 38400\n", &pos);
    buffer[pos] = '\0';
    fs_create("/dev/ttyS0", buffer, pos);
}

/** sysfiles_update_proc */
void sysfiles_update_proc(void)
{
    /* Can be called periodically to update dynamic /proc files */
    /* For now, just recreate them */
    sysfiles_populate_proc();
}

/** sysfiles_init */
void sysfiles_init(void)
{
    /* Create standard directories first */
    fs_mkdir("/etc");
    fs_mkdir("/proc");
    fs_mkdir("/dev");
    fs_mkdir("/sys");
    fs_mkdir("/boot");
    
    /* Populate with real files */
    sysfiles_populate_etc();
    sysfiles_populate_proc();
    sysfiles_populate_dev();
}