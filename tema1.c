#include "tema1.h"

char* construct_out_file(char* input_file, char* output_file) {
    // parsez denumirea fisierului de intrare
    replace(input_file, INPUTS, OUTPUTS, output_file);
    char copy_out[strlen(output_file) + 1];
    strcpy(copy_out, output_file);
    replace(copy_out, INPUT_EXTENSION, OUTPUT_EXTENSION, output_file);

    return output_file;
}

// https://stackoverflow.com/questions/15465755/replacing-a-word-in-a-text-by-another-given-word
void replace(char* line, char* to_replace, char* replacement, char* new_line) {
    char *pos = strstr(line, to_replace);

    // if found
    if (pos != NULL) {
        // Copy the part of the old sentence *before* the replacement
        memcpy(new_line, line, pos - line);

        // Copy the replacement
        memcpy(new_line + (pos - line), replacement, strlen(replacement));

        // Copy the rest
        strcpy(new_line + (pos - line) + strlen(replacement), pos + strlen(to_replace));

    }

    return;
}

int process_fragment(char* buff_out, char* frag, char* delim_line, HashTable* ht) {

    char* copy_frag;
    char* word;
    char* new_frag;
    // delimitez pe cuvinte
    copy_frag = calloc(MAX_LEN, sizeof(char));
    if (copy_frag == NULL) {
        return ENOMEM;
    }
    strcpy(copy_frag, frag);

    word = strtok(frag, delim_line);
    new_frag = calloc(MAX_LEN, sizeof(char));
    if (new_frag == NULL) {
        return ENOMEM;
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

// scot "" din text
void literal_to_value(char* text_value, char* processed_text) {
    
    strcpy(processed_text, text_value);
    replace(text_value, "\"", "", processed_text);
    strcpy(text_value, processed_text);
    replace(text_value, "\"", "", processed_text);
}

// proceseaza text_value si il returneaza schimbat in
// processed_text, fara tab-uri \ sau \n;
void process_multi_line_def(char* text_value, char* processed_text) {
    
    // scapam de tab, \ si \n
    strcpy(processed_text, text_value);
    replace(text_value, "        ", "", processed_text);
    strcpy(text_value, processed_text);
    replace(text_value, "\\", "", processed_text);
    strcpy(text_value, processed_text);
    replace(text_value, "\n", "", processed_text);
    strcat(processed_text, " ");
}

int construct_path(char* input_file, char* path) {
    // sterg test.in
    char* new_path;
    char* token;
    char copy_input[strlen(input_file) + 1];

    strcpy(copy_input, input_file);

    new_path = calloc (MAX_LEN, sizeof(char));
    if (new_path == NULL) {
        return ENOMEM;
    }
    token = strtok(copy_input, "/");

    strcpy(new_path, "/");
    while (token != NULL) {
        strcpy(new_path + 1, token);
        token = strtok(NULL, "/");
    }

    replace(input_file, new_path, "", path);

    free(new_path);

    return SUCCESS;
}

void construct_include_path(char* include_path, char* path, char* include_file) {
    // trebuie sa caut in directorul curent
    strcpy(include_path, path);
    strcat(include_path, "/");
    strcat(include_path, include_file);
}

void error_ret(char* output_file, char* input_file, FILE* fin, FILE* fout,
                HashTable *ht, char** include_paths, int len_include_paths, char* path) {
 
    free(output_file);
    free(input_file);
    fclose(fin);
    fclose(fout);

  
    free_ht(ht);
    for (int i = 0; i < len_include_paths; i++) {
        free(include_paths[i]);
    }
    free(include_paths);
    free(path);
}

int append_all_header_file(FILE* f_h, FILE* fout) {
    char* include_buff;
    char buff_header[MAX_LEN];

    include_buff = calloc(MAX_LEN, sizeof(char));
    if (include_buff == NULL) {
        return ENOMEM;
    }
    
    while (fgets(buff_header, MAX_LEN, f_h)) {
        strcat(include_buff, buff_header);
    }
    fprintf(fout, "%s", include_buff);

    free(include_buff);

    return SUCCESS;
}

int append_all_header_file_1(FILE* f_h, FILE* fout, int* flag_input) {
    char* include_buff;
    char buff_header[MAX_LEN];

    include_buff = calloc(MAX_LEN, sizeof(char));
    if (include_buff == NULL) {
        return ENOMEM;
    }

    while (fgets(buff_header, MAX_LEN, f_h)) {
        // daca mai am un include
        if (strstr(buff_header, INCLUDE_TAG) != NULL) {
            *flag_input = 1;
        }
        strcat(include_buff, buff_header);
    }
    fprintf(fout, "%s", include_buff);


    free(include_buff);
    return SUCCESS;
}


void append_all_header_file_in_buff(FILE* f_h, char* include_buff, int* flag_include) {
    char buff_header[MAX_LEN];
    int flag = 0;

    while (fgets(buff_header, MAX_LEN, f_h)) {
        strcat(include_buff, buff_header);
        // daca mai am un #include retin
        if (strstr(buff_header, INCLUDE_TAG) != NULL) {
            flag = 1;
        }
    }

    // nu mai am niciun include
    if (flag == 0) {
        *flag_include = 0;
    }

}


// elimina variabila de dupa undef din dictionar
void undef_func(char* buffer, HashTable* ht) {
    char parsed_buff[MAX_LEN];
    replace(buffer, UNDEF_TAG, "", parsed_buff);
    replace(parsed_buff, " ", "", buffer);
    replace(buffer, "\n", "", parsed_buff);
    // sterg cheia din dictionar
    if (has_key(ht, parsed_buff) == 1) {
        remove_entry(ht, parsed_buff);
    }
}

void erase_tag(char* buffer, char* processed_buf, char* tag) {
    replace(buffer, tag, "", processed_buf);
    replace(processed_buf, " ", "", buffer); 
    replace(buffer, "\n", "", processed_buf);
}


int define_process(char* buffer, FILE* fin, HashTable* ht) {
    int err;
    char* copy_buf = strdup(buffer);
    if (copy_buf == NULL) {
        return ENOMEM;
    }
    char *key, *value, *text_value, *processed_text;
    processed_text = calloc (MAX_LEN, sizeof(char));
    if (processed_text == NULL) {
        return ENOMEM;
    }
    text_value = calloc (MAX_LEN, sizeof(char));
    if (text_value == NULL) {
        return ENOMEM;
    }
    value = calloc (MAX_LEN, sizeof(char));
    if (value == NULL) {
        return ENOMEM;
    }
    key = calloc (MAX_LEN, sizeof(char));
    if (key == NULL) {
        return ENOMEM;
    }

    // sar peste #define
    char* delim_define = " \n";
    char* token = strtok(copy_buf + strlen(DEFINE_TAG), delim_define);
    if (token != NULL) {
        strcpy(key, token);
    }

    char* delim_val = "\n";
    token = strtok(NULL, delim_val);

    if (token != NULL) {
        strcpy(text_value, token);
    }
    else {
        strcpy(text_value, "");
    }

    // daca am text in text_value
    if (strlen(text_value) >= 1) {
        // define imbricat
        if (text_value[strlen(text_value) - 1] != '\\') {
            // trebuie sa vad daca am un #define in alt #define
            err = process_fragment(value, text_value, delim_define, ht);
            if (err != SUCCESS) {
                return err;
            }
        }

        // define multilinie
        else {
            // scapam de tab, \ si \n
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
                    process_multi_line_def(text_value, processed_text);
                    strcat(value, processed_text);
                    // scot " " de la final
                    value[strlen(value) - 1] = '\0';

                    fgets(buffer, MAX_LEN, fin);
                    break;
                }
                else {
                    strcpy(text_value, buffer);
                    process_multi_line_def(text_value, processed_text);
                    strcat(value, processed_text);
                }
            }
        }
    }

    // adaug (key, value) in hashtable
    err = put_value(ht, key, value);
    if (err != SUCCESS) {
        return err;
    }

    free(processed_text);
    free(copy_buf);
    free(key);
    free(value);
    free(text_value);

    return SUCCESS;
}

