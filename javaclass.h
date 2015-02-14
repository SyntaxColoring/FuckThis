#ifndef JAVACLASS_H_INCLUDED
#define JAVACLASS_H_INCLUDED

#include <stdio.h>

struct java_constant;

struct java_method
{
	const char* name;
	const char* type;
	unsigned int access_flags;
	
	unsigned char* bytecode;
	unsigned int max_stack;
	unsigned int max_locals;
};

struct java_class
{
	const char* name;
	const char* super_name;
	unsigned int access_flags;
	
	size_t constant_count;
	struct java_constant* constants;
	
	size_t method_count;
	struct java_method* methods;
};

extern const unsigned int JAVA_CLASS_PUBLIC;
extern const unsigned int JAVA_CLASS_FINAL;
extern const unsigned int JAVA_CLASS_STATIC;

void java_class_write(struct java_class* source, FILE* destination);

unsigned int java_class_ref_utf8(struct java_class* context,
                                 const char* string);

unsigned int java_class_ref_class(struct java_class* context,
                                  const char* name);

unsigned int java_class_ref_name_and_type(struct java_class* context,
                                          const char* name,
                                          const char* type);

unsigned int java_class_ref_field(struct java_class* context,
                                  const char* class_name,
                                  const char* field_name,
                                  const char* field_type);

unsigned int java_class_ref_method(struct java_class* context,
                                   const char* class_name,
                                   const char* method_name,
                                   const char* method_type);

#endif
