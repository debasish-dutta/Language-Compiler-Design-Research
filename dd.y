%{

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../syntax.h"
#include "../stack.h"

void yyerror(char *s);

#define YYSTYPE char*

int yyparse(void);
int yylex();

int yywrap()
{
	return 1;
}

extern FILE *yyin;

Stack *syntax_stack;
%}
%token IDENTIFIER CONSTANT 
%token LBRACK '(' RBRACK ')' LCURLYBRACK '{' RCURLYBRACK '}' SEMICOLON ';'
%token NEG '-' BNEG '~' LNEG '!'
%token ADD '+' MULTI '*' DIV "/"
%token RETURN FUN
%start library
%left  LBRACK LCURLYBRACK
%left NEG
%left ADD
%left MULTI
%left DIV
%nonassoc LNEG
%nonassoc BNEG

%%
library
    : functions                                                             {}{  Syntax *top_level_syntax;
                                                                if (stack_empty(syntax_stack)) {
                                                                    top_level_syntax = top_level_new();
                                                                } else if (((Syntax *)stack_peek(syntax_stack))->type != TOP_LEVEL) {
                                                                    top_level_syntax = top_level_new();
                                                                } else {
                                                                    top_level_syntax = stack_pop(syntax_stack);
                                                                }

                                                                list_push(top_level_syntax->top_level->declarations,
                                                                        stack_pop(syntax_stack));
                                                                stack_push(syntax_stack, top_level_syntax);} 
    ;
functions
    : FUN IDENTIFIER parameter_decl LCURLYBRACK  statement RCURLYBRACK      {} {Syntax *current_syntax = stack_pop(syntax_stack); stack_push(syntax_stack, function_new((char*)$2, current_syntax));}
    | /* empty */                                                           {}
    ;
parameter_decl
    : LBRACK parameter  RBRACK          {}
    ;
parameter
    : /* empty */                       {}
    ;
statement
    : RETURN expression SEMICOLON       {}{ Syntax *current_syntax = stack_pop(syntax_stack);
                                            stack_push(syntax_stack, return_statement_new(current_syntax));} 
    | SEMICOLON
    ;
expression
    : CONSTANT                          {} {stack_push(syntax_stack, immediate_new(atoi((char *)$1)));free($1);}
    | LBRACK expression RBRACK          {}
    | NEG expression                    {} { Syntax *current_syntax = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, negation_new(current_syntax));
                                            }
    | BNEG expression                   {} { Syntax *current_syntax = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, bitwise_negation_new(current_syntax));
                                            }
    | LNEG expression                   {} { Syntax *current_syntax = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, logical_negation_new(current_syntax));
                                            }
	| expression ADD expression         {} { Syntax *right = stack_pop(syntax_stack);
                                                Syntax *left = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, addition_new(left, right));
                                            }
	| expression NEG expression         {} { Syntax *right = stack_pop(syntax_stack);
                                                Syntax *left = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, subtraction_new(left, right));
                                            }
	| expression MULTI expression       {} { Syntax *right = stack_pop(syntax_stack);
                                                Syntax *left = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, multiplication_new(left, right));
                                            }
	| expression DIV expression         {} { Syntax *right = stack_pop(syntax_stack);
                                                Syntax *left = stack_pop(syntax_stack);
                                                stack_push(syntax_stack, division_new(left, right));
                                            }
    ;
%%
/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char yytext[];

char st[100][10];
int top=0;
char tmp[3] = "t0"; */
extern int column;

void yyerror(char *s)
{
	fflush(stdout);
    fprintf(stderr,"error: %s\n",s);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}
