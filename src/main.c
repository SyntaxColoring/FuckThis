#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "java.h"
#include "codegen.h"

static const char* const help_string =
	"Fuck This: a Brainfuck compiler that produces Java binaries.\n"
	"https://github.com/SyntaxColoring/FuckThis\n"
	"Usage:\n"
	"  ft input_file class_name\n"
	"Arguments:\n"
	"  input_file - The name of the Brainfuck source file.\n"
	"  class_name - The name of the Java class to generate.\n";

static bool user_needs_help(size_t argument_count, char** arguments)
{
	size_t index;
	for (index = 1; index < argument_count; index++)
		if (!strcmp(arguments[index], "-h") || !strcmp(arguments[index], "--h"))
			return true;
	return false;
}

int main(int argument_count, char** arguments)
{
	if (user_needs_help(argument_count, arguments) || argument_count != 3)
	{
		printf(help_string);
		return EXIT_SUCCESS;
	}
	
	else
	{
		const char* const source_filename = arguments[1];
		const char* const class_name = arguments[2];
		
		char* output_filename;
		FILE* source;
		FILE* output;
		java_file file;
		struct java_class* class;
		
		output_filename = malloc_or_die(strlen(class_name)+sizeof(".class")-1);
		strcpy(output_filename, class_name);
		strcat(output_filename, ".class");
		
		source = fopen(source_filename, "r");
		if (!source)
		{
			perror("Couldn't open source file");
			return EXIT_FAILURE;
		}
		
		output = fopen(output_filename, "w");
		if (!output)
		{
			perror("Couldn't open output file for writing");
			return EXIT_FAILURE;
		}
		
		file = java_create();
		class = java_get_class(file);
		
		class->name = class_name;
		class->super_name = "java/lang/Object";
		class->access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_FINAL;
		
		class->method_count = 1;
		class->methods = malloc_or_die(sizeof(struct java_method));
		class->methods[0].name = "main";
		class->methods[0].type = "([Ljava/lang/String;)V";
		class->methods[0].access_flags = JAVA_ACCESS_PUBLIC | JAVA_ACCESS_STATIC;
		
		generate_code(file, 0, source);
		
		java_write(file, output);
		
		free(class->methods);
		java_free(file);
		free(output_filename);
		
		return EXIT_SUCCESS;
	}
}
