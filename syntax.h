#include "list.h"

#ifndef BABYC_SYNTAX_HEADER
#define BABYC_SYNTAX_HEADER

typedef enum {
    IMMEDIATE,
    VARIABLE,
    // We already use 'RETURN' and 'IF' as token names.
    BLOCK,
    RETURN_STATEMENT,
    FUNCTION,
    FUNCTION_CALL,
    FUNCTION_ARGUMENTS,
    ASSIGNMENT,
    TOP_LEVEL
} SyntaxType;


struct Syntax;
typedef struct Syntax Syntax;

typedef struct Immediate { int value; } Immediate;

typedef struct Variable {
    // TODO: once we have other types, we will need to store type here.
    char *var_name;
} Variable;


typedef struct FunctionArguments { List *arguments; } FunctionArguments;

typedef struct FunctionCall {
    char *function_name;
    Syntax *function_arguments;
} FunctionCall;

typedef struct Assignment {
    char *var_name;
    Syntax *expression;
} Assignment;

typedef struct ReturnStatement { Syntax *expression; } ReturnStatement;

typedef struct Block { List *statements; } Block;

typedef struct Function {
    char *name;
    List *parameters;
    Syntax *root_block;
} Function;

typedef struct Parameter {
    // TODO: once we have other types, we will need to store type here.
    char *name;
} Parameter;

typedef struct TopLevel { List *declarations; } TopLevel;

struct Syntax {
    SyntaxType type;
    union {
        Immediate *immediate;

        Variable *variable;

        Assignment *assignment;

        ReturnStatement *return_statement;

        FunctionArguments *function_arguments;

        FunctionCall *function_call;

        Block *block;

        Function *function;

        TopLevel *top_level;
    };
};

Syntax *immediate_new(int value);

Syntax *variable_new(char *var_name);

Syntax *function_call_new(char *function_name, Syntax *func_args);

Syntax *function_arguments_new();

Syntax *assignment_new(char *var_name, Syntax *expression);

Syntax *return_statement_new(Syntax *expression);

Syntax *block_new(List *statements);

Syntax *function_new(char *name, Syntax *root_block);

Syntax *top_level_new();

void syntax_free(Syntax *syntax);

char *syntax_type_name(Syntax *syntax);

void print_syntax(Syntax *syntax);

#endif