// verifica daca linia contine
// define sau undef sau nu contine nimic
// si trebuie doar sa o scrie in fout
int verify_single_line(char* buffer, FILE* fin, FILE* fout, HashTable* ht) {
    int err;
     // daca am define in ea
    if (strstr(buffer, DEFINE_TAG) != NULL) {
        err = define_process(buffer, fin, ht);
        if (err != SUCCESS) {
            return err;
        }
    }
    else {
        // am undef
        if (strstr(buffer, UNDEF_TAG) != NULL) {
            undef_func(buffer, ht);
        }
    
        // doar trebuie sa o scriu
        else {
            fprintf(fout,"%s", buffer);
            // printf("%s", buffer);
        }
    }

    return SUCCESS;
}


// procesarea pentru ifdef si ifndef
int def_process(char* buffer, char* processed_buf, char* tag,
                HashTable* ht, FILE* fin, FILE* fout, int defined) {
    
    int err;
    erase_tag(buffer, processed_buf, tag);

    // daca a fost definit/nu a fost definit inca
    if (has_key(ht, processed_buf) == defined) {
        // procesez urmatoarea linie (atat imi trebuie)
        fgets(buffer, MAX_LEN, fin);
        err = verify_single_line(buffer, fin, fout, ht);
        if (err != SUCCESS) {
            return err;
        }                       

        // ignor restul liniilor pana la endif
        while (fgets(buffer, MAX_LEN, fin)) {
            if (strstr(buffer, ENDIF_TAG) != NULL) {
                break;
            }
        }
    }

    else {
        // ignor urmatoarea linie
        fgets(buffer, MAX_LEN, fin);
        fgets(buffer, MAX_LEN, fin);

        // daca e else trebuie sa execut urmatoarea linie 
        // dupa else
        if (strstr(buffer, ELSE_TAG) != NULL) {
            fgets(buffer, MAX_LEN, fin);
            // executa linia
            err = verify_single_line(buffer, fin, fout, ht);
            if (err != SUCCESS) {
                return err;
            }
            // citeste endif si ignora
            fgets(buffer, MAX_LEN, fin);
        }
    }

    return SUCCESS;
}

