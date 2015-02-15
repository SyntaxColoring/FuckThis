#ifndef JAVACLASS_H_INCLUDED
#define JAVACLASS_H_INCLUDED

#include <stdio.h>
#include "util.h"

struct java_file;

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

struct java_file* java_create(void);
void java_free(struct java_file* file);
struct java_class* java_get_class(struct java_file* file);
void java_write(struct java_file* file, FILE* stream);

unsigned int java_ref_utf8(struct java_file* file, const char* string);
unsigned int java_ref_class(struct java_file* file, const char* name);
unsigned int java_ref_name_and_type(struct java_file* file,
                                    const char* name,
                                    const char* type);
unsigned int java_ref_field(struct java_file* file,
                            const char* class_name,
                            const char* field_name,
                            const char* field_type);
unsigned int java_ref_method(struct java_file* context,
                             const char* class_name,
                             const char* method_name,
                             const char* method_type);

#endif
