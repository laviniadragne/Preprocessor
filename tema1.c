#include "functions.h"

int main(int argc, char *argv[])
{
	int i, err, len_include_paths = 0;
	int flag_stdin = 0, flag_stdout = 0, flag_include = 1;
	char **include_paths = NULL;
	char *file, *path, *include_input, *input_file = NULL,
					   *output_file = NULL;
	const char point = '.';
	char buffer[MAX_LEN];
	HashTable *ht;
	FILE *fin = NULL, *fout = NULL;

	ht = malloc(sizeof(HashTable));
	if (ht == NULL)
		return -ENOMEM;

	err = init_ht(ht, MAX_BUCKETS);
	if (err != SUCCESS) {
		free(ht);
		return err;
	}

	include_paths = malloc(argc * sizeof(char *));
	if (include_paths == NULL) {
		free_ht(ht);
		return -ENOMEM;
	}

	// parsare argumente
	for (i = 1; i < argc; i++) {
		// daca ar trebui sa fie un flag
		if (argv[i][0] == '-') {
			err = parse_arguments(argv, ht, &i, &include_input,
					      &include_paths,
					      &len_include_paths, &output_file);
			if (err != SUCCESS)
				return err;
		} else {
			// poate fi fisier .out sau fisier .in
			file = malloc((strlen(argv[i]) + 1) * sizeof(char));
			if (file == NULL) {
				free_ht(ht);
				free(include_paths);
				return -ENOMEM;
			}
			strcpy(file, argv[i]);

			// verific daca este INFILE-ul (ar trebui sa aiba .in)
			if (strcmp(strchr(file, point), INPUT_EXTENSION) == 0) {
				input_file =
				    malloc((strlen(file) + 1) * sizeof(char));
				if (input_file == NULL) {
					free_ht(ht);
					free(include_paths);
					free(file);
					return -ENOMEM;
				}
				strcpy(input_file, file);
			} else {
				// e outfile-ul
				if (strcmp(strchr(file, point),
					   OUTPUT_EXTENSION) == 0) {
					output_file = malloc(
					    (strlen(file) + 1) * sizeof(char));
					if (output_file == NULL) {
						free_ht(ht);
						free(include_paths);
						free(file);
						return -ENOMEM;
					}
					strcpy(output_file, file);
				} else
					return WRONG_COMMAND;
			}
			free(file);
		}
	}

	path = malloc(MAX_LEN * sizeof(char));
	if (path == NULL) {
		free_ht(ht);
		free(input_file);
		free(output_file);
		free_include_paths(include_paths, len_include_paths);
		return -ENOMEM;
	}

	err = open_input_file(input_file, output_file, &flag_stdin, ht,
			      include_paths, len_include_paths, path, &fin);

	if (err != SUCCESS)
		return err;

	err = open_output_file(output_file, &flag_stdout, input_file, ht,
			       include_paths, len_include_paths, &fin, &fout);

	if (err != SUCCESS)
		return err;

	err = call_include_process(path, include_paths, len_include_paths,
				   input_file, output_file, flag_include,
				   flag_stdin, fin, fout, ht);

	if (err != SUCCESS)
		return err;

	err = construct_output_file(buffer, fin, fout, ht);

	free_all(output_file, input_file, path, include_paths,
		 len_include_paths, fin, fout, ht);

	return err;
}
