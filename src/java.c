#include "java.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"

/* Represents any kind of constant in the class's runtime constant pool. */
struct java_constant
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

const unsigned int JAVA_CLASS_PUBLIC = 0x0001;
const unsigned int JAVA_CLASS_STATIC = 0x0008;
const unsigned int JAVA_CLASS_FINAL  = 0x0010;

/* Returns nonzero if a is logically equal to b, assuming both a and b reside
   (or will reside) in the same constant pool. */
static int constants_equal(const struct java_constant a, const struct java_constant b)
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

static void write_constant(const struct java_constant constant, struct byte_buffer* const buffer)
{	
	bb_write_u1(buffer, constant.tag);
	
	if (constant.tag == CONSTANT_UTF8)
	{
		const size_t length = strlen(constant.data.utf8);
		bb_write_u2(buffer, length);
		bb_write_array(buffer, (u1*)constant.data.utf8, length);
	}
	
	else
	{
		bb_write_u2(buffer, constant.data.refs.a);
		if (constant.tag != CONSTANT_CLASS)
			bb_write_u2(buffer, constant.data.refs.b);
	}
}

static void write_method(struct java_class* class, size_t method_index, struct byte_buffer* const buffer)
{
	const struct java_method method = class->methods[method_index];
	bb_write_u2(buffer, method.access_flags);
	bb_write_u2(buffer, java_class_ref_utf8(class, method.name));
	bb_write_u2(buffer, java_class_ref_utf8(class, method.type));
	bb_write_u2(buffer, 0);
}

static unsigned int add_constant(struct java_class* destination,
                                 struct java_constant new_constant)
{
	size_t index;
	
	if (!destination->constants)
		destination->constants = malloc(sizeof(struct java_constant)*50);
		
	/* Return a reference to an existing equivalent constant, if one exists. */
	for (index = 0; index < destination->constant_count; index++)
		if (constants_equal(new_constant, destination->constants[index]))
			return index + 1;
	
	/* Otherwise, add a new constant and return a reference to that. */
	destination->constants[destination->constant_count++] = new_constant;
	return destination->constant_count;
}

unsigned int java_class_ref_utf8(struct java_class* context, const char* string)
{
	struct java_constant constant;
	constant.tag = CONSTANT_UTF8;
	constant.data.utf8 = string;
	return add_constant(context, constant);
}

unsigned int java_class_ref_class(struct java_class* context, const char* name)
{
	struct java_constant constant;
	constant.tag = CONSTANT_CLASS;
	constant.data.refs.a = java_class_ref_utf8(context, name);
	return add_constant(context, constant);
}

unsigned int java_class_ref_name_and_type(struct java_class* context,
                                          const char* name,
                                          const char* type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_NAME_AND_TYPE;
	constant.data.refs.a = java_class_ref_utf8(context, name);
	constant.data.refs.b = java_class_ref_utf8(context, type);
	return add_constant(context, constant);
}

unsigned int java_class_ref_field(struct java_class* context,
                                  const char* class_name,
                                  const char* field_name,
                                  const char* field_type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_FIELD;
	constant.data.refs.a = java_class_ref_class(context, class_name);
	constant.data.refs.b = java_class_ref_name_and_type(context, field_name, field_type);
	return add_constant(context, constant);
}

unsigned int java_class_ref_method(struct java_class* context,
                                   const char* class_name,
                                   const char* method_name,
                                   const char* method_type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_METHOD;
	constant.data.refs.a = java_class_ref_class(context, class_name);
	constant.data.refs.b = java_class_ref_name_and_type(context, method_name, method_type);
	return add_constant(context, constant);
}

void java_class_write(struct java_class* source, FILE* of)
{
	size_t index;
	struct byte_buffer entire_file = BYTE_BUFFER_EMPTY;
	struct byte_buffer after_constant_pool = BYTE_BUFFER_EMPTY; 
	
	bb_write_u2(&after_constant_pool, source->access_flags);
	bb_write_u2(&after_constant_pool, java_class_ref_class(source, source->name));
	bb_write_u2(&after_constant_pool, java_class_ref_class(source, source->super_name));
	bb_write_u2(&after_constant_pool, 0); /* Interfaces count. */
	bb_write_u2(&after_constant_pool, 0); /* Fields count. */
	bb_write_u2(&after_constant_pool, source->method_count);
	for (index = 0; index < source->method_count; index++)
		write_method(source, index, &after_constant_pool);
	bb_write_u2(&after_constant_pool, 0); /* Class attributes count. */
	
	bb_write_u4(&entire_file, 0xcafebabe); /* Magic number. */
	bb_write_u2(&entire_file, 0); /* Minor version. */
	bb_write_u2(&entire_file, 51); /* Major version. */
	bb_write_u2(&entire_file, source->constant_count + 1);
	for (index = 0; index < source->constant_count; index++)
		write_constant(source->constants[index], &entire_file);
	bb_write_array(&entire_file, after_constant_pool.data, after_constant_pool.length);
	
	bb_write_to_stream(&entire_file, of);
}
