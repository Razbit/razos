// common.h typedefs, some global funcs
// JamesM's tuts

#ifndef COMMON_H
#define COMMON_H

// for standardizing types..
// these are for x86
typedef unsigned int uint32_t; //uint32_t;
typedef int int32_t; //int32_t;
typedef unsigned short uint16_t; //uint16_t;
typedef short int16_t; //sint16_t;
typedef unsigned char uint8_t; //uint8_t;
typedef char int8_t; //int8_t;

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif
