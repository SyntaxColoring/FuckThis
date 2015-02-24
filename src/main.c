#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "java.h"
#include "codegen.h"

int main(int argument_count, char** arguments)
{
	if (argument_count != 3 ||
	    (argument_count >= 2 &&
	     (!strcmp(arguments[1], "-h") ||
	      !strcmp(arguments[1], "--help"))))
	{
		printf("The FuckJava Brainfuck compiler\n"
		       "https://github.com/SyntaxColoring/FuckJava\n"
		       "Usage:\n"
		       "  %s input_file class_name\n"
		       "Arguments:\n"
		       "  input_file  - The name of the Brainfuck source file.\n"
		       "  class_name - The name of the Java class to generate.\n",
		       arguments[0]);
		return EXIT_SUCCESS;
	}
	
	else
	{
		char* output_filename = malloc(strlen(arguments[2]) + sizeof(".class") - 1);
		FILE* input = fopen(arguments[1], "r");
		FILE* output;
		
		strcpy(output_filename, arguments[2]);
		strcat(output_filename, ".class"); 
		output = fopen(output_filename, "w");
		
		if (!output || !input)
		{
			perror("Error opening input or output file");
			return EXIT_FAILURE;
		}
		
		else
		{
			java_file file = java_create();
			struct java_class* class = java_get_class(file);
			
			class->name = arguments[2];
			class->super_name = "java/lang/Object";
			class->access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_FINAL;
			class->method_count = 1;
			class->methods = malloc(sizeof(struct java_method));
			class->methods[0].name = "main";
			class->methods[0].type = "([Ljava/lang/String;)V";
			class->methods[0].access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_STATIC;
			generate_code(file, 0, input);
			java_write(file, output);
			
			return EXIT_SUCCESS;
		}
	}
}
