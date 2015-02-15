#ifndef CODEGEN_H_INCLUDED
#define CODEGEN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "java.h"

void generate_code(struct java_file* const file, const size_t method_index, FILE* source);

#endif
