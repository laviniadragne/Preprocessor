#include "functions.h"

// Inlocuieste in variabila line, textul to_replace cu textul replacement si
// il returneaza in new_line
void replace(char *line, char *to_replace, char *replacement, char *new_line)
{
	char *pos;

	pos = strstr(line, to_replace);

	// if found
	if (pos != NULL) {
		// Copy the part of the old sentence *before* the replacement
		memcpy(new_line, line, pos - line);

		// Copy the replacement
		memcpy(new_line + (pos - line), replacement,
		       strlen(replacement));

		// Copy the rest
		strcpy(new_line + (pos - line) + strlen(replacement),
		       pos + strlen(to_replace));
	}
}

// Proceseaza o linie pentru a fi scrisa in fisierul de out
// verifica daca vreun cuvant din linie trebuie inlocuit
// cu definitia specifica
int process_fragment(char *buff_out, char *frag, char *delim_line,
		     HashTable *ht)
{

	char *copy_frag;
	char *word;
	char *new_frag;

	// delimitez pe cuvinte
	copy_frag = calloc(MAX_LEN, sizeof(char));
	if (copy_frag == NULL)
		return -ENOMEM;

	strcpy(copy_frag, frag);

	word = strtok(frag, delim_line);
	new_frag = calloc(MAX_LEN, sizeof(char));
	if (new_frag == NULL) {
		free(copy_frag);
		return -ENOMEM;
	}
	strcpy(new_frag, copy_frag);

	while (word != NULL) {
		// daca e in dictionar
		if (has_key(ht, word) == 1) {
			replace(copy_frag, word, get_value(ht, word), new_frag);
			strcpy(copy_frag, new_frag);
		}
		word = strtok(NULL, delim_line);
	}

	// alipesc new_frag
	strcat(buff_out, new_frag);

	free(copy_frag);
	free(new_frag);

	return SUCCESS;
}

// Proceseaza text_value si il returneaza schimbat in
// processed_text, fara tab-uri \ sau \n;
void process_multi_line_def(char *text_value, char *processed_text)
{
	// scapam de tab, \ si \n
	strcpy(processed_text, text_value);
	replace(text_value, "        ", "", processed_text);
	strcpy(text_value, processed_text);
	replace(text_value, "\\", "", processed_text);
	strcpy(text_value, processed_text);
	replace(text_value, "\n", "", processed_text);
	strcat(processed_text, " ");
}

// Construieste calea catre fisierul de intrare
int construct_path(char *input_file, char *path)
{
	// sterg test.in
	char *new_path;
	char *token;
	char *copy_input = malloc(strlen(input_file) + 1);

	if (copy_input == NULL)
		return -ENOMEM;

	strcpy(copy_input, input_file);

	new_path = calloc(MAX_LEN, sizeof(char));
	if (new_path == NULL) {
		free(copy_input);
		return -ENOMEM;
	}
	token = strtok(copy_input, "/");

	strcpy(new_path, "/");
	while (token != NULL) {
		strcpy(new_path + 1, token);
		token = strtok(NULL, "/");
	}

	replace(input_file, new_path, "", path);

	free(new_path);
	free(copy_input);

	return SUCCESS;
}

// Construieste calea catre header, alipind
// la calea curenta numele headerului
void construct_include_path(char *include_path, char *path, char *include_file)
{
	// trebuie sa caut in directorul curent
	strcpy(include_path, path);
	strcat(include_path, "/");
	strcat(include_path, include_file);
}

// Apenduiesc tot continutul fisierului header in fisierul fout
int append_all_header_file(FILE *f_h, FILE *fout, int *flag_input)
{
	char *include_buff;
	char buff_header[MAX_LEN];

	include_buff = calloc(MAX_LEN, sizeof(char));
	if (include_buff == NULL)
		return -ENOMEM;

	while (fgets(buff_header, MAX_LEN, f_h)) {
		// daca mai am un include
		if (strstr(buff_header, INCLUDE_TAG) != NULL)
			*flag_input = 1;
		strcat(include_buff, buff_header);
	}
	fprintf(fout, "%s", include_buff);

	free(include_buff);
	return SUCCESS;
}

