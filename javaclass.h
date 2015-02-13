#ifndef JAVACLASS_H_INCLUDED
#define JAVACLASS_H_INCLUDED

#include <stdio.h>

typedef struct java_class_impl* java_class;

extern const unsigned int JAVA_CLASS_PUBLIC;
extern const unsigned int JAVA_CLASS_FINAL;
extern const unsigned int JAVA_CLASS_STATIC;

java_class java_class_create(const char* name,
                             const char* super_name,
                             unsigned int access_flags);

void java_class_write(java_class source, FILE* destination);

unsigned int java_class_ref_utf8(java_class context,
                                 const char* string);

unsigned int java_class_ref_class(java_class context,
                                  const char* name);

unsigned int java_class_ref_name_and_type(java_class context,
                                          const char* name,
                                          const char* type);

unsigned int java_class_ref_field(java_class context,
                                  const char* class_name,
                                  const char* field_name,
                                  const char* field_type);

unsigned int java_class_ref_method(java_class context,
                                   const char* class_name,
                                   const char* method_name,
                                   const char* method_type);

#endif
