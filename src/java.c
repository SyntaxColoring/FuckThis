#include "java.h"

#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "buffer.h"

struct java_file_opaque
{
	constant_pool constants;
	
	struct java_class class;
};

java_file java_create(void)
{
	java_file new_file = malloc_or_die(sizeof(struct java_file_opaque));
	new_file->constants = constant_pool_create();
	return new_file;
}

void java_free(java_file file)
{
	constant_pool_free(file->constants);
	free(file);
}

struct java_class* java_get_class(java_file const file)
{
	return &file->class;
}

constant_pool java_get_constant_pool(java_file const file)
{
	return file->constants;
}

static void write_method(java_file file, const struct java_method method, buffer destination)
{
	buffer_write_2(destination, method.access_flags);
	buffer_write_2(destination, constant_utf8(java_get_constant_pool(file), method.name));
	buffer_write_2(destination, constant_utf8(java_get_constant_pool(file), method.type));
	buffer_write_2(destination, 1); /* Attributes count (just the code attribute). */
	
	buffer_write_2(destination, constant_utf8(java_get_constant_pool(file), "Code"));
	buffer_write_4(destination, 12 + method.bytecode_length);
	buffer_write_2(destination, method.max_stack);
	buffer_write_2(destination, method.max_locals);
	buffer_write_4(destination, method.bytecode_length);
	buffer_write_array(destination, method.bytecode, method.bytecode_length);
	buffer_write_2(destination, 0); /* Exception table length. */
	buffer_write_2(destination, 0); /* Number of sub-attributes of the code attribute. */
}

void java_write(java_file const file, FILE* const stream)
{
	size_t index;
	const struct java_class class = *java_get_class(file);
	constant_pool constants = java_get_constant_pool(file);
	buffer file_buffer = buffer_create(1024);
	buffer after_constant_pool = buffer_create(1024);
	
	buffer_write_2(after_constant_pool, class.access_flags);
	buffer_write_2(after_constant_pool, constant_class(constants, class.name));
	buffer_write_2(after_constant_pool, constant_class(constants, class.super_name));
	buffer_write_2(after_constant_pool, 0); /* Interfaces count. */
	buffer_write_2(after_constant_pool, 0); /* Fields count. */
	buffer_write_2(after_constant_pool, class.method_count);
	for (index = 0; index < class.method_count; index++)
		write_method(file, class.methods[index], after_constant_pool);
	buffer_write_2(after_constant_pool, 0); /* Class attributes count. */
	
	buffer_write_4(file_buffer, 0xcafebabe); /* Magic number. */
	buffer_write_2(file_buffer, 0); /* Minor version. */
	buffer_write_2(file_buffer, 49); /* Major version. */
	constant_pool_write(file_buffer, constants);
	buffer_write_buffer(file_buffer, after_constant_pool);
	
	buffer_write_to_stream(file_buffer, stream);
}
