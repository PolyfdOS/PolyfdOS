#include "hardware.h"

/** cpuid:
 *  Execute CPUID instruction
 */
static void cpuid(unsigned int code, unsigned int *a, unsigned int *b, 
                  unsigned int *c, unsigned int *d)
{
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

/** hw_detect_memory:
 *  Detect actual system memory
 *  Uses multiboot info and BIOS memory map
 */
unsigned int hw_detect_memory(void)
{
    /* Method 1: Read from CMOS (0x15-0x16 for extended memory in KB) */
    unsigned int mem_kb = 0;
    unsigned char low, high;
    
    /* Disable interrupts while accessing CMOS */
    __asm__ volatile("cli");
    
    /* Read extended memory size from CMOS */
    /* Address 0x17 (low byte) and 0x18 (high byte) */
    __asm__ volatile(
        "movb $0x17, %%al\n"
        "outb %%al, $0x70\n"
        "inb $0x71, %%al\n"
        "movb %%al, %0\n"
        : "=r"(low)
        :
        : "al"
    );
    
    __asm__ volatile(
        "movb $0x18, %%al\n"
        "outb %%al, $0x70\n"
        "inb $0x71, %%al\n"
        "movb %%al, %0\n"
        : "=r"(high)
        :
        : "al"
    );
    
    __asm__ volatile("sti");
    
    /* Calculate total memory in KB (extended memory + 1MB base) */
    mem_kb = (((unsigned int)high << 8) | low) + 1024;
    
    /* If CMOS reading failed or looks wrong, try method 2 */
    if (mem_kb < 1024 || mem_kb > 1048576) {
        /* Method 2: Use INT 0x15, EAX=0xE820 results stored by bootloader */
        /* For now, try to detect by checking if memory is accessible */
        /* This is a simplified detection - probe memory */
        
        volatile unsigned int *test_addr;
        unsigned int mb;
        
        /* Start from 1MB and test each MB */
        for (mb = 1; mb < 4096; mb++) {
            test_addr = (unsigned int *)(0x100000 + (mb * 1024 * 1024));
            
            /* Try to read/write (this is risky but works in VM) */
            unsigned int old_val = *test_addr;
            *test_addr = 0xAA55AA55;
            
            if (*test_addr != 0xAA55AA55) {
                /* Memory not accessible */
                *test_addr = old_val;
                return mb;
            }
            
            *test_addr = old_val;
            
            /* Don't test beyond reasonable limits */
            if (mb > 256) break;
        }
        
        mem_kb = mb * 1024;
    }
    
    /* Convert KB to MB */
    return mem_kb / 1024;
}

/** hw_get_cpu_info:
 *  Get real CPU information using CPUID
 */
void hw_get_cpu_info(char *vendor, unsigned int *family, 
                     unsigned int *model, unsigned int *stepping)
{
    unsigned int eax, ebx, ecx, edx;
    
    /* Get vendor string */
    cpuid(0, &eax, &ebx, &edx, &ecx);
    
    *(unsigned int *)(vendor + 0) = ebx;
    *(unsigned int *)(vendor + 4) = edx;
    *(unsigned int *)(vendor + 8) = ecx;
    vendor[12] = '\0';
    
    /* Get family, model, stepping */
    cpuid(1, &eax, &ebx, &ecx, &edx);
    
    *stepping = eax & 0x0F;
    *model = (eax >> 4) & 0x0F;
    *family = (eax >> 8) & 0x0F;
    
    /* Extended family/model for newer CPUs */
    if (*family == 0x0F) {
        *family += (eax >> 20) & 0xFF;
    }
    
    if (*family == 0x06 || *family == 0x0F) {
        *model += ((eax >> 16) & 0x0F) << 4;
    }
}

/** hw_get_cpu_features:
 *  Get CPU feature flags
 */
void hw_get_cpu_features(unsigned int *edx, unsigned int *ecx)
{
    unsigned int eax, ebx;
    
    cpuid(1, &eax, &ebx, ecx, edx);
}