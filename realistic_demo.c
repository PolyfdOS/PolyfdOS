/**
 * realistic_demo.c - Demo and test program for Realistic type
 * 
 * Demonstrates three-valued logic operations in polyfdOS
 */

#include "realistic.h"
#include "fb.h"

/** Print truth table for binary operation */
static void print_truth_table_binary(const char *op_name,
                                     realistic_t (*op)(realistic_t, realistic_t))
{
    realistic_t a, b, result;
    const char *vals[] = {"F", "T", "R"};
    
    fb_puts("\n");
    fb_puts((char *)op_name);  /* Cast away const */
    fb_puts(" Truth Table:\n");
    fb_puts("     F  T  R\n");
    fb_puts("   +---------\n");
    
    for (a = R_FALSE; a <= R_REALISTIC; a++) {
        fb_puts(" ");
        fb_puts((char *)vals[a]);
        fb_puts(" | ");
        
        for (b = R_FALSE; b <= R_REALISTIC; b++) {
            result = op(a, b);
            fb_puts((char *)vals[result]);
            fb_puts("  ");
        }
        
        fb_puts("\n");
    }
}

/** Print truth table for unary operation */
static void print_truth_table_unary(const char *op_name,
                                   realistic_t (*op)(realistic_t))
{
    realistic_t a, result;
    const char *vals[] = {"F", "T", "R"};
    
    fb_puts("\n");
    fb_puts((char *)op_name);  /* Cast away const */
    fb_puts(" Truth Table:\n");
    fb_puts("  Input | Output\n");
    fb_puts("  ------+-------\n");
    
    for (a = R_FALSE; a <= R_REALISTIC; a++) {
        fb_puts("    ");
        fb_puts((char *)vals[a]);
        fb_puts("   |   ");
        result = op(a);
        fb_puts((char *)vals[result]);
        fb_puts("\n");
    }
}

/** Demonstrate uncertainty handling */
static void demonstrate_uncertainty(void)
{
    realistic_t sensor_reading = R_REALISTIC;  /* Uncertain sensor */
    realistic_t threshold_ok = R_TRUE;         /* System OK */
    realistic_t system_safe;
    
    fb_puts("\n=== Uncertainty Handling Demo ===\n");
    fb_puts("Sensor reading: ");
    fb_puts((char *)realistic_to_string(sensor_reading));
    fb_puts("\n");
    
    fb_puts("Threshold OK: ");
    fb_puts((char *)realistic_to_string(threshold_ok));
    fb_puts("\n");
    
    /* System is safe if sensor is OK AND threshold is OK */
    system_safe = realistic_and(sensor_reading, threshold_ok);
    
    fb_puts("System safe? ");
    fb_puts((char *)realistic_to_string(system_safe));
    fb_puts("\n");
    fb_puts("(REALISTIC = uncertain, need more information)\n");
}

/** Demonstrate practical example: File system check */
static void demonstrate_filesystem_check(void)
{
    realistic_t disk_readable = R_TRUE;
    realistic_t data_corrupt = R_REALISTIC;  /* Unknown if corrupt */
    realistic_t backup_exists = R_TRUE;
    realistic_t can_recover;
    
    fb_puts("\n=== Filesystem Check Demo ===\n");
    fb_puts("Disk readable: ");
    fb_puts((char *)realistic_to_string(disk_readable));
    fb_puts("\n");
    
    fb_puts("Data corrupt: ");
    fb_puts((char *)realistic_to_string(data_corrupt));
    fb_puts("\n");
    
    fb_puts("Backup exists: ");
    fb_puts((char *)realistic_to_string(backup_exists));
    fb_puts("\n");
    
    /* Can recover if: (disk readable AND backup exists) OR NOT corrupt */
    realistic_t recovery_possible = realistic_and(disk_readable, backup_exists);
    realistic_t not_corrupt = realistic_not(data_corrupt);
    can_recover = realistic_or(recovery_possible, not_corrupt);
    
    fb_puts("Can recover? ");
    fb_puts((char *)realistic_to_string(can_recover));
    fb_puts("\n");
    fb_puts("(REALISTIC = recovery uncertain)\n");
}

/** Main realistic demo function */
void realistic_demo(void)
{
    fb_puts("======================================\n");
    fb_puts("  Three-Valued Logic with REALISTIC\n");
    fb_puts("  polyfdOS - Research Implementation\n");
    fb_puts("======================================\n");
    fb_puts("\nValues: F=FALSE, T=TRUE, R=REALISTIC\n");
    
    /* Show truth tables */
    print_truth_table_unary("NOT", realistic_not);
    print_truth_table_binary("AND", realistic_and);
    print_truth_table_binary("OR", realistic_or);
    print_truth_table_binary("XOR", realistic_xor);
    print_truth_table_binary("IMPLIES", realistic_implies);
    print_truth_table_binary("EQUIV", realistic_equiv);
    
    /* Practical demonstrations */
    demonstrate_uncertainty();
    demonstrate_filesystem_check();
    
    fb_puts("\n======================================\n");
    fb_puts("Press any key to return to shell...\n");
    
    /* Wait for keypress */
    extern char keyboard_get_char(void);
    while (keyboard_get_char() == 0) {
        /* Wait for key */
    }
}
