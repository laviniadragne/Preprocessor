#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define INPUTS "inputs"
#define OUTPUTS "outputs"
#define INPUT_EXTENSION ".in"
#define INPUT_PARTICLE "in"
#define OUTPUT_EXTENSION ".out"
#define ZERO_STR "0"
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
#define DELIM_DEFINE "' '\n"
#define MIDDLE_STRING 2
#define WRONG_FILE 8
#define WRONG_CLOSED_FILE 9
#define WRONG_COMMAND 10
#define WRONG_OPEN_FILE 11
#define MAX_LEN 256
#define TRUE 1
#define FALSE 0

void replace(char *line, char *to_replace, char *replacement, char *new_line);

int process_fragment(char *buff_out, char *frag, char *delim_line,
		     HashTable *ht);

void process_multi_line_def(char *text_value, char *processed_text);

int construct_path(char *input_file, char *path);

void construct_include_path(char *include_path, char *path, char *include_file);

int append_all_header_file(FILE *f_h, FILE *fout, int *flag_input);

void undef_func(char *buffer, HashTable *ht);

void erase_tag(char *buffer, char *processed_buf, char *tag);

int define_process(char *buffer, FILE *fin, HashTable *ht);

int verify_one_line(char *buffer, FILE *fin, FILE *fout, HashTable *ht);

int if_process(char *buffer, FILE *fin, FILE *fout, HashTable *ht);

int include_process(char *path, int len_include_paths, char **include_paths,
		    char *input_file, FILE *fin, int *input_flag);

int update_input_file(FILE *copy_fin, FILE *fin, char *input_file,
		      char *buffer);

int ifdef_process(char *buffer, char *processed_buf, char *tag, HashTable *ht,
		  FILE *fin, FILE *fout, int last_cond_executed);

int simply_write(char *buffer, FILE *fout, HashTable *ht);

void free_include_paths(char **include_paths, int len_include_paths);

void free_all(char *output_file, char *input_file, char *path,
	      char **include_paths, int len_include_paths, FILE *fin,
	      FILE *fout, HashTable *ht);

int define_argument(HashTable *ht, char *argv[], int *i);

void include_argument(char *argv[], char *include_input, int *i);

void output_argument(char *argv[], char *output_file, int *i);

int construct_output_file(char *buffer, FILE *fin, FILE *fout, HashTable *ht);

int call_include_process(char *path, char **include_paths,
			 int len_include_paths, char *input_file,
			 char *output_file, int flag_include, int flag_stdin,
			 FILE *fin, FILE *fout, HashTable *ht);

int open_input_file(char *input_file, char *output_file, int *flag_stdin,
		    HashTable *ht, char **include_paths, int len_include_paths,
		    char *path, FILE **fin);

int open_output_file(char *output_file, int *flag_stdout, char *input_file,
		     HashTable *ht, char **include_paths, int len_include_paths,
		     FILE **fin, FILE **fout);

int process_inside_ifdef(char *buffer, FILE *fin, FILE *fout, HashTable *ht);

int construct_output_file(char *buffer, FILE *fin, FILE *fout, HashTable *ht);

int ifdef_ifndef_processing(char *buffer, HashTable *ht, FILE *fin, FILE *fout);

int call_recursive_ifdef_process(char *buffer, char *processed_buf,
				 HashTable *ht, FILE *fin, FILE *fout,
				 int last_cond_executed);

int parse_arguments(char *argv[], HashTable *ht, int *i, char **include_input,
		    char ***include_paths, int *len_include_paths,
		    char **output_file);
