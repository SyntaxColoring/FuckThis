/* intwrite.h: Functions for writing unsigned ints in big-endian form. */

#ifndef INTWRITE_H_INCLUDED
#define INTWRITE_H_INCLUDED

#include <stdio.h>

size_t write_8(unsigned char data, FILE* stream);
size_t write_16(unsigned int data, FILE* stream);
size_t write_32(unsigned long data, FILE* stream);

#endif
