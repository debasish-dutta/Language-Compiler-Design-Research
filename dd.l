%{
#include <stdio.h>
#define YYSTYPE char*
#include "y.tab.h"

void count();
void comment();

void yyerror();
%}
%%
"!//"                         { comment();  }
"return"                     { count(); return(RETURN); }
"fun"                        { count(); return(FUN); }
"("                          { count(); return(LBRACK); }
")"                          { count(); return(RBRACK); }
"{"                          { count(); return(LCURLYBRACK); }
"}"                          { count(); return(RCURLYBRACK); }
[a-zA-Z][_a-zA-Z0-9]*	     { count(); yylval = strdup(yytext); return(IDENTIFIER); }
[0-9]+?                      { count(); yylval = strdup(yytext); return(CONSTANT); }
";"                          { count(); return(SEMICOLON); }
[ \t\v\f]                    { count(); }
\n                           { count(); }
.                            { /* ignore bad characters */;}
%%

/* int yywrap(void)
{
    return 1;
} */
#define INPUT_EOF 0

int column = 0;

void comment()
{
	char c, c1;

loop:
	while ((c = yytext[0]) != '!' && c != 0)
		putchar(c);

	if ((c1 = yytext[1]) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}

	if (c != 0)
		putchar(c1);
}

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
	ECHO;
}
