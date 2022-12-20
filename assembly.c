#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <err.h>
#include "syntax.h"

static const int WORD_SIZE = 4;
const int MAX_MNEMONIC_LENGTH = 1;

void emit_header(FILE *out, char *name) { fprintf(out, "%s\n", name); }

void emit_instr(FILE *out, char *instr, char *operands) {

    fprintf(out, "    %s", instr);

    int argument_offset = MAX_MNEMONIC_LENGTH - strlen(instr) + 4;
    while (argument_offset > 0) {
        fprintf(out, " ");
        argument_offset--;
    }

    fprintf(out, "%s;\n", operands);
}

void emit_instr_format(FILE *out, char *instr, char *operands_format, ...) {
    // The assembler requires at least 4 spaces for indentation.
    fprintf(out, "    %s", instr);

    // Ensure our argument are aligned, regardless of the assembly
    // mnemonic length.
    int argument_offset = MAX_MNEMONIC_LENGTH - strlen(instr) + 4;
    while (argument_offset > 0) {
        fprintf(out, " ");
        argument_offset--;
    }

    va_list argptr;
    va_start(argptr, operands_format);
    vfprintf(out, operands_format, argptr);
    va_end(argptr);

    fputs("\n", out);
}

void emit_function_prologue(FILE *out) {
    fprintf(out, ".align 4");
    fprintf(out, "\n\n");
}

void emit_function_declaration(FILE *out, char *name) {
    fprintf(out, ".global _%s\n", name);
    emit_function_prologue(out);
    fprintf(out, "_%s:\n", name);
}

void emit_return(FILE *out) {
    fprintf(out, "    ret;\n");
}

void emit_function_epilogue(FILE *out) {
    fprintf(out, "\n");
}

void write_header(FILE *out) { }

void write_footer(FILE *out) {
    // emit_function_prologue(out);
    emit_instr(out, "svc", "#0xFFFF");
}

void write_syntax(FILE *out, Syntax *syntax) {
    
    if (syntax->type == IMMEDIATE) {
        emit_instr_format(out, "mov", "X0, #%d", syntax->immediate->value);
    }  else if (syntax->type == RETURN_STATEMENT) {
        ReturnStatement *return_statement = syntax->return_statement;
        write_syntax(out, return_statement->expression);
        emit_return(out);

    } else if (syntax->type == BLOCK) {
        List *statements = syntax->block->statements;
        for (int i = 0; i < list_length(statements); i++) {
            write_syntax(out, list_get(statements, i));
        }
    }  else if (syntax->type == FUNCTION) {
        emit_function_declaration(out, syntax->function->name);
        write_syntax(out, syntax->function->root_block);
        emit_function_epilogue(out);
    } else if (syntax->type == TOP_LEVEL) {
        List *declarations = syntax->top_level->declarations;
        for (int i = 0; i < list_length(declarations); i++) {
            write_syntax(out, list_get(declarations, i));
        }

    }
}


void write_assembly(Syntax *syntax) {
    FILE *out = fopen("dd.asm", "wb");

    write_header(out);

    write_syntax(out, syntax);

    write_footer(out);

    fclose(out);
}