// Elimina variabila de la undef din dictionar
void undef_func(char *buffer, HashTable *ht)
{
	char parsed_buff[MAX_LEN];

	replace(buffer, UNDEF_TAG, "", parsed_buff);
	replace(parsed_buff, " ", "", buffer);
	replace(buffer, "\n", "", parsed_buff);

	// sterg cheia din dictionar
	if (has_key(ht, parsed_buff) == 1)
		remove_entry(ht, parsed_buff);
}

// Sterge un tag primit ca si parametru si
// intoarce linia noua in processed_buf
void erase_tag(char *buffer, char *processed_buf, char *tag)
{
	replace(buffer, tag, "", processed_buf);
	replace(processed_buf, " ", "", buffer);
	replace(buffer, "\n", "", processed_buf);
}

// Se ocupa de procesul de define
int define_process(char *buffer, FILE *fin, HashTable *ht)
{
	int err;
	char *copy_buf, *key, *value, *text_value, *processed_text;
	char *delim_define, *token, *delim_val;

	copy_buf = malloc((strlen(buffer) + 1) * sizeof(char));
	if (copy_buf == NULL)
		return -ENOMEM;

	strcpy(copy_buf, buffer);

	processed_text = calloc(MAX_LEN, sizeof(char));
	if (processed_text == NULL) {
		free(copy_buf);
		return -ENOMEM;
	}

	text_value = calloc(MAX_LEN, sizeof(char));
	if (text_value == NULL) {
		free(copy_buf);
		free(processed_text);
		return -ENOMEM;
	}

	value = calloc(MAX_LEN, sizeof(char));
	if (value == NULL) {
		free(copy_buf);
		free(processed_text);
		free(text_value);
		return -ENOMEM;
	}

	key = calloc(MAX_LEN, sizeof(char));
	if (key == NULL) {
		free(copy_buf);
		free(processed_text);
		free(text_value);
		free(value);
		return -ENOMEM;
	}
	// sar peste #define
	delim_define = DELIM_DEFINE;
	token = strtok(copy_buf + strlen(DEFINE_TAG), delim_define);
	if (token != NULL)
		strcpy(key, token);

	delim_val = "\n";
	token = strtok(NULL, delim_val);

	if (token != NULL)
		strcpy(text_value, token);
	else
		strcpy(text_value, "");

	// daca am text in text_value
	if (strlen(text_value) >= 1) {
		// define imbricat
		if (text_value[strlen(text_value) - 1] != '\\') {
			// trebuie sa vad daca am un #define in alt #define
			err = process_fragment(value, text_value, delim_define,
					       ht);
			if (err != SUCCESS)
				return err;

		}
		// define multilinie
		else {
			// elimin tab, \ si \n
			strcpy(processed_text, text_value);
			replace(text_value, " ", "", processed_text);
			strcpy(text_value, processed_text);

			// lipesc prima linie la value
			process_multi_line_def(text_value, processed_text);
			strcat(value, processed_text);

			while (fgets(buffer, MAX_LEN, fin)) {
				// daca s-a terminat multiline-ul
				if (buffer[strlen(buffer) - 2] != '\\') {
					strcpy(text_value, buffer);
					process_multi_line_def(text_value,
							       processed_text);
					strcat(value, processed_text);
					// scot " " de la final
					value[strlen(value) - 1] = '\0';

					fgets(buffer, MAX_LEN, fin);
					break;
				}
				strcpy(text_value, buffer);
				process_multi_line_def(text_value,
						       processed_text);
				strcat(value, processed_text);
			}
		}
	}

	// adaug (key, value) in hashtable
	err = put_value(ht, key, value);
	if (err != SUCCESS) {
		free(processed_text);
		free(copy_buf);
		free(key);
		free(value);
		free(text_value);
		return err;
	}

	free(processed_text);
	free(copy_buf);
	free(key);
	free(value);
	free(text_value);

	return SUCCESS;
}

// Verifica daca linia contine
// define sau undef sau nu contine nimic
// si trebuie doar sa o scrie in fout
int verify_one_line(char *buffer, FILE *fin, FILE *fout, HashTable *ht)
{
	int err;

	// daca am define in ea
	if (strstr(buffer, DEFINE_TAG) != NULL) {
		err = define_process(buffer, fin, ht);
		if (err != SUCCESS)
			return err;

	} else {
		// am undef
		if (strstr(buffer, UNDEF_TAG) != NULL)
			undef_func(buffer, ht);

		// doar trebuie sa o scriu
		else
			fprintf(fout, "%s", buffer);
	}

	return SUCCESS;
}