// operatia de include
int include_process(char* buffer, char* path, int len_include_paths,
                    char** include_paths, char* input_file, char* output_file,
                    FILE* fin, FILE* fout, HashTable* ht) {
    
    char* include_file;
    char* include_path;
    int i;
    int err;
    // sar peste include
    char* copy_buf = strdup(buffer);
    if (copy_buf == NULL) {
        return ENOMEM;
    }
    char* token = strtok(copy_buf + strlen(INCLUDE_TAG), "  \"\n");

    // salvez numele fisierului .h
    include_file = malloc(MAX_LEN * sizeof(char));
    if (include_file == NULL) {
        return ENOMEM;
    }
    include_path = malloc(MAX_LEN * sizeof(char));
    if (include_path == NULL) {
        return ENOMEM;
    }

    strcpy(include_file, token);

    // trebuie sa caut in directorul curent
    strcpy(include_path, path);
    strcat(include_path, "/");
    strcat(include_path, include_file);

    FILE *f_h;
    f_h = fopen(include_path, "r");

    // caut in argumentele de intrare
    if (f_h == NULL) {
        for (i = 0; i < len_include_paths; i++) {
            char* copy_path = malloc(MAX_LEN * sizeof(char));
            if (copy_path == NULL) {
                return ENOMEM;
            }

            construct_include_path(copy_path, include_paths[i], include_file);
            
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
            free(include_file);
            free(copy_buf);
            free(include_path);
            error_ret(output_file, input_file, fin, fout, ht, include_paths, len_include_paths, path);
            return WRONG_FILE;
        }

        // l-am primit ca si parametru la intrare
        else {
            // appenduiesc tot fisierul
            err = append_all_header_file(f_h, fout);
            if (err != SUCCESS) {
                return err;
            }

        }
    } else {
            err = append_all_header_file(f_h, fout);
            if (err != SUCCESS) {
                return err;
            }
    }

    fclose (f_h);

    free(include_file);
    free(copy_buf);
    free(include_path);
    return SUCCESS;
}

