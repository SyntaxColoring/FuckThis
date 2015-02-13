#include "javaclass.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "intwrite.h"

/* Represents any kind of constant in the class's runtime constant pool. */
struct constant
{
	unsigned char tag; /* Type identifier. */
	
	union
	{
		const char* utf8; /* For CONSTANT_Utf8_info constants. */
		
		/* For other kinds of constants.  a and b are general-purpose references
		   to other locations in the runtime constant pool.  Their exact purpose
		   depends on the type of constant. */
		struct
		{
			unsigned int a;
			unsigned int b; /* May be left unused. */
		} refs;
	} data;
};

/* Constant tags. */
static const unsigned int CONSTANT_UTF8          = 1;
static const unsigned int CONSTANT_CLASS         = 7;
static const unsigned int CONSTANT_FIELD         = 9;
static const unsigned int CONSTANT_METHOD        = 10;
static const unsigned int CONSTANT_NAME_AND_TYPE = 12;

struct java_class_impl
{
	const char* name;
	const char* super_name;
	unsigned int access_flags;
	
	size_t constant_count;
	struct constant* constants;
};

const unsigned int JAVA_CLASS_PUBLIC = 0x0001;
const unsigned int JAVA_CLASS_STATIC = 0x0008;
const unsigned int JAVA_CLASS_FINAL  = 0x0010;

/* Returns nonzero if a is logically equal to b, assuming both a and b reside
   (or will reside) in the same constant pool. */
static int constants_equal(const struct constant a, const struct constant b)
{
	if (a.tag != b.tag) return 0;
	else if (a.tag == CONSTANT_UTF8)
		return !strcmp(a.data.utf8, b.data.utf8);
	else if (a.tag == CONSTANT_CLASS)
		return (a.data.refs.a == b.data.refs.a);
	else /* Assuming a valid tag, only the 2-reference constant types are left. */
		return (a.data.refs.a == b.data.refs.a &&
		        a.data.refs.b == b.data.refs.b);
}

static void write_constant(const struct constant constant, FILE* const stream)
{	
	write_8(constant.tag, stream);
	
	if (constant.tag == CONSTANT_UTF8)
	{
		write_16(strlen(constant.data.utf8), stream);
		fprintf(stream, "%s", constant.data.utf8);
	}
	
	else
	{
		write_16(constant.data.refs.a, stream);
		if (constant.tag != CONSTANT_CLASS)
			write_16(constant.data.refs.b, stream);
	}
}

static unsigned int add_constant(struct java_class_impl* destination,
                                 struct constant new_constant)
{
	size_t index;
	
	/* Return a reference to an existing equivalent constant, if one exists. */
	for (index = 0; index < destination->constant_count; index++)
		if (constants_equal(new_constant, destination->constants[index]))
			return index + 1;
	
	/* Otherwise, add a new constant and return a reference to that. */
	destination->constants[destination->constant_count++] = new_constant;
	return destination->constant_count;
}

java_class java_class_create(const char* const name,
                             const char* const super_name,
                             const unsigned int access_flags)
{
	struct java_class_impl* result = malloc(sizeof(struct java_class_impl));
	result->name = name;
	result->super_name = super_name;
	result->access_flags = access_flags;
	result->constant_count = 0;
	result->constants = malloc(sizeof(struct constant) * 50); /* Hope that's enough! */
	return result;
}

unsigned int java_class_ref_utf8(java_class context, const char* string)
{
	struct constant constant;
	constant.tag = CONSTANT_UTF8;
	constant.data.utf8 = string;
	return add_constant(context, constant);
}

unsigned int java_class_ref_class(java_class context, const char* name)
{
	struct constant constant;
	constant.tag = CONSTANT_CLASS;
	constant.data.refs.a = java_class_ref_utf8(context, name);
	return add_constant(context, constant);
}

unsigned int java_class_ref_name_and_type(java_class context,
                                          const char* name,
                                          const char* type)
{
	struct constant constant;
	constant.tag = CONSTANT_NAME_AND_TYPE;
	constant.data.refs.a = java_class_ref_utf8(context, name);
	constant.data.refs.b = java_class_ref_utf8(context, type);
	return add_constant(context, constant);
}

unsigned int java_class_ref_field(java_class context,
                                  const char* class_name,
                                  const char* field_name,
                                  const char* field_type)
{
	struct constant constant;
	constant.tag = CONSTANT_FIELD;
	constant.data.refs.a = java_class_ref_class(context, class_name);
	constant.data.refs.b = java_class_ref_name_and_type(context, field_name, field_type);
	return add_constant(context, constant);
}

unsigned int java_class_ref_method(java_class context,
                                   const char* class_name,
                                   const char* method_name,
                                   const char* method_type)
{
	struct constant constant;
	constant.tag = CONSTANT_METHOD;
	constant.data.refs.a = java_class_ref_class(context, class_name);
	constant.data.refs.b = java_class_ref_name_and_type(context, method_name, method_type);
	return add_constant(context, constant);
}

void java_class_write(java_class source, FILE* of)
{
	size_t index;
	size_t this_class = java_class_ref_class(source, source->name);
	size_t super_class = java_class_ref_class(source, source->super_name);
	
	write_32(0xcafebabe, of); /* Magic number. */
	write_16(0, of); /* Minor version. */
	write_16(51, of); /* Major version. */

	write_16(source->constant_count + 1, of);
	
	for (index = 0; index < source->constant_count; index++)
		write_constant(source->constants[index], of);

	write_16(source->access_flags, of); /* Access flags (public final). */

	write_16(this_class, of); /* This class. */
	write_16(super_class, of); /* Super class. */

	write_16(0, of); /* Interfaces count. */
	write_16(0, of); /* Fields count. */
	                
	write_16(0, of); /* Methods count. */

	write_16(0, of); /* Class attributes count. */
}