// Procesarea buffer-ului in cazul in care
// e o conditie de tip if
int if_process(char *buffer, FILE *fin, FILE *fout, HashTable *ht)
{
	int err;
	int flag_true_if;
	char processed_buf[MAX_LEN];

	erase_tag(buffer, processed_buf, IF_TAG);

	// vad daca e in dictionar
	if (has_key(ht, processed_buf) == 1)
		strcpy(processed_buf, get_value(ht, processed_buf));

	flag_true_if = 0;

	// daca conditia e adevarata
	if (strcmp(processed_buf, ZERO_STR) != 0) {
		// citesc urmatoarea linie si o scriu
		// nemodificata
		fgets(buffer, MAX_LEN, fin);

		// poate e define
		if (strncmp(buffer, DEFINE_TAG, strlen(DEFINE_TAG)) == 0) {
			err = define_process(buffer, fin, ht);
			if (err != SUCCESS)
				return err;

		} else
			fprintf(fout, "%s", buffer);

		// cat timp nu am ajuns la endif citesc
		// si ignor liniile
		while (fgets(buffer, MAX_LEN, fin))
			if (strstr(buffer, ENDIF_TAG) != NULL)
				break;
		flag_true_if = 1;
	}
	// conditia nu e adevarata
	else {
		// citesc
		// cat timp am linii cu elif verific conditia
		// daca e adevarata pun un flag si ignor ce urmeaza
		while (fgets(buffer, MAX_LEN, fin)) {
			if (strstr(buffer, ELIF_TAG) != NULL) {
				// inca nu am gasit o conditie adevarata pana
				// acum
				if (flag_true_if == 0) {
					erase_tag(buffer, processed_buf,
						  ELIF_TAG);

					// vad daca e in dictionar
					if (has_key(ht, processed_buf) == 1)
						strcpy(processed_buf,
						       get_value(
							   ht, processed_buf));

					// daca conditia e adevarata scriu linia
					// si setez flag-ul
					if (strcmp(processed_buf, "0") != 0) {
						fgets(buffer, MAX_LEN, fin);
						fprintf(fout, "%s", buffer);

						flag_true_if = 1;
					} else
						fgets(buffer, MAX_LEN, fin);
				} else
					fgets(buffer, MAX_LEN, fin);
			}

			if (strstr(buffer, ELSE_TAG) != NULL) {
				if (flag_true_if == 0) {
					// daca conditia e adevarata scriu linia
					// si setez flag-ul
					fgets(buffer, MAX_LEN, fin);
					fprintf(fout, "%s", buffer);

					flag_true_if = 1;

				} else
					fgets(buffer, MAX_LEN, fin);
			}

			if (strstr(buffer, ENDIF_TAG) != NULL)
				break;
		}
	}
	return SUCCESS;
}

int update_input_file(FILE *copy_fin, FILE *fin, char *input_file, char *buffer)
{
	fseek(copy_fin, 0, SEEK_SET);

	fclose(fopen(input_file, "w"));

	fseek(fin, 0, SEEK_SET);

	// copiez in input ce aveam in fisierul auxiliar
	while (fgets(buffer, MAX_LEN, copy_fin))
		fprintf(fin, "%s", buffer);

	fflush(fin);

	fclose(fopen(input_file, "r"));
	fseek(fin, 0, SEEK_SET);

	fclose(copy_fin);
	if (remove(COPY_INPUT_FILE) == 0)
		return SUCCESS;
	else
		return WRONG_CLOSED_FILE;

	return SUCCESS;
}

