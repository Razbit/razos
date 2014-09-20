// monitor.h defines interface for the monitor code
// jamesM's tuts

#ifndef MONITOR_H
#define MONITOR_H

#include "lib/stdint.h"

//write a character to the screen
void monitor_put(char c);

//clear the screen
void monitor_clear();

//print a null-terminated string to the screen
void monitor_write(char *c);

void monitor_write_hex(uint32_t n);

void monitor_write_dec(uint32_t n);

#endif //MONITOR_H
