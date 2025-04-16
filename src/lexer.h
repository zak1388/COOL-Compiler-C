#ifndef LEXER_h
#define LEXER_h

#define BUF_SIZE 1027

enum TokenType {
    /* Punctuation */
    PERIOD,
    COMMA,
    AT,
    SEMICOLON,
    COLON,
    CURLY_OPEN,
    CURLY_CLOSE,
    PARENT_OPEN,
    PARENT_CLOSE,
    PLUS_OPERATOR,
    MINUS_OPERATOR,
    MULT_OPERATOR,
    DIV_OPERATOR,
    INT_COMPLEMENT_OPERATOR,
    LESS_OPERATOR,
    LESS_EQ_OPERATOR,
    EQ_OPERATOR,
    ASSIGN_OPERATOR,
    RIGHTARROW,

    /* Decision */
    IF,
    THEN,
    ELSE,
    FI,
    WHILE,
    LOOP,
    POOL,
    CASE,
    OF,
    ESAC,

    /* Keywords */
    LET,
    IN,
    NEW,
    ISVOID,
    INHERITS,
    CLAZZ,

    /* Literals */
    INTEGER,
    BOOL_CONST,
    STRING,

    TYPEID,
    OBJECTID,

    ERROR,
    EOF_TOKEN,
    TOKEN_COUNT,
};

char* TokenTypeMap[TOKEN_COUNT] = {
    /* Punctuation */
    [PERIOD] = ".",
    [COMMA] = ",",
    [AT] = "@",
    [SEMICOLON] = ";",
    [COLON] = ":",
    [CURLY_OPEN] = "{",
    [CURLY_CLOSE] = "}",
    [PARENT_OPEN] = "(",
    [PARENT_CLOSE] = ")",
    [PLUS_OPERATOR] = "+",
    [MINUS_OPERATOR] = "-",
    [MULT_OPERATOR] = "*",
    [DIV_OPERATOR] = "/",
    [INT_COMPLEMENT_OPERATOR] = "~",
    [LESS_OPERATOR] = "<",
    [LESS_EQ_OPERATOR] = "<=",
    [EQ_OPERATOR] = "=",
    [ASSIGN_OPERATOR] = "<-",
    [RIGHTARROW] = "=>",

    /* Decision */
    [IF] = "IF",
    [THEN] = "THEN",
    [ELSE] = "ELSE",
    [FI] = "FI",
    [WHILE] = "WHILE",
    [LOOP] = "LOOP",
    [POOL] = "POOL",
    [CASE] = "CASE",
    [OF] = "OF",
    [ESAC] = "ESAC",

    /* Keywords */
    [LET] = "LET",
    [IN] = "IN",
    [NEW] = "NEW",
    [ISVOID] = "ISVOID",
    [INHERITS] = "INHERITS",
    [CLAZZ] = "CLASS",

    /* Literals */
    [BOOL_CONST] = "BOOL_CONST",
    [INTEGER] = "INTEGER",
    [STRING] = "STRING",

    [TYPEID] = "TYPEID",
    [OBJECTID] = "OBJECTID",

    [ERROR] = "ERROR",
    [EOF_TOKEN] = "END_OF_FILE",
};

char* TokenNames[TOKEN_COUNT] = {
    /* Punctuation */
    [PERIOD] = "'.'",
    [COMMA] = "','",
    [AT] = "'@'",
    [SEMICOLON] = "';'",
    [COLON] = "':'",
    [CURLY_OPEN] = "'{'",
    [CURLY_CLOSE] = "'}'",
    [PARENT_OPEN] = "'('",
    [PARENT_CLOSE] = "')'",
    [PLUS_OPERATOR] = "'+'",
    [MINUS_OPERATOR] = "'-'",
    [MULT_OPERATOR] = "'*'",
    [DIV_OPERATOR] = "'/'",
    [INT_COMPLEMENT_OPERATOR] = "'~'",
    [LESS_OPERATOR] = "'<'",
    [LESS_EQ_OPERATOR] = "'<='",
    [EQ_OPERATOR] = "'='",
    [ASSIGN_OPERATOR] = "'<-'",
    [RIGHTARROW] = "'=>'",

    /* Decision */
    [IF] = "IF",
    [THEN] = "THEN",
    [ELSE] = "ELSE",
    [FI] = "FI",
    [WHILE] = "WHILE",
    [LOOP] = "LOOP",
    [POOL] = "POOL",
    [CASE] = "CASE",
    [OF] = "OF",
    [ESAC] = "ESAC",

    /* Keywords */
    [LET] = "LET",
    [IN] = "IN",
    [NEW] = "NEW",
    [ISVOID] = "ISVOID",
    [INHERITS] = "INHERITS",
    [CLAZZ] = "CLASS",

    /* Literals */
    [BOOL_CONST] = "BOOL_CONST",
    [INTEGER] = "INTEGER",
    [STRING] = "STRING",

    [TYPEID] = "TYPEID",
    [OBJECTID] = "OBJECTID",

    [ERROR] = "ERROR",
    [EOF_TOKEN] = "EOF_TOKEN",
};

struct Token {
    enum TokenType type;
    unsigned int line;
    char* file_name;
    char data[BUF_SIZE];
    struct Token* next;
};

#endif // LEXER_h