// Procesarea unui include
int include_process(char *path, int len_include_paths, char **include_paths,
		    char *input_file, FILE *fin, int *input_flag)
{
	int err, i, exist_include;
	char buffer[MAX_LEN];
	char *copy_buf, *token, *include_file, *include_path, *copy_path;
	FILE *copy_fin, *f_h;

	*input_flag = 0;

	// pana dau de include
	exist_include = 0;

	copy_fin = fopen(COPY_INPUT_FILE, "w+");
	if (copy_fin == NULL)
		return WRONG_OPEN_FILE;

	while (fgets(buffer, MAX_LEN, fin)) {
		if (strstr(buffer, INCLUDE_TAG) != NULL) {
			exist_include = 1;
			break;
		}
		fprintf(copy_fin, "%s", buffer);
	}

	if (exist_include == 1) {
		// sar peste include
		copy_buf = malloc((strlen(buffer) + 1) * sizeof(char));
		if (copy_buf == NULL) {
			fclose(copy_fin);
			return -ENOMEM;
		}
		strcpy(copy_buf, buffer);

		token = strtok(copy_buf + strlen(INCLUDE_TAG), "  \"\n");

		// salvez numele fisierului .h
		include_file = malloc(MAX_LEN * sizeof(char));

		if (include_file == NULL) {
			free(copy_buf);
			fclose(copy_fin);
			return -ENOMEM;
		}

		include_path = malloc(MAX_LEN * sizeof(char));

		if (include_path == NULL) {
			free(copy_buf);
			free(include_file);
			fclose(copy_fin);
			return -ENOMEM;
		}

		strcpy(include_file, token);

		// trebuie sa caut in directorul curent
		strcpy(include_path, path);
		strcat(include_path, "/");
		strcat(include_path, include_file);

		f_h = fopen(include_path, "r");

		// caut in argumentele de intrare
		if (f_h == NULL) {
			for (i = 0; i < len_include_paths; i++) {
				copy_path = malloc(MAX_LEN * sizeof(char));
				if (copy_path == NULL) {
					free(copy_buf);
					free(include_file);
					free(include_path);
					fclose(copy_fin);
					return -ENOMEM;
				}

				construct_include_path(
				    copy_path, include_paths[i], include_file);

				// deschid fisierul si il pastrez pe primul
				f_h = fopen(copy_path, "r");

				if (f_h != NULL) {
					free(copy_path);
					break;
				}
				free(copy_path);
			}

			if (f_h == NULL) {
				// elibereaza memorie
				free(copy_buf);
				free(include_file);
				free(include_path);
				fclose(copy_fin);
				return WRONG_FILE;
			}

			// l-am primit ca si parametru la intrare
			else {
				// appenduiesc tot fisierul
				err = append_all_header_file(f_h, copy_fin,
							     input_flag);
				if (err != SUCCESS) {
					free(copy_buf);
					free(include_file);
					free(include_path);
					fclose(copy_fin);
					fclose(f_h);
					return err;
				}
			}
		} else {
			err = append_all_header_file(f_h, copy_fin, input_flag);
			if (err != SUCCESS) {
				free(copy_buf);
				free(include_file);
				free(include_path);
				fclose(copy_fin);
				return err;
				;
			}
		}

		fclose(f_h);

		free(include_file);
		free(copy_buf);
		free(include_path);
	}

	while (fgets(buffer, MAX_LEN, fin))
		fprintf(copy_fin, "%s", buffer);

	err = update_input_file(copy_fin, fin, input_file, buffer);

	return err;
}

int process_inside_ifdef(char *buffer, FILE *fin, FILE *fout, HashTable *ht)
{
	int err;

	// incep procesarea
	if (strstr(buffer, IF_TAG) != NULL)
		if_process(buffer, fin, fout, ht);
	else {
		if (strstr(buffer, DEFINE_TAG) != NULL) {
			err = define_process(buffer, fin, ht);
			if (err != SUCCESS)
				return err;
		} else {
			if (strstr(buffer, UNDEF_TAG) != NULL) {
				undef_func(buffer, ht);
			} else {
				if (strcmp(buffer, "\n") != 0) {
					err = simply_write(buffer, fout, ht);
					if (err != SUCCESS)
						return err;
				}
			}
		}
	}

	return SUCCESS;
}

int call_recursive_ifdef_process(char *buffer, char *processed_buf,
				 HashTable *ht, FILE *fin, FILE *fout,
				 int last_cond_executed)
{
	int err;

	if (strstr(buffer, IFDEF_TAG) != NULL) {
		err = ifdef_process(buffer, processed_buf, IFDEF_TAG, ht, fin,
				    fout, last_cond_executed);
		if (err != SUCCESS)
			return err;
	} else {
		if (strstr(buffer, IFNDEF_TAG) != NULL) {
			err = ifdef_process(buffer, processed_buf, IFNDEF_TAG,
					    ht, fin, fout, last_cond_executed);
			if (err != SUCCESS)
				return err;
		} else
			process_inside_ifdef(buffer, fin, fout, ht);
	}

	return SUCCESS;
}

