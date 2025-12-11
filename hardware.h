#ifndef INCLUDE_HARDWARE_H
#define INCLUDE_HARDWARE_H

/** hw_detect_memory:
 *  Detect actual system memory using BIOS data
 *
 *  @return Memory size in MB
 */
unsigned int hw_detect_memory(void);

/** hw_get_cpu_info:
 *  Get real CPU information
 *
 *  @param vendor    Buffer for vendor string (13 bytes)
 *  @param family    Pointer to store family
 *  @param model     Pointer to store model
 *  @param stepping  Pointer to store stepping
 */
void hw_get_cpu_info(char *vendor, unsigned int *family, 
                     unsigned int *model, unsigned int *stepping);

/** hw_get_cpu_features:
 *  Get CPU feature flags
 *
 *  @param edx  Pointer to store EDX features
 *  @param ecx  Pointer to store ECX features
 */
void hw_get_cpu_features(unsigned int *edx, unsigned int *ecx);

#endif /* INCLUDE_HARDWARE_H */