int if_process(char* buffer, FILE* fin, FILE* fout, HashTable* ht) {
    int err;
    int flag_true_if;
    char processed_buf[MAX_LEN];

    erase_tag(buffer, processed_buf, IF_TAG);

    // vad daca e in dictionar
    if (has_key(ht, processed_buf) == 1) {
        strcpy(processed_buf, get_value(ht, processed_buf));
    }

    flag_true_if = 0;
    // daca conditia e adevarata
    if (strcmp(processed_buf, "0") != 0) {
        // citesc urmatoarea linie si o scriu
        // nemodificata
        fgets(buffer, MAX_LEN, fin);

        // poate e define
        if (strncmp(buffer, DEFINE_TAG, strlen(DEFINE_TAG)) == 0) {
            err = define_process(buffer, fin, ht);
            if (err != SUCCESS) {
                return err;
            }
        }
        else {
            fprintf(fout,"%s", buffer);
        }

        // printf("%s", buffer);

        // cat timp nu am dat de endif citesc
        // si ignor
        while (fgets(buffer, MAX_LEN, fin)) {
            if (strstr(buffer, ENDIF_TAG) != NULL) {
                break;
            }
        }
        flag_true_if = 1;
    }
    // conditia nu e adevarata
    else {
        // citesc
        // cat timp am linii cu elif verific conditia
        // daca e adevarata pun un flag si ignor ce urmeaza
        while (fgets(buffer, MAX_LEN, fin)) {
            if (strstr(buffer, ELIF_TAG) != NULL) {
                // inca nu am gasit o conditie adevarata
                if (flag_true_if == 0) {
                    erase_tag(buffer, processed_buf, ELIF_TAG);

                    // vad daca e in dictionar
                    if (has_key(ht, processed_buf) == 1) {
                        strcpy(processed_buf, get_value(ht, processed_buf));
                    }
                    
                    // daca conditia e adevarata scriu linia
                    // si setez flag-ul
                    if (strcmp(processed_buf, "0") != 0) {
                        fgets(buffer, MAX_LEN, fin);
                        fprintf(fout,"%s", buffer);
                        // printf("%s", buffer);

                        flag_true_if = 1;
                    }
                    else {
                        fgets(buffer, MAX_LEN, fin);
                    }
                }
                else {
                    fgets(buffer, MAX_LEN, fin);
                }
            }

            if (strstr(buffer, ELSE_TAG) != NULL) {
                if (flag_true_if == 0) {
                    // daca conditia e adevarata scriu linia
                    // si setez flag-ul
                    fgets(buffer, MAX_LEN, fin);
                    fprintf(fout,"%s", buffer);
                    // printf("%s", buffer);

                    flag_true_if = 1;
                    
                }
                else {
                    fgets(buffer, MAX_LEN, fin);
                }
            }

            if (strstr(buffer, ENDIF_TAG) != NULL) {
                break;
            }
        }
    }

    return SUCCESS;
}