// Procesarea pentru ifdef si ifndef
// functia procesata se alege in functie de tag
int ifdef_process(char *buffer, char *processed_buf, char *tag, HashTable *ht,
		  FILE *fin, FILE *fout, int last_cond_executed)
{
	int write, defined = -1;
	int err;

	// sterg tag-ul
	erase_tag(buffer, processed_buf, tag);

	if (strcmp(tag, IFDEF_TAG) == 0)
		defined = 1;

	if (strcmp(tag, IFNDEF_TAG) == 0)
		defined = 0;

	// daca conditia e adevarata
	if (has_key(ht, processed_buf) == defined) {
		write = TRUE;
		last_cond_executed = TRUE;
	}

	else {
		write = FALSE;
		last_cond_executed = FALSE;
	}

	// cat timp nu am ajuns la endif procesez linii
	while (strstr(fgets(buffer, MAX_LEN, fin), ENDIF_TAG) == NULL) {
		// am ajuns la else
		if (strstr(buffer, ELSE_TAG) != NULL) {
			// ultimul ifdef / ifndef a fost adevarat
			if (last_cond_executed == TRUE)
				// de acum nu mai pot scrie pana la endif
				write = FALSE;
			else
				// pot scrie pana la endif
				write = TRUE;
		} else {
			if (write == TRUE) {
				err = call_recursive_ifdef_process(
				    buffer, processed_buf, ht, fin, fout,
				    last_cond_executed);
				if (err != SUCCESS)
					return err;
			}
		}
	}

	// am dat de endif ies din recursivitate
	if (strstr(buffer, ENDIF_TAG) != NULL)
		return SUCCESS;

	return SUCCESS;
}

// Proceseaza o linie care nu are simbolul la inceput #
int simply_write(char *buffer, FILE *fout, HashTable *ht)
{
	char *buff_out;
	char *copy_buf, *token, *frag1 = NULL, *frag2 = NULL, *frag3 = NULL;
	char *delim_string = "\"";
	char *delim_line = "\t []{}<>=+-*/%!&|^.,:;()\\";
	int cnt = 0;
	int err;

	buff_out = calloc(MAX_LEN, sizeof(char));
	if (buff_out == NULL)
		return -ENOMEM;

	copy_buf = malloc((strlen(buffer) + 1) * sizeof(char));
	if (copy_buf == NULL) {
		free(buff_out);
		return -ENOMEM;
	}
	strcpy(copy_buf, buffer);

	// daca e linie cu "
	if (strchr(copy_buf, '"')) {
		// sparg textul in 3 fragmente dupa " "
		token = strtok(copy_buf, delim_string);

		while (token != NULL) {
			cnt++;
			if (cnt == 1) {
				frag1 =
				    malloc((strlen(token) + 1) * sizeof(char));
				if (frag1 == NULL) {
					free(buff_out);
					free(copy_buf);
					return -ENOMEM;
				}
				strcpy(frag1, token);
			}
			if (cnt == 2) {
				frag2 =
				    malloc((strlen(token) + 1) * sizeof(char));
				if (frag2 == NULL) {
					free(buff_out);
					free(copy_buf);
					free(frag1);
					return -ENOMEM;
				}
				strcpy(frag2, token);
			}
			if (cnt == 3) {
				frag3 =
				    malloc((strlen(token) + 1) * sizeof(char));
				if (frag3 == NULL) {
					free(buff_out);
					free(copy_buf);
					free(frag1);
					free(frag2);
					return -ENOMEM;
				}
				strcpy(frag3, token);
			}
			token = strtok(NULL, delim_string);
		}

		err = process_fragment(buff_out, frag1, delim_line, ht);
		if (err != SUCCESS) {
			free(buff_out);
			free(copy_buf);
			free(frag1);
			free(frag2);
			free(frag3);
			return err;
		}

		if (frag2 != NULL) {
			strcat(buff_out, "\"");
			strcat(buff_out, frag2);
			strcat(buff_out, "\"");
		}

		err = process_fragment(buff_out, frag3, delim_line, ht);
		if (err != SUCCESS) {
			free(buff_out);
			free(copy_buf);
			free(frag1);
			free(frag2);
			free(frag3);
			return err;
		}

		free(frag1);
		free(frag2);
		free(frag3);
	}

	else {
		err = process_fragment(buff_out, buffer, delim_line, ht);
		if (err != SUCCESS) {
			free(buff_out);
			free(copy_buf);
			return err;
		}
	}

	// adaug linia in fisierul de iesire
	fprintf(fout, "%s", buff_out);

	free(copy_buf);
	free(buff_out);

	return SUCCESS;
}

