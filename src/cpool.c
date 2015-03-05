#include "cpool.h"

#include <stdlib.h>
#include <string.h> /* For strcmp(). */
#include "util.h"
#include "buffer.h"

#define MAX_CONSTANTS 32

/* Broadly, the constant types we support fall into two categories: UTF-8
   constants, and constants that simply wrap references to other constants.
   This tagged union type supports them both. */
struct constant
{
	enum {
		TYPE_UTF8          = 1,
		TYPE_CLASS         = 7,
		TYPE_FIELD         = 9,
		TYPE_METHOD        = 10,
		TYPE_NAME_AND_TYPE = 12
	} type;
	
	union {
		const char* utf8;
		
		/* The second reference is left unused for TYPE_CLASS. */
		constant_reference references[2];
	} data;
};

struct constant_pool_implementation
{
	struct constant constants[MAX_CONSTANTS];
	size_t constant_count;
};

constant_pool constant_pool_create()
{
	return malloc_or_die(sizeof(struct constant_pool_implementation));
}

void constant_pool_free(constant_pool pool)
{
	free(pool);
}

void constant_pool_write(buffer destination, constant_pool pool)
{
	size_t index;
	
	buffer_write_2(destination, pool->constant_count + 1);
	
	for (index = 0; index < pool->constant_count; index++)
	{		
		struct constant constant = pool->constants[index];
		
		buffer_write_1(destination, constant.type);
		
		switch (constant.type)
		{
			case TYPE_UTF8:
			{
				const size_t length = strlen(constant.data.utf8);
				buffer_write_2(destination, length);
				buffer_write_array(destination, (const buffer_u1*)constant.data.utf8, length);
				break;
			}
			case TYPE_CLASS:
				buffer_write_2(destination, constant.data.references[0]);
				break;
			case TYPE_FIELD:
			case TYPE_METHOD:
			case TYPE_NAME_AND_TYPE:
				buffer_write_2(destination, constant.data.references[0]);
				buffer_write_2(destination, constant.data.references[1]);
				break;
			default:
				fatal_error("Unknown constant type");
		}
	}
}

/* Returns whether or not a is equal to be.  A deep comparison is performed
   between UTF-8 constants, but a shallow comparison is perfomed for all other
   constant types (the value of their references is compared). */
static bool constants_equal(struct constant a, struct constant b)
{
	if (a.type != b.type) return false;
	
	switch (a.type)
	{
		case TYPE_UTF8:
			return !strcmp(a.data.utf8, b.data.utf8);
		case TYPE_CLASS:
			return (a.data.references[0] == b.data.references[0]);
		case TYPE_FIELD:
		case TYPE_METHOD:
		case TYPE_NAME_AND_TYPE:
			return (a.data.references[0] == b.data.references[0] &&
			        a.data.references[1] == b.data.references[1]);
		default:
			return false; /* Unknown constant type. */
	}
}

/* Returns a reference to a constant in pool that compares equal to new_constant.
   If no such constant exists, one is created.  0 is returned if there is an
   error (constant references are 1-based indices). */
static constant_reference add_constant(constant_pool pool, struct constant new_constant)
{
	/* Return a reference to an equivalent constant, if one exists. */
	size_t index;
	
	for (index = 0; index < MAX_CONSTANTS; index++)
		if (constants_equal(pool->constants[index], new_constant))
			return index + 1;
	
	/* Otherwise, create a new constant and return a reference to that. */
	if (pool->constant_count == MAX_CONSTANTS)
		return 0; /* Too many constants. */
	else
	{
		pool->constants[pool->constant_count++] = new_constant;
		return pool->constant_count;
	}
}

constant_reference constant_utf8(
	constant_pool pool,
	const char* utf8)
{
	struct constant utf8_constant;
	utf8_constant.type = TYPE_UTF8;
	utf8_constant.data.utf8 = utf8;
	return add_constant(pool, utf8_constant);
}

constant_reference constant_class(
	constant_pool pool,
	const char* class_name)
{
	struct constant class_constant;
	class_constant.type = TYPE_CLASS;
	class_constant.data.references[0] = constant_utf8(pool, class_name);
	
	return add_constant(pool, class_constant);
}

constant_reference constant_field(
	constant_pool pool,
	const char* class_name,
	const char* field_name,
	const char* field_type)
{
	struct constant field_constant;
	field_constant.type = TYPE_FIELD;
	field_constant.data.references[0] = constant_class(pool, class_name);
	field_constant.data.references[1] = constant_name_and_type(pool, field_name, field_type);
	
	return add_constant(pool, field_constant);
}

constant_reference constant_method(
	constant_pool pool,
	const char* class_name,
	const char* method_name,
	const char* method_type)
{
	struct constant method_constant;
	method_constant.type = TYPE_METHOD;
	method_constant.data.references[0] = constant_class(pool, class_name);
	method_constant.data.references[1] = constant_name_and_type(pool, method_name, method_type);
	
	return add_constant(pool, method_constant);
}

constant_reference constant_name_and_type(
	constant_pool pool,
	const char* name,
	const char* type)
{
	struct constant name_and_type_constant;
	name_and_type_constant.type = TYPE_NAME_AND_TYPE;
	name_and_type_constant.data.references[0] = constant_utf8(pool, name);
	name_and_type_constant.data.references[1] = constant_utf8(pool, type);
	
	return add_constant(pool, name_and_type_constant);
}

