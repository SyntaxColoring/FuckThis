#include <stdlib.h>
#include <string.h>
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

size_t write_utf8_constant(const char* string, FILE* stream)
{
	size_t length = strlen(string);
	return (write_8(1, stream) &&
	        write_16(length, stream) &&
	        fwrite(string, 1, length, stream));
}

size_t write_class_constant(unsigned int name_index, FILE* stream)
{
	return (write_8(7, stream) && write_16(name_index, stream));
}

int main(void)
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
		write_16(51, of); /* Major version. */
		
		write_16(8, of); /* Number of entries in constant pool plus one. */
		write_utf8_constant("FuckJava", of);
		write_class_constant(1, of);
		write_utf8_constant("java/lang/Object", of);
		write_class_constant(3, of);
		write_utf8_constant("main", of);
		write_utf8_constant("([Ljava/lang/String;)V", of);
		write_utf8_constant("Code", of);
		
		write_16(0x0001 | 0x0010, of); /* Access flags (public final). */
		
		write_16(2, of); /* This class. */
		write_16(4, of); /* Super class. */
		
		write_16(0, of); /* Interfaces count. */
		write_16(0, of); /* Fields count. */
		                    
		write_16(1, of); /* Methods count. */
		write_16(0x0001 | 0x0008, of); /* Method flags (public static). */
		write_16(5, of); /* Method name index. */
		write_16(6, of); /* Method descriptor index. */
		write_16(1, of); /* Method attributes count. */
		
		write_16(7, of); /* Code attribute name index. */
		write_32(13, of); /* Code attribute length (without first six bytes). */
		write_16(100, of); /* Max stack. */
		write_16(100, of); /* Max locals. */
		write_32(1, of); /* Code length. */
		write_8(0xb1, of); /* Return instruction. */
		write_16(0, of); /* Exception table length. */
		write_16(0, of); /* Code attribute sub-attributes count. */
		
		write_16(0, of); /* Class attributes count. */
	}
}
