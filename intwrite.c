#include "intwrite.h"

size_t write_8(unsigned char data, FILE* stream)
{
	return fwrite(&data, 1, 1, stream);
}

size_t write_16(unsigned int data, FILE* stream)
{
	return (write_8(data >> 8, stream) && write_8(data, stream));
}

size_t write_32(unsigned long data, FILE* stream)
{
	return (write_8(data >> 24, stream) &&
	        write_8(data >> 16, stream) &&
	        write_8(data >> 8,  stream) &&
	        write_8(data,       stream));
}
