#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "syntax.h"
#include "list.h"

Syntax *immediate_new(int value) {
    Immediate *immediate = malloc(sizeof(Immediate));
    immediate->value = value;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = IMMEDIATE;
    syntax->immediate = immediate;

    return syntax;
}

Syntax *variable_new(char *var_name) {
    Variable *variable = malloc(sizeof(Variable));
    variable->var_name = var_name;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = VARIABLE;
    syntax->variable = variable;

    return syntax;
}

Syntax *function_call_new(char *function_name, Syntax *func_args) {
    FunctionCall *function_call = malloc(sizeof(FunctionCall));
    function_call->function_name = function_name;
    function_call->function_arguments = func_args;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION_CALL;
    syntax->function_call = function_call;

    return syntax;
}

Syntax *function_arguments_new() {
    FunctionArguments *func_args = malloc(sizeof(FunctionArguments));
    func_args->arguments = list_new();

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION_ARGUMENTS;
    syntax->function_arguments = func_args;

    return syntax;
}

Syntax *assignment_new(char *var_name, Syntax *expression) {
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->var_name = var_name;
    assignment->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = ASSIGNMENT;
    syntax->assignment = assignment;

    return syntax;
}

Syntax *return_statement_new(Syntax *expression) {
    ReturnStatement *return_statement = malloc(sizeof(ReturnStatement));
    return_statement->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = RETURN_STATEMENT;
    syntax->return_statement = return_statement;

    return syntax;
}

Syntax *block_new(List *statements) {
    Block *block = malloc(sizeof(Block));
    block->statements = statements;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BLOCK;
    syntax->block = block;

    return syntax;
}

Syntax *function_new(char *name, Syntax *root_block) {
    Function *function = malloc(sizeof(Function));
    function->name = name;
    function->parameters = NULL;
    function->root_block = root_block;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION;
    syntax->function = function;

    return syntax;
}

Syntax *top_level_new() {
    TopLevel *top_level = malloc(sizeof(TopLevel));
    top_level->declarations = list_new();

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = TOP_LEVEL;
    syntax->top_level = top_level;

    return syntax;
}

void syntax_list_free(List *syntaxes) {
    if (syntaxes == NULL) {
        return;
    }

    for (int i = 0; i < list_length(syntaxes); i++) {
        syntax_free(list_get(syntaxes, i));
    }

    list_free(syntaxes);
}

void syntax_free(Syntax *syntax) {
    if (syntax->type == IMMEDIATE) {
        free(syntax->immediate);

    } else if (syntax->type == VARIABLE) {
        free(syntax->variable->var_name);
        free(syntax->variable);

    } else if (syntax->type == FUNCTION_CALL) {
        syntax_free(syntax->function_call->function_arguments);
        free(syntax->function_call->function_name);
        free(syntax->function_call);

    } else if (syntax->type == FUNCTION_ARGUMENTS) {
        syntax_list_free(syntax->function_arguments->arguments);
        free(syntax->function_arguments);

    } else if (syntax->type == RETURN_STATEMENT) {
        syntax_free(syntax->return_statement->expression);
        free(syntax->return_statement);

    } else if (syntax->type == BLOCK) {
        syntax_list_free(syntax->block->statements);
        free(syntax->block);

    } else if (syntax->type == FUNCTION) {
        free(syntax->function->name);
        syntax_free(syntax->function->root_block);

        free(syntax->function);

    } else if (syntax->type == ASSIGNMENT) {
        free(syntax->assignment->var_name);
        syntax_free(syntax->assignment->expression);

        free(syntax->assignment);

    } else if (syntax->type == TOP_LEVEL) {
        syntax_list_free(syntax->top_level->declarations);
        free(syntax->top_level);
    } else {
        warnx("Could not free syntax tree with type: %s",
              syntax_type_name(syntax));
    }

    free(syntax);
}

char *syntax_type_name(Syntax *syntax) {
    if (syntax->type == IMMEDIATE) {
        return "IMMEDIATE";
    } else if (syntax->type == VARIABLE) {
        return "VARIABLE";
    }else if (syntax->type == FUNCTION_CALL) {
        return "FUNCTION CALL";
    } else if (syntax->type == FUNCTION_ARGUMENTS) {
        return "FUNCTION ARGUMENTS";
    } else if (syntax->type == RETURN_STATEMENT) {
        return "RETURN";
    } else if (syntax->type == BLOCK) {
        return "BLOCK";
    } else if (syntax->type == FUNCTION) {
        return "FUNCTION";
    } else if (syntax->type == ASSIGNMENT) {
        return "ASSIGNMENT";
    } else if (syntax->type == TOP_LEVEL) {
        return "TOP LEVEL";
    }

    // Should never be reached.
    return "??? UNKNOWN SYNTAX";
}

void print_syntax_indented(Syntax *syntax, int indent) {
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    char *syntax_type_string = syntax_type_name(syntax);

    if (syntax->type == IMMEDIATE) {
        printf("%s %d\n", syntax_type_string, syntax->immediate->value);
    } else if (syntax->type == VARIABLE) {
        printf("%s '%s'\n", syntax_type_string, syntax->variable->var_name);
    } else if (syntax->type == FUNCTION_CALL) {
        printf("%s '%s'\n", syntax_type_string,
               syntax->function_call->function_name);
        print_syntax_indented(syntax->function_call->function_arguments,
                              indent);

    } else if (syntax->type == FUNCTION_ARGUMENTS) {
        printf("%s\n", syntax_type_string);

        List *arguments = syntax->function_arguments->arguments;
        Syntax *argument;
        for (int i = 0; i < list_length(arguments); i++) {
            argument = list_get(arguments, i);
            print_syntax_indented(argument, indent + 4);
        }

    } else if (syntax->type == RETURN_STATEMENT) {
        printf("%s\n", syntax_type_string);
        print_syntax_indented(syntax->return_statement->expression, indent + 4);

    } else if (syntax->type == BLOCK) {
        printf("%s\n", syntax_type_string);

        List *statements = syntax->block->statements;
        for (int i = 0; i < list_length(statements); i++) {
            print_syntax_indented(list_get(statements, i), indent + 4);
        }

    } else if (syntax->type == FUNCTION) {
        printf("%s '%s'\n", syntax_type_string, syntax->function->name);
        print_syntax_indented(syntax->function->root_block, indent + 4);

    } else if (syntax->type == ASSIGNMENT) {
        printf("%s '%s'\n", syntax_type_string, syntax->assignment->var_name);
        print_syntax_indented(syntax->assignment->expression, indent + 4);

    } else if (syntax->type == TOP_LEVEL) {
        printf("%s\n", syntax_type_string);

        List *declarations = syntax->top_level->declarations;
        for (int i = 0; i < list_length(declarations); i++) {
            print_syntax_indented(list_get(declarations, i), indent + 4);
        }

    } else {
        printf("??? UNKNOWN SYNTAX TYPE\n");
    }
}

void print_syntax(Syntax *syntax) { print_syntax_indented(syntax, 0); }