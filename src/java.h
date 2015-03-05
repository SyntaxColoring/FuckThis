/* java.h - Structures for building Java class files. */

#ifndef JAVACLASS_H_INCLUDED
#define JAVACLASS_H_INCLUDED

#include <stdio.h>
#include "cpool.h"
#include "util.h"

typedef struct java_file_opaque* java_file;

enum java_access_flags
{
	JAVA_ACCESS_PUBLIC = 0x0001,
	JAVA_ACCESS_STATIC = 0x0008,
	JAVA_ACCESS_FINAL  = 0x0010
};

struct java_method
{
	const char* name;
	const char* type;
	enum java_access_flags access_flags;
	
	u1* bytecode;
	size_t bytecode_length;
	unsigned int max_stack;
	unsigned int max_locals;
};

struct java_class
{
	const char* name;
	const char* super_name;
	enum java_access_flags access_flags;
	
	size_t method_count;
	struct java_method* methods;
};

java_file java_create(void);
void java_free(java_file file);
struct java_class* java_get_class(java_file file);
constant_pool java_get_constant_pool(java_file file);
void java_write(java_file file, FILE* stream);

#endif
