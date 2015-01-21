#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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

int main(int argc, char** argv)
{
	FILE* of = fopen("FuckJava.class", "w");
	if (!of)
	{
		perror("Error creating file");
	}
	else
	{
		write_32(0xcafebabe, of); /* Magic number. */
		write_16(0, of); /* Minor version. */
		write_16(45, of); /* Major version. */
	}
}
