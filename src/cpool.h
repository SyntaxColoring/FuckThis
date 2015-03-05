/* cpool.h - A structure for building a Java class's runtime constant pool. */

#ifndef CPOOL_H_INCLUDED
#define CPOOL_H_INCLUDED

#include "buffer.h"

/* Opaque pointer to a type representing a constant pool. */
typedef struct constant_pool_implementation* constant_pool;

/* An unsigned integral type that can hold any constant pool reference. */
typedef unsigned int constant_reference;

constant_pool constant_pool_create();
void constant_pool_free(constant_pool pool);

/* Writes the constant pool to destination.. */
void constant_pool_write(buffer destination, constant_pool pool);

/* Functions for obtaining references to constants.  If there is an equivalent
   constant in the given constant pool, a reference to it will be returned.
   Otherwise, a new entry will be created for it.  Once a reference is obtained,
   it remains valid until the constant pool is freed.  If a string passed to
   one of these functions is changed or freed before the constant pool is freed,
   the behavior is undefined. */

constant_reference constant_utf8(
	constant_pool pool,
	const char* utf8);

constant_reference constant_class(
	constant_pool pool,
	const char* class_name);

constant_reference constant_field(
	constant_pool pool,
	const char* class_name,
	const char* field_name,
	const char* field_type);

constant_reference constant_method(
	constant_pool pool,
	const char* class_name,
	const char* method_name,
	const char* method_type);

constant_reference constant_name_and_type(
	constant_pool pool,
	const char* name,
	const char* type);

#endif