void free_include_paths(char **include_paths, int len_include_paths)
{
	int i;

	for (i = 0; i < len_include_paths; i++)
		free(include_paths[i]);
	free(include_paths);
}

void free_all(char *output_file, char *input_file, char *path,
	      char **include_paths, int len_include_paths, FILE *fin,
	      FILE *fout, HashTable *ht)
{

	free(output_file);
	free(input_file);
	fclose(fin);
	fclose(fout);

	free_ht(ht);
	free_include_paths(include_paths, len_include_paths);
	free(path);
}

// Proceseaza daca am -D
int define_argument(HashTable *ht, char *argv[], int *i)
{
	char *define_input;
	char *word, *key, *value;
	int err;

	define_input = calloc(MAX_LEN, sizeof(char));
	if (define_input == NULL)
		return -ENOMEM;

	// e doar flag-ul
	if (strlen(argv[*i]) == 2) {
		// argumentul pt define este urmatorul element
		// sar peste el
		strcpy(define_input, argv[(*i) + 1]);
		(*i)++;
	} else {
		strcpy(define_input, argv[*i] + 2);
	}

	// daca are egal
	if (strchr(define_input, '=') != NULL) {
		word = strtok(define_input, "=");

		key = malloc((strlen(word) + 1) * sizeof(char));
		if (key == NULL) {
			free(define_input);
			return -ENOMEM;
		}
		strcpy(key, word);

		word = strtok(NULL, "=");

		value = malloc((strlen(word) + 1) * sizeof(char));
		if (value == NULL) {
			free(define_input);
			free(key);
			return -ENOMEM;
		}
		strcpy(value, word);

		err = put_value(ht, key, value);
		if (err != SUCCESS) {
			free(key);
			free(value);
			free(define_input);
			return err;
		}

		free(key);
		free(value);
	}
	// e string-ul nul ca value
	else {
		err = put_value(ht, define_input, "");
		if (err != SUCCESS) {
			free(define_input);
			return err;
		}
	}

	free(define_input);
	return SUCCESS;
}

// Proceseaza -I
void include_argument(char *argv[], char *include_input, int *i)
{
	// e doar flag-ul
	if (strlen(argv[*i]) == 2) {
		// argumentul pt define este urmatorul element
		// sar peste el
		strcpy(include_input, argv[(*i) + 1]);
		(*i)++;
	} else {
		strcpy(include_input, argv[*i] + 2);
	}
}

void output_argument(char *argv[], char *output_file, int *i)
{
	// e doar flag-ul
	if (strlen(argv[*i]) == 2) {
		// argumentul pt define este urmatorul element
		// sar peste el
		strcpy(output_file, argv[(*i) + 1]);
		(*i)++;
	} else
		strcpy(output_file, argv[*i] + 2);
}

int ifdef_ifndef_processing(char *buffer, HashTable *ht, FILE *fin, FILE *fout)
{
	int err;
	char processed_buf[MAX_LEN];

	if (strstr(buffer, IF_TAG) != NULL &&
	    strstr(buffer, IFNDEF_TAG) == NULL &&
	    strstr(buffer, IFDEF_TAG) == NULL)
		if_process(buffer, fin, fout, ht);
	else {
		// daca e ifdef se apeleaza
		// functia cu 1
		if (strstr(buffer, IFDEF_TAG) != NULL) {
			err = ifdef_process(buffer, processed_buf, IFDEF_TAG,
					    ht, fin, fout, FALSE);
			if (err != SUCCESS)
				return err;
		}

		else {
			// daca e ifndef se
			// apeleaza functia cu 0
			if (strstr(buffer, IFNDEF_TAG) != NULL) {
				err = ifdef_process(buffer, processed_buf,
						    IFNDEF_TAG, ht, fin, fout,
						    FALSE);
				if (err != SUCCESS)
					return err;
			} else {
				err = simply_write(buffer, fout, ht);
				if (err != SUCCESS)
					return err;
			}
		}
	}
	return SUCCESS;
}

