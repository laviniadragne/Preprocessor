#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
// #include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */

#include "hashtable.h"

#define INPUT_EXTENSION ".in"
#define INPUT_PARTICLE "in"
#define OUTPUT_EXTENSION ".out"
#define INPUTS "inputs"
#define OUTPUTS "outputs"
#define MAX_LEN 256
#define MAX_LEN_H 1000
#define DEFINE_TAG "#define"
#define INCLUDE_TAG "#include"
#define UNDEF_TAG "#undef"
#define IF_TAG "#if"
#define ENDIF_TAG "#endif"
#define ELIF_TAG "#elif"
#define ELSE_TAG "#else"
#define IFDEF_TAG "#ifdef"
#define IFNDEF_TAG "#ifndef"
#define COPY_INPUT_FILE "copy_input"
#define MIDDLE_STRING 2
#define MAX_INCLUDES 10
#define WRONG_COMMAND 10
#define WRONG_FILE 8
#define WRONG_CLOSED_FILE 9
#define LEN_HEADER 1000
#define LEN_INPUT 5000
#define TRUE 1
#define FALSE 0

char* construct_out_file(char* input_file, char* output_file);
void replace(char* line, char* to_replace, char* replacement, char* new_line);
int process_fragment(char* buff_out, char* frag, char* delim_line, HashTable* ht);
void process_multi_line_def(char* text_value, char* processed_text);
int simply_write(char* buffer, FILE* fout, HashTable* ht);