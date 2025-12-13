/**
 * sysfiles.h - Real System Files Implementation
 * 
 * Creates actual system files like Linux:
 * /etc/os-release, /proc/cpuinfo, /proc/meminfo, /dev/null, etc.
 * 
 * All files are REAL and can be read/written using standard commands
 */

#ifndef INCLUDE_SYSFILES_H
#define INCLUDE_SYSFILES_H

/** sysfiles_init:
 *  Initialize all system files at boot
 *  Creates real files in /etc, /proc, /dev with actual content
 */
void sysfiles_init(void);

/** sysfiles_update_proc:
 *  Update /proc files with current system state
 *  Called periodically to refresh dynamic data
 */
void sysfiles_update_proc(void);

/** sysfiles_populate_etc:
 *  Create all /etc configuration files
 *  - os-release (OS information)
 *  - hostname (system hostname)
 *  - fstab (filesystem table)
 *  - shells (available shells)
 */
void sysfiles_populate_etc(void);

/** sysfiles_populate_proc:
 *  Create all /proc files (dynamic system info)
 *  - cpuinfo (CPU information)
 *  - meminfo (memory information)
 *  - uptime (system uptime)
 *  - version (kernel version)
 */
void sysfiles_populate_proc(void);

/** sysfiles_populate_dev:
 *  Create all /dev device files
 *  - null (null device)
 *  - zero (zero device)
 *  - random (random data)
 *  - keyboard (keyboard device)
 *  - fb0 (framebuffer)
 */
void sysfiles_populate_dev(void);

#endif /* INCLUDE_SYSFILES_H */