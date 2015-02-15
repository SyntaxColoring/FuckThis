#include "java.h"

#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX_CONSTANTS 50

enum java_constant_tag
{
	CONSTANT_UTF8          = 1,
	CONSTANT_CLASS         = 7,
	CONSTANT_FIELD         = 9,
	CONSTANT_METHOD        = 10,
	CONSTANT_NAME_AND_TYPE = 12
};

/* Represents any kind of constant in the class's runtime constant pool. */
struct java_constant
{
	enum java_constant_tag tag;
	
	union
	{
		const char* utf8; /* For CONSTANT_Utf8_info constants. */
		
		/* For other kinds of constants.  a and b are general-purpose references
		   to other locations in the runtime constant pool.  Their exact purpose
		   depends on the type of constant. */
		struct
		{
			unsigned int a;
			unsigned int b; /* b may be left unused. */
		} refs;
	} data;
};

struct java_file
{
	size_t constant_count;
	struct java_constant constants[MAX_CONSTANTS];
	
	struct java_class class;
};

struct java_file* java_create(void)
{
	struct java_file* new_file = malloc_or_die(sizeof(struct java_file));
	new_file->constant_count = 0;
	return new_file;
}

void java_free(struct java_file* const file)
{
	free(file);
}

struct java_class* java_get_class(struct java_file* const file)
{
	return &file->class;
}

/* Performs a shallow equality check on the given constants.  This assumes both
   constants reside, or will reside, in the same constant pool. */
static bool constants_equal(const struct java_constant a, const struct java_constant b)
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

static unsigned int add_constant(struct java_file* destination,
                                 struct java_constant new_constant)
{
	size_t index;
	
	/* No more room to add another constant; return an invalid reference. */
	if (destination->constant_count >= MAX_CONSTANTS) return 0;
		
	/* Return a reference to an existing equivalent constant, if one exists. */
	for (index = 0; index < destination->constant_count; index++)
		if (constants_equal(new_constant, destination->constants[index]))
			return index + 1;
	
	/* Otherwise, add a new constant and return a reference to that. */
	destination->constants[destination->constant_count++] = new_constant;
	return destination->constant_count;
}

unsigned int java_ref_utf8(struct java_file* context, const char* string)
{
	struct java_constant constant;
	constant.tag = CONSTANT_UTF8;
	constant.data.utf8 = string;
	return add_constant(context, constant);
}

unsigned int java_ref_class(struct java_file* context, const char* name)
{
	struct java_constant constant;
	constant.tag = CONSTANT_CLASS;
	constant.data.refs.a = java_ref_utf8(context, name);
	return add_constant(context, constant);
}

unsigned int java_ref_name_and_type(struct java_file* context,
                                    const char* name,
                                    const char* type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_NAME_AND_TYPE;
	constant.data.refs.a = java_ref_utf8(context, name);
	constant.data.refs.b = java_ref_utf8(context, type);
	return add_constant(context, constant);
}

unsigned int java_ref_field(struct java_file* context,
                            const char* class_name,
                            const char* field_name,
                            const char* field_type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_FIELD;
	constant.data.refs.a = java_ref_class(context, class_name);
	constant.data.refs.b = java_ref_name_and_type(context, field_name, field_type);
	return add_constant(context, constant);
}

unsigned int java_ref_method(struct java_file* context,
                             const char* class_name,
                             const char* method_name,
                             const char* method_type)
{
	struct java_constant constant;
	constant.tag = CONSTANT_METHOD;
	constant.data.refs.a = java_ref_class(context, class_name);
	constant.data.refs.b = java_ref_name_and_type(context, method_name, method_type);
	return add_constant(context, constant);
}

void java_write(struct java_file* const file, FILE* const stream)
{
	size_t index;
	const struct java_class class = *java_get_class(file);
	struct byte_buffer class_buffer = BYTE_BUFFER_EMPTY;
	struct byte_buffer file_buffer = BYTE_BUFFER_EMPTY; 
	
	bb_write_u2(&class_buffer, class.access_flags);
	bb_write_u2(&class_buffer, java_ref_class(file, class.name));
	bb_write_u2(&class_buffer, java_ref_class(file, class.super_name));
	bb_write_u2(&class_buffer, 0); /* Interfaces count. */
	bb_write_u2(&class_buffer, 0); /* Fields count. */
	bb_write_u2(&class_buffer, class.method_count);
	/* To do: Write methods if there are any. */
	bb_write_u2(&class_buffer, 0); /* Class attributes count. */
	
	bb_write_u4(&file_buffer, 0xcafebabe); /* Magic number. */
	bb_write_u2(&file_buffer, 0); /* Minor version. */
	bb_write_u2(&file_buffer, 51); /* Major version. */
	bb_write_u2(&file_buffer, file->constant_count + 1);
	for (index = 0; index < file->constant_count; index++)
		write_constant(file->constants[index], &file_buffer);
	bb_write_array(&file_buffer, class_buffer.data, class_buffer.length);
	
	bb_write_to_stream(&file_buffer, stream);
}