int replace_include_1(char* path, int len_include_paths,
                    char** include_paths, char* input_file, char* output_file,
                    FILE* fin, FILE* fout, HashTable* ht, int* input_flag) {

    int err;
    FILE *copy_fin = fopen(COPY_INPUT_FILE, "w+");
    char buffer[MAX_LEN];

    *input_flag = 0;
    int i;

    // pana dau de include
    int exist_include = 0;
    while (fgets(buffer, MAX_LEN, fin)) {
        if (strstr(buffer, INCLUDE_TAG) != NULL) {
            exist_include = 1;
            break;
        }
        fprintf(copy_fin, "%s", buffer);
    }

    if (exist_include == 1) {
        // sar peste include
        char* copy_buf = strdup(buffer);
        if (copy_buf == NULL) {
            return ENOMEM;
        }
        char* token = strtok(copy_buf + strlen(INCLUDE_TAG), "  \"\n");

        // salvez numele fisierului .h
        char* include_file = malloc(MAX_LEN * sizeof(char));
        
        if (include_file == NULL) {
            return ENOMEM;
        }

        char* include_path = malloc(MAX_LEN * sizeof(char));

        if (include_path == NULL) {
            return ENOMEM;
        }

        strcpy(include_file, token);

        // trebuie sa caut in directorul curent
        strcpy(include_path, path);
        strcat(include_path, "/");
        strcat(include_path, include_file);

        FILE *f_h;
        f_h = fopen(include_path, "r");

        // caut in argumentele de intrare
        if (f_h == NULL) {
            for (i = 0; i < len_include_paths; i++) {
                char* copy_path = malloc(MAX_LEN * sizeof(char));
                if (copy_path == NULL) {
                    return ENOMEM;
                }

                construct_include_path(copy_path, include_paths[i], include_file);
                
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
                free(include_file);
                free(copy_buf);
                free(include_path);
                error_ret(output_file, input_file, fin, fout, ht, include_paths, len_include_paths, path);
                return WRONG_FILE;
            }

            // l-am primit ca si parametru la intrare
            else {
                // appenduiesc tot fisierul
                err = append_all_header_file_1(f_h, copy_fin, input_flag);
                if (err != SUCCESS) {
                    return err;
                }

            }
        } else {
                err = append_all_header_file_1(f_h, copy_fin, input_flag);
                if (err != SUCCESS) {
                    return err;;
                }
        }

        fclose (f_h);

        free(include_file);
        free(copy_buf);
        free(include_path);
    }

    while (fgets(buffer, MAX_LEN, fin)) {
        fprintf(copy_fin, "%s", buffer);
    }

    fseek(fin, 0, SEEK_SET);
    fseek(copy_fin, 0, SEEK_SET);

    fclose(fopen(input_file, "w"));

    fseek(fin, 0, SEEK_SET);
    
    // copiez in input ce aveam in fisierul auxiliar
    while (fgets(buffer, MAX_LEN, copy_fin)) {
        fprintf(fin, "%s", buffer);
        // printf("%s", buffer);
    }

    fflush(fin);

    fclose(fopen(input_file, "r"));
    fseek(fin, 0, SEEK_SET);


    fclose(copy_fin);
    if (remove(COPY_INPUT_FILE) == 0) {
        return SUCCESS;
    } else {
        return WRONG_CLOSED_FILE;
    }
   
    return SUCCESS;
}



