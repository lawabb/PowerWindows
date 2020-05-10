/*****************************************
 *  
 *  
 *****************************************/

#include <stdint.h>
#include <Arduino.h>

void serial_print(const char* print_string, bool side);
void serial_print_val(const char* print_string, unsigned long print_val, bool side);
void serial_print_uval(const char* print_string, int32_t print_val, bool side);
int aprintf(char *str, ...);
