#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "util.h"
#include "java.h"
#include "codegen.h"

int main(void)
{
	java_file file = java_create();
	FILE* of = fopen("FuckJava.class", "w");
	FILE* source = fopen("FuckJava.bf", "r");
	
	if (!of || !source)
	{
		perror("Error creating file");
		return EXIT_FAILURE;
	}
	
	else
	{
		struct java_class* class = java_get_class(file);
		
		class->name = "FuckJava";
		class->super_name = "java/lang/Object";
		class->access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_FINAL;
		class->method_count = 1;
		class->methods = malloc(sizeof(struct java_method));
		class->methods[0].name = "main";
		class->methods[0].type = "([Ljava/lang/String;)V";
		class->methods[0].access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_STATIC;
		generate_code(file, 0, source);
		java_write(file, of);
	}
	
	return EXIT_SUCCESS;
}