// la apelarea din main, last_cond e implicit falsa
// last_cond inseamna daca instructiunea trebuie sa se execute sau nu
int ifdef_process(char* buffer, char* processed_buf, char* tag,
                HashTable* ht, FILE* fin, FILE* fout, int last_cond_executed) {
    
    int write, defined;
    int err;
    // sterg tag-ul
    erase_tag(buffer, processed_buf, tag);

    if (strcmp(tag, IFDEF_TAG) == 0) {
        defined = 1;
    }
    if (strcmp(tag, IFNDEF_TAG) == 0) {
        defined = 0;
    }

    // daca conditia e adevarata
    if (has_key(ht, processed_buf) == defined) {
        write = TRUE;
        last_cond_executed = TRUE;
    }

    else {
        write = FALSE;
        last_cond_executed = FALSE;
    }

    // last_cond = write;

    // cat timp nu am ajuns la endif
    while (strstr(fgets(buffer, MAX_LEN, fin), ENDIF_TAG) == NULL) {
        // am dat de else
        if (strstr(buffer, ELSE_TAG) != NULL) {
            // ultimul ifdef / ifndef a fost adevarat
            if (last_cond_executed == TRUE) {
                // de acum nu mai pot scrie pana la endif
                write = FALSE;
            }
            else {
                // pot scrie pana la endif
                write = TRUE;
            }
        }
        else {
            if (write == TRUE) {
                if (strstr(buffer, IFDEF_TAG) != NULL) {
                    err = ifdef_process(buffer, processed_buf, IFDEF_TAG, ht, fin, fout, last_cond_executed);
                    if (err != SUCCESS) {
                        return err;
                    }
                }
                else {
                    if (strstr(buffer, IFNDEF_TAG) != NULL) {
                        err = ifdef_process(buffer, processed_buf, IFNDEF_TAG, ht, fin, fout, last_cond_executed);
                        if (err != SUCCESS) {
                            return err;
                        }
                    }
                    else {
                        // incep procesarea
                        if (strstr(buffer, IF_TAG) != NULL) {
                            if_process(buffer, fin, fout, ht);
                        }
                        else {
                            if (strstr(buffer, DEFINE_TAG) != NULL) {
                                err = define_process(buffer, fin, ht);
                                if (err != SUCCESS) {
                                    return err;
                                }
                            }
                            else {
                                if (strstr(buffer, UNDEF_TAG) != NULL) {
                                    undef_func(buffer, ht);
                                }
                                else {
                                    if (strcmp(buffer, "\n") != 0) {
                                        err = simply_write(buffer, fout, ht);
                                        if (err != SUCCESS) {
                                            return err;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // am dat de endif ies din recursivitate
    if (strstr(buffer, ENDIF_TAG) != NULL) {
        return SUCCESS;
    }

    return SUCCESS;
}


int simply_write(char* buffer, FILE* fout, HashTable* ht) {
    char* buff_out;
    char* copy_buf;
    char* delim_string = "\"";
    char* delim_line = "\t []{}<>=+-*/%!&|^.,:;()\\";
    int cnt = 0;
    int err;

    buff_out = calloc (MAX_LEN, sizeof(char));
    if (buff_out == NULL) {
        return ENOMEM;
    }

    copy_buf = strdup(buffer);
    if (copy_buf == NULL) {
        return ENOMEM;
    }

    // daca e linie cu "
    if (strchr(copy_buf, '"')) {
        // sparg textul in 3 fragmente dupa " "
        char* token = strtok(copy_buf, delim_string);
        char* frag1;
        char* frag2;
        char* frag3;

        while (token != NULL) {
            cnt++;
            if (cnt == 1) {
                frag1 = strdup(token);
                if (frag1 == NULL) {
                    return ENOMEM;
                }
            }
            if (cnt == 2) {
                frag2 = strdup(token);
                if (frag2 == NULL) {
                    return ENOMEM;
                }
            }
            if (cnt == 3) {
                frag3 = strdup(token);
                if (frag3 == NULL) {
                    return ENOMEM;
                }
            }
            token = strtok(NULL, delim_string);
        }

        err = process_fragment(buff_out, frag1, delim_line, ht);
        if (err != SUCCESS) {
            return err;
        }

        if (frag2 != NULL) {
            strcat(buff_out, "\"");
            strcat(buff_out, frag2);
            strcat(buff_out, "\"");
        }

        err = process_fragment(buff_out, frag3, delim_line, ht);
        if (err != SUCCESS) {
            return err;
        }

        free(frag1);
        free(frag2);
        free(frag3);
    }

    else {
        err = process_fragment(buff_out, buffer, delim_line, ht);
        if (err != SUCCESS) {
            return err;
        }
    }

    // adaug linia in fisierul de iesire
    fprintf(fout,"%s", buff_out);

    // printf("%s", buff_out);

    free(copy_buf);
    free(buff_out);

    return SUCCESS;
}



int main(int argc, char *argv[]) 
{
    int i, ret, err, flag_stdin = 0;
    int flag_stdout = 0;
    char* input_file = NULL;
    char* output_file = NULL;
    char** include_paths = NULL;
    const char point = '.';
    char processed_buf[MAX_LEN];
    int len_include_paths = 0;
    int flag_include = 1;

    HashTable* ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return ENOMEM;
    }
    err = init_ht(ht, MAX_BUCKETS);
    if (err != SUCCESS) {
        free(ht);
        return err;
    }
    
    include_paths = malloc(MAX_INCLUDES * sizeof(char *));
    if (include_paths == NULL) {
        free(ht);
        return ENOMEM;
    }

    // parsare argumente
    for (i = 1; i < argc; i++) {

        // daca ar trebui sa fie un flag
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'D') {
                char* define_input;
                define_input = calloc(MAX_LEN, sizeof(char));
                if (define_input == NULL) {
                    return ENOMEM;
                }

                // e doar flag-ul
                if (strlen(argv[i]) == 2) {
                    // argumentul pt define este urmatorul element
                    // sar peste el
                    strcpy(define_input, argv[i + 1]);
                    i++;
                }
                else {
                    strcpy(define_input, argv[i] + 2);
                }

                // daca are egal
                if (strchr(define_input, '=') != NULL) {
                    char* word = strtok(define_input, "=");
                    char* key = strdup(word);
                    if (key == NULL) {
                        return ENOMEM;
                    }
                    word = strtok(NULL, "=");
                    char* value = strdup(word);
                    if (value == NULL) {
                        return ENOMEM;
                    }

                    err = put_value(ht, key, value);
                    if (err != SUCCESS) {
                        return err;
                    }
                
                    free(key);
                    free(value);
                }
                // e string-ul nul ca value
                else {
                    err = put_value(ht, define_input, "");
                    if (err != SUCCESS) {
                        return err;
                    }
                }

                free(define_input);

            }
            else {
                if (argv[i][1] == 'I') {
                    char* include_input;
                    include_input = calloc(MAX_LEN, sizeof(char));
                    if (include_input == NULL) {
                        return ENOMEM;
                    }

                    // e doar flag-ul
                    if (strlen(argv[i]) == 2) {
                        // argumentul pt define este urmatorul element
                        // sar peste el
                        strcpy(include_input, argv[i + 1]);
                        i++;
                    }
                    else {
                        strcpy(include_input, argv[i] + 2);
                    }

                    include_paths[len_include_paths] = strdup(include_input);
                    if (include_paths[len_include_paths] == NULL) {
                        return ENOMEM;
                    }

                    len_include_paths++;
                    free(include_input);
                }
                else {
                    if (argv[i][1] == 'o') {
                        output_file = calloc(MAX_LEN, sizeof(char));
                        if (output_file == NULL) {
                            return ENOMEM;
                        }

                        // e doar flag-ul
                        if (strlen(argv[i]) == 2) {
                            // argumentul pt define este urmatorul element
                            // sar peste el
                            strcpy(output_file, argv[i + 1]);
                            i++;
                        }
                        else {
                            strcpy(output_file, argv[i] + 2);
                        }
                    }
                    else {
                        return WRONG_COMMAND;
                    }
                }
            }
        }
        else {
            // poate fi fisier .out sau fisier .in
            char* file = strdup(argv[i]);
            if (file == NULL) {
                return ENOMEM;
            }

            // verific daca este INFILE-ul (ar trebui sa aiba .in)
            if (strcmp(strchr(file, point), INPUT_EXTENSION) == 0) {
                input_file = strdup(file);
                if (input_file == NULL) {
                    return ENOMEM;
                }
            }
            else {
                // e outfile-ul
                if (strcmp(strchr(file, point), OUTPUT_EXTENSION) == 0) {
                    output_file = strdup(file);
                    if (output_file == NULL) {
                        return ENOMEM;
                    }
                }
                else {
                    return WRONG_COMMAND;
                }
            }

            free(file);
        }
    }

    if (output_file == NULL && input_file != NULL) {
        output_file = calloc (strlen(input_file) + 3, sizeof(char));
        if (output_file == NULL) {
            return ENOMEM;
        }
        output_file = construct_out_file(input_file, output_file);
        flag_stdout = 0;

    }


    FILE *fin, *fout;
    char* path = malloc (MAX_LEN * sizeof(char));
    if (path == NULL) {
        return ENOMEM;
    }


    // for (int i = 0; i < len_include_paths; i++) {
    //     printf("%s ", include_paths[i]);
    // }

    if (input_file == NULL) {
        fin = stdin;
        flag_stdin = 1;
        if (output_file == NULL) {
            fout = stdout;
            flag_stdout = 1;
        }
    }
    else {
        fin = fopen(input_file, "r+");
        if (fin == NULL) {
            free_ht(ht);
            return WRONG_FILE;             
        }
        if (path == NULL) {
            return ENOMEM;
        }
        err = construct_path(input_file, path);
        if (err != SUCCESS) {
            return err;
        }
    }

    if (flag_stdout != 1) {
        fout = fopen(output_file, "w");
        if (fout == NULL) {
            free_ht(ht);
            return WRONG_FILE;
        }
    }

    // cat timp mai am include-uri in fisier
    while (flag_include == 1 && flag_stdin == 0) {
        // procesez include-urile
        // err = replace_include(input_file, &flag_include, path, len_include_paths, include_paths, output_file,
        //                 fin, fout, ht);
        err = replace_include_1(path, len_include_paths, include_paths,
                                input_file, output_file, fin, fout, ht, &flag_include);
        // printf("%d\n", flag_include);
        if (err != SUCCESS) {
            return err; 
        }

        fseek(fin, 0, SEEK_SET);
    }



    // citesc linie cu linie din fisier
    char buffer[MAX_LEN];

    while (fgets(buffer, MAX_LEN, fin)) {
        if (strcmp(buffer, "\n") != 0) {
            // caut daca e define
            if (strstr(buffer, DEFINE_TAG) != NULL) {
                err = define_process(buffer, fin, ht);
                if (err != SUCCESS) {
                    return err;
                }
            }

            else {
                // e include
                if (strstr(buffer, INCLUDE_TAG) != NULL) {
                    ret = include_process(buffer, path, len_include_paths, include_paths,
                    input_file, output_file, fin, fout, ht);
                    if (ret != SUCCESS) {
                        return ret; 
                    }

                } else {
                    if (strstr(buffer, UNDEF_TAG) != NULL) {
                        // scot #undef 
                        undef_func(buffer, ht);
                    }
                    else {
                        if (strstr(buffer, IF_TAG) != NULL && strstr(buffer, IFNDEF_TAG) == NULL &&
                            strstr(buffer, IFDEF_TAG) == NULL) {
                                if_process(buffer, fin, fout, ht);
                        } else {
                            // daca e ifdef se apeleaza functia cu 1
                            if (strstr(buffer, IFDEF_TAG) != NULL) {
                                // def_process(buffer, processed_buf, IFDEF_TAG, ht, fin, fout, 1);
                                err = ifdef_process(buffer, processed_buf, IFDEF_TAG, ht, fin, fout, FALSE);
                                if (err != SUCCESS) {
                                    return err;
                                }
                            }

                            else {
                                // daca e ifndef se apeleaza functia cu 0
                                if (strstr(buffer, IFNDEF_TAG) != NULL) {
                                    // def_process(buffer, processed_buf, IFNDEF_TAG, ht, fin, fout, 0);
                                    err = ifdef_process(buffer, processed_buf, IFNDEF_TAG, ht, fin, fout, FALSE);
                                    if (err != SUCCESS) {
                                        return err;
                                    }
                                }
                                else {
                                    err = simply_write(buffer, fout, ht);
                                    if (err != SUCCESS) {
                                        return err;
                                    } 
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // print_ht(ht);
    
    free(output_file);
    free(input_file);
    fclose(fin);
    fclose(fout);

  
    free_ht(ht);
    for (int i = 0; i < len_include_paths; i++) {
        free(include_paths[i]);
    }
    free(include_paths);
    free(path);
    return 0;
}

