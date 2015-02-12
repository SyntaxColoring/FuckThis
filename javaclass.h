#ifndef JAVACLASS_H_INCLUDED
#define JAVACLASS_H_INCLUDED

#include <stdio.h>

typedef struct java_class_impl* java_class;
typedef unsigned int constant_ref;

java_class java_class_create(const char* name, const char* super_class, unsigned int access_flags);

void java_class_write(java_class source, FILE* destination);

constant_ref java_class_ref_utf8(java_class context,
                                 const char* string);

constant_ref java_class_ref_class(java_class context,
                                  const char* name);

constant_ref java_class_ref_name_and_type(java_class context,
                                          const char* name,
                                          const char* type);

constant_ref java_class_ref_field(java_class_context,
                                  const char* class_name,
                                  const char* field_name,
                                  const char* field_type);

constant_ref java_class_ref_method(java_class_context,
                                   const char* class_name,
                                   const char* method_name,
                                   const char* method_type);

#endif