int construct_output_file(char *buffer, FILE *fin, FILE *fout, HashTable *ht)
{
	int err;

	// citesc linie cu linie din fisier
	while (fgets(buffer, MAX_LEN, fin)) {
		if (strcmp(buffer, "\n") != 0) {
			// caut daca e define
			if (strstr(buffer, DEFINE_TAG) != NULL) {
				err = define_process(buffer, fin, ht);
				if (err != SUCCESS)
					return err;
			} else {
				if (strstr(buffer, UNDEF_TAG) != NULL) {
					// scot #undef
					undef_func(buffer, ht);
				} else {
					err = ifdef_ifndef_processing(
					    buffer, ht, fin, fout);
					if (err != SUCCESS)
						return err;
				}
			}
		}
	}

	return SUCCESS;
}

int call_include_process(char *path, char **include_paths,
			 int len_include_paths, char *input_file,
			 char *output_file, int flag_include, int flag_stdin,
			 FILE *fin, FILE *fout, HashTable *ht)
{
	int err;

	// cat timp mai am include-uri in fisier
	while (flag_include == 1 && flag_stdin == 0) {
		// procesez include-urile
		err = include_process(path, len_include_paths, include_paths,
				      input_file, fin, &flag_include);
		if (err != SUCCESS) {
			free_all(output_file, input_file, path, include_paths,
				 len_include_paths, fin, fout, ht);
			return err;
		}
		fseek(fin, 0, SEEK_SET);
	}

	return SUCCESS;
}

int open_input_file(char *input_file, char *output_file, int *flag_stdin,
		    HashTable *ht, char **include_paths, int len_include_paths,
		    char *path, FILE **fin)
{
	int err;

	if (input_file == NULL) {
		*fin = stdin;
		*flag_stdin = 1;
	} else {
		*fin = fopen(input_file, "r+");
		if (*fin == NULL) {
			free_ht(ht);
			free(input_file);
			free(output_file);
			free_include_paths(include_paths, len_include_paths);
			return WRONG_FILE;
		}

		err = construct_path(input_file, path);
		if (err != SUCCESS) {
			free_ht(ht);
			free(input_file);
			free(output_file);
			free_include_paths(include_paths, len_include_paths);
			return err;
		}
	}

	return SUCCESS;
}

int open_output_file(char *output_file, int *flag_stdout, char *input_file,
		     HashTable *ht, char **include_paths, int len_include_paths,
		     FILE **fin, FILE **fout)
{

	if (output_file == NULL) {
		*fout = stdout;
		*flag_stdout = 1;
	}

	if (*flag_stdout != 1) {
		*fout = fopen(output_file, "w");
		if (*fout == NULL) {
			free_ht(ht);
			free(input_file);
			free(output_file);
			free_include_paths(include_paths, len_include_paths);
			fclose(*fin);
			return WRONG_FILE;
		}
	}

	return SUCCESS;
}

int parse_arguments(char *argv[], HashTable *ht, int *i, char **include_input,
		    char ***include_paths, int *len_include_paths,
		    char **output_file)
{
	int err;

	if (argv[*i][1] == 'D') {
		err = define_argument(ht, argv, i);
		if (err != SUCCESS) {
			free_ht(ht);
			free(*include_paths);
			return err;
		}
	} else {
		if (argv[*i][1] == 'I') {
			(*include_input) = calloc(MAX_LEN, sizeof(char));
			if ((*include_input) == NULL) {
				free_ht(ht);
				return -ENOMEM;
			}
			include_argument(argv, (*include_input), i);

			(*include_paths)[*len_include_paths] =
			    malloc((strlen(*include_input) + 1) * sizeof(char));
			if ((*include_paths)[*len_include_paths] == NULL) {
				free_ht(ht);
				free(*include_input);
				return -ENOMEM;
			}
			strcpy((*include_paths)[*len_include_paths],
			       (*include_input));

			(*len_include_paths)++;
			free(*include_input);
		} else {
			if (argv[*i][1] == 'o') {
				(*output_file) = calloc(MAX_LEN, sizeof(char));
				if ((*output_file) == NULL) {
					free_ht(ht);
					return -ENOMEM;
				}
				output_argument(argv, *output_file, i);
			} else
				return WRONG_COMMAND;
		}
	}

	return SUCCESS;
}
