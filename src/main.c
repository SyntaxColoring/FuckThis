#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "java.h"
#include "util.h"

int main(void)
{
	struct java_file* file = java_create();
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
		class->method_count = 0;
		java_write(file, of);
	}
	
	return EXIT_SUCCESS;
}
