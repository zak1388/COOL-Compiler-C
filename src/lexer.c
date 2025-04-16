#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

enum TokenType TokenTypeOfString(int size, char tokenString[size]) {
    if (size == 0) {
        return ERROR;
    }

    /* Punctuation */
    for (int i = 0; i < TOKEN_COUNT; i++) {
        if (strcasecmp(tokenString, TokenTypeMap[i]) == 0) {
            return i;
        }
    }

    /* Literals */
    bool isInt = true;
    bool isString = tokenString[0] == '"' && tokenString[size - 1] == '"';
    bool isBool = islower(tokenString[0]) && (strcasecmp(tokenString, "true") == 0 || strcasecmp(tokenString, "false") == 0);
    bool isObjId = islower(tokenString[0]) && isalpha(tokenString[0]);
    bool isTypeId = isupper(tokenString[0]) && isalpha(tokenString[0]);
    for (int i = 0; i < size; i++) {
        isInt = isInt && isdigit(tokenString[i]);

        if (i + 1 < size) {
            isString &= !(tokenString[i] == '\\' && tokenString[i + 1] == '0');
        }

        

        isObjId &= isalnum(tokenString[i]) || tokenString[i] == '_';
        isTypeId &= isalnum(tokenString[i]) || tokenString[i] == '_';
    }

    if (isInt) return INTEGER;
    else if (isString) return STRING;
    else if (isBool) {
        for (int i = 0; i < size; i++) tokenString[i] = tolower(tokenString[i]);
        return BOOL_CONST;
    }
    else if (isObjId ) return OBJECTID;
    else if (isTypeId) return TYPEID;
    else return ERROR;
}

int coolStringLength(struct Token* t) {
    int length = 0;
    bool escaped = false;

    if (t->type != STRING) return -1;

    int cStrLength = strlen(t->data);
    for (int i = 0; i < cStrLength; i++) {
        if (escaped) {
            escaped = false;
            continue;
        } else if (t->data[i] == '\\') {
            escaped = true;
        }
        length++;
    }

    return length - 2; // ignore quotes
}

void consume_block_comment(FILE* f) {
    int comment_level = 0;
    char possible_end[3] = {0};
    do {
        if (fread(possible_end, 1, 2, f) < 2) {
            break;
        }

        if (possible_end[0] == '(' && possible_end[1] == '*') {
            comment_level++;
        } else if (possible_end[0] == '*' && possible_end[1] == ')') {
            comment_level--;
        } else {
            // try to read 2 chars, if we didnt find the end it might have been partially consumed
            fseek(f, -1, SEEK_CUR);
        }
    } while (comment_level > 0);
}

struct Token* lex_file(char* filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }

    struct Token* first = NULL;
    struct Token* last = NULL;
    bool reached_eof = false;
    int line = 1;
    while (!reached_eof) {
        char* tokenString = malloc(BUF_SIZE);
        int size = 0;
        bool in_string = false;
        while (size < BUF_SIZE - 2 && fread(tokenString + size, 1, 1, f) > 0) {
            size++;
            if (*(tokenString + size - 1) == '"') {
                in_string ^= true;
            } else if ((tokenString[0] == '(' || tokenString[0] == '<' || tokenString[0] == '-') 
                    && (size < BUF_SIZE - 3 && fread(tokenString + size, 1, 1, f) > 0)) {  
                // 1 char look ahead
                size++;
                tokenString[size] = '\0';

                // printf("tokenString - 2 in lookahead: \"%s\"\n", tokenString + size - 2);
                // printf("\tstrcmp for \"*)\"? %s\n", strcmp(tokenString + size - 2, "*)") == 0? "true" : "false");

                if (strcmp(tokenString + size - 2, "(*") == 0) {
                    size -= 2;
                    fseek(f, -2, SEEK_CUR);
                    consume_block_comment(f);
                }  else if (strcmp(tokenString + size - 2, "<-") == 0) {
                    break;
                } else if (strcmp(tokenString + size - 2, "--") == 0) {
                    size-=2;
                    // ignore the rest of the line due to the comment
                    while (fread(tokenString + size, 1, 1, f) > 0) {
                        if (tokenString[size] == '\n') {
                            break;
                        }
                    }
                    break;
                    size--;
                } else {
                    fseek(f, -1, SEEK_CUR);
                    size--;
                }

            } else if (in_string) {
                continue;
            } else if (!isalnum(*(tokenString + size - 1)) && *(tokenString + size - 1) != '_') {
                if (size > 1) {
                    size--;
                    fseek(f, -1, SEEK_CUR);
                }
                break;
            }
        };

        if (size == 0) {
            reached_eof = true;
            break;
        }

        // trim
        char *new_start = tokenString;
        while (size >= 1 && isspace(tokenString[0])) {
            if (tokenString[0] == '\n') line++;
            size--;
            new_start++;
        }
        while (size >= 1 && isspace(tokenString[size - 1])) {
            if (tokenString[size - 1] == '\n') line++;
            size--;
        }

        // skip over sections of just whitespace
        if (size == 0 || (size == 1 && isspace(tokenString[0]))) {
            continue;
        } 


        tokenString[size] = '\0';

        struct Token* t = malloc(sizeof(struct Token));
        t->type = TokenTypeOfString(size, new_start);
        t->file_name = filename;
        t->line = line;
        strncpy(t->data, new_start, BUF_SIZE);
        free(tokenString);

        if (first == NULL) {
            first = last = t;
        } else {
            last->next = t;
            last = last->next;
        }
    }

    return first;
}
int main(int argc, char* argv[argc]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s cool_files...\n", argv[0]);
        return 1;
    }

    struct Token *t = NULL;
    for (int i = 1; i < argc; i++) {
        struct Token* lexed = lex_file(argv[i]);

        if (lexed == NULL) {
            fprintf(stderr, "Failed to lex \"%s\"\n", argv[i]);
            return 1;
        }

        if (t == NULL) t = lexed;
        else {
            struct Token* last_token = t;
            while (last_token->next != NULL) {
                last_token = last_token->next;
            }
            last_token->next = lexed;
        }
    }

    char* last_file = NULL;
    struct Token* t1 = t;
    while (t1 != NULL) {
        if (last_file != t1->file_name) {
            last_file = t1->file_name;
            printf("#name \"%s\"\n", last_file);
        }

        printf("#%d %s", t1->line, TokenNames[t1->type]);

        switch (t1->type) {
            case ERROR:
            case BOOL_CONST:
            case OBJECTID: 
            case TYPEID:
            case STRING:
            case INTEGER:
                printf(" %s\n", t1->data);
                break;
            
            default: printf("\n");
        }

        struct Token* old = t1;
        t1 = t1->next;
        // free(old);
    }

    return 0;
}
