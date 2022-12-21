#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <err.h>

#include "stack.h"
#include "build/y.tab.h"
#include "syntax.h"
#include "assembly.h"

void print_help() {
    printf("dd\n");
    printf("To compile a file:\n");
    printf("    $ dd foo.dd\n");
    printf("To output the tokens without parsing:\n");
    printf("    $ dd --dump-tokens foo.dd\n");
    printf("To output the AST without compiling:\n");
    printf("    $ dd --dump-ast foo.dd\n");
    printf("To print this message:\n");
    printf("    $ dd --help\n\n");
}

extern Stack *syntax_stack;

extern int yyparse(void);
extern FILE *yyin;

typedef enum {
    TOKENIZE,
    PARSE,
    EMIT_ASM,
} stage_t;


int main(int argc, char *argv[])
{
     ++argv, --argc; /* Skip over program name. */
    
    stage_t terminate_at = EMIT_ASM;

    char *file_name;

    if (argc == 1 && strcmp(argv[0], "--help") == 0) {
        print_help();
        return 0;
    } else if (argc == 1) {
        file_name = argv[0];
    } else if (argc == 2 && strcmp(argv[0], "--dump-tokens") == 0) {
        terminate_at = TOKENIZE;
        file_name = argv[1];
    } else if (argc == 2 && strcmp(argv[0], "--dump-ast") == 0) {
        terminate_at = PARSE;
        file_name = argv[1];
    } else {
        print_help();
        return 1;
    }

    int result;

    yyin = fopen(file_name, "r");

    if (yyin == NULL) {
        printf("Could not open file: '%s'\n", file_name);
        result = 2;
        goto cleanup_file;
    }

    if (terminate_at == TOKENIZE) {
        int tokens;
        
        printf("Tokens \n");

        result = 1;
        goto cleanup_file;
    }


    syntax_stack = stack_new();

    result = yyparse();

    if (result != 0) {
        printf("done\n");
        goto cleanup_syntax;
    }


    Syntax *complete_syntax = stack_pop(syntax_stack);
    if (syntax_stack->size > 0) {
        warnx("Did not consume the whole syntax stack during parsing! Remaining:");
        
        while(syntax_stack->size > 0) {
            fprintf(stderr, "%s", syntax_type_name(stack_pop(syntax_stack)));
        }
    }

    printf("\n \n");
    if (terminate_at == PARSE) {
        printf("---AST---\n");
        print_syntax(complete_syntax);
    } else {
        write_assembly(complete_syntax, file_name);
        syntax_free(complete_syntax);

        printf("Written dd.asm.\n");
    }

    cleanup_syntax:
        stack_free(syntax_stack);
    
    cleanup_file:
        if (yyin != NULL) {
            fclose(yyin);
        }

    return result;
}
