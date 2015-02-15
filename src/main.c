#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "java.h"
#include "util.h"

int main(void)
{
	struct java_class class;
	FILE* of = fopen("FuckJava.class", "w");
	FILE* source = fopen("FuckJava.bf", "r");
	
	if (!of || !source)
	{
		perror("Error creating file");
	}
	
	else
	{
		class.name = "FuckJava";
		class.super_name = "java/lang/Object";
		class.access_flags = JAVA_CLASS_PUBLIC | JAVA_CLASS_FINAL;
		class.constant_count = 0;
		class.constants = NULL;
		class.method_count = 0;
		java_class_write(&class, of);
	}
}
