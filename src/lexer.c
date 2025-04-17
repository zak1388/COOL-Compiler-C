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


    /* Try to match things exactly */
    for (int i = 0; i < TOKEN_COUNT; i++) {
        if (size == (int) strlen(TokenTypeMap[i]) && strncasecmp(tokenString, TokenTypeMap[i], size) == 0) {
            return i;
        }
    }

    /* Literals */
    bool isInt = true;
    bool isString = tokenString[0] == '"' && tokenString[size - 1] == '"';
    bool isBool = islower(tokenString[0]) && (strncasecmp(tokenString, "true", size) == 0 || strncasecmp(tokenString, "false", size) == 0);
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

    if (isInt) return INT_CONST;
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

void consume_block_comment(FILE* f, int *line) {
    int comment_level = 0;
    char possible_end[3] = {0};
    do {
        if ((possible_end[0] = getc(f)) == EOF || ((possible_end[1] = getc(f)) == EOF)) {
            break;
        }

        if (possible_end[0] == '\n') {
            (*line)++;
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
        if (tokenString == NULL) {
            fprintf(stderr, "Failed to allocate memory in %s:%d\n", __FILE__, __LINE__);
            exit(1);
        }
        int size = 0;
        bool in_string = false;
        struct Token* t = malloc(sizeof(struct Token));
        while (size < BUF_SIZE - 2 && (tokenString[size] = getc(f)) != EOF) {
            size++;

            if (size == 1 && isspace(tokenString[0])) {
                // skip leading whitespace
                if (tokenString[0] == '\n') {
                    line++;
                }
                size--;
                continue;
            } 

            if (size > 0 && tokenString[size - 1] == '"') {
                // entering a string const
                in_string ^= true;
            } else if (in_string) {
                // cannot stop if we're in string
                continue;
            } 

            // consume line comments
            if (size > 0 && tokenString[size - 1] == '-') {
                if  ((tokenString[size] = getc(f)) == '-') {
                    char c;
                    size--;
                    do {
                        c = getc(f);
                    } while (c != '\n' && c != EOF);
                    line++;
                    continue;
                } else {
                    ungetc(tokenString[size], f);
                }
            } 

            if ((t->type = TokenTypeOfString(size, tokenString)) != ERROR) {
                // double check that this isnt a block comment
                if (t->type == PARENT_OPEN) {
                    if ((tokenString[size] = getc(f)) != EOF
                            && tokenString[size] == '*') {
                        ungetc(tokenString[size], f);
                        size--;
                        ungetc(tokenString[size], f);
                        consume_block_comment(f, &line);
                        continue;
                    } else {
                        ungetc(tokenString[size], f);
                    }
                } 
                bool isIdentifierLike = isalpha(tokenString[0]); 
                printf("isIdentifierLike %s\n", isIdentifierLike? "true" : "false");
                for (int i = 1; i < size; i++) {
                    isIdentifierLike &= isalnum(tokenString[i]) || tokenString[i] == '_';
                }
                if (isIdentifierLike) {
                    // if its an identifier keep going until theres no more identifier
                    tokenString[size] = getc(f);
                    bool can_read_more = isalnum(tokenString[size]) || tokenString[size] == '_';
                    ungetc(tokenString[size], f);
                    if (can_read_more) {
                        continue;
                    }
                }


                // stop because we matched the type of a token
                break;
            }
        };

        if (size == 0) {
            reached_eof = true;
            break;
        }

        // skip over sections of just whitespace
        if (size == 0 || (size == 1 && isspace(tokenString[0]))) {
            continue;
        } 

        tokenString[size] = '\0';
        t->file_name = filename;
        t->line = line;
        strncpy(t->data, tokenString, BUF_SIZE);
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
            case INT_CONST:
                printf(" %s\n", t1->data);
                break;
            
            default: printf("\n");
        }

        // struct Token* old = t1;
        t1 = t1->next;
        // free(old);
    }

    return 0;
}
