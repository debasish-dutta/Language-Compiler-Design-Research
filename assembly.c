#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <err.h>
#include "syntax.h"
#include "env.h"
#include "context.h"

static int is_M1 = 0;
static const int WORD_SIZE = 16;
const int MAX_MNEMONIC_LENGTH = 7;

void check_target_architecture() {
#ifdef TARGET_ARCH_M1
    // printf("Target architecture: M1\n");
    is_M1=1;
#endif
// printf("is m1 %d", is_M1);
}

void emit_header(FILE *out, char *name) { fprintf(out, "%s\n", name); }


void emit_instr(FILE *out, char *instr, char *operands) {
    // TODO: fix duplication with emit_instr_format.
    // The assembler requires at least 4 spaces for indentation.
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

char *fresh_local_label(char *prefix, Context *ctx) {
    // We assume we never write more than 6 chars of digits, plus a '.' and '_'.
    size_t buffer_size = strlen(prefix) + 8;
    char *buffer = malloc(buffer_size);

    snprintf(buffer, buffer_size, ".%s_%d", prefix, ctx->label_count);
    ctx->label_count++;

    return buffer;
}

void emit_label(FILE *out, char *label) { fprintf(out, "%s:\n", label); }

void emit_function_prologue(FILE *out) {
    if(is_M1){
        fprintf(out, ".align 4");
    } else {
        fprintf(out, ".align 2");
    }
    fprintf(out, "\n\n");
}

void emit_function_declaration(FILE *out, char *name) {
    fprintf(out, ".global _%s\n", name);
    emit_function_prologue(out);
    fprintf(out, "_%s:\n", name);
}


void emit_return(FILE *out) {
    // fprintf(out, "    leave\n");
    // fprintf(out, "    ret\n");
}

void emit_function_epilogue(FILE *out) {
    fprintf(out, "\n");
}

void write_header(FILE *out) {  }

void write_footer(FILE *out) {
    // TODO: this will break if a user defines a function called '_start'.

    fprintf(out, "\n");

    // Exiting
    if (is_M1) {
        emit_instr(out, "mov", "x16, #1");
    } else {
        emit_instr(out, "mov", "x8, #93");
    }

    emit_instr(out, "svc", "#0xFFFF");
}

void write_syntax(FILE *out, Syntax *syntax, Context *ctx) {
    // Note stack_offset is the next unused memory address in the
    // stack, so we can use it directly but must adjust it for the next caller.
    if (syntax->type == UNARY_OPERATOR) {
        UnaryExpression *unary_syntax = syntax->unary_expression;

        write_syntax(out, unary_syntax->expression, ctx);

        if (unary_syntax->unary_type == NEGATION) {
            emit_instr(out, "neg", "X0, X0");
        } else if (unary_syntax->unary_type == BITWISE_NEGATION) {
            emit_instr(out, "mvn", "X0, X0");
        }  else if (unary_syntax->unary_type == LOGICAL_NEGATION) {
            emit_instr(out, "cmp", "X0, #0");
            emit_instr(out, "cset", "X0, eq");
        }
    } else if (syntax->type == IMMEDIATE) {
        emit_instr_format(out, "mov", "X0, #%d", syntax->immediate->value);

    } else if (syntax->type == VARIABLE) {
        emit_instr_format(
            out, "ldr", "X0, [sp, #%d]",
            environment_get_offset(ctx->env, syntax->variable->var_name));
    } else if (syntax->type == ASSIGNMENT) {
        write_syntax(out, syntax->assignment->expression, ctx);

        emit_instr_format(
            out, "str", "X0, [sp, #%d]",
            environment_get_offset(ctx->env, syntax->variable->var_name));

    } else if (syntax->type == BINARY_OPERATOR) {
        BinaryExpression *binary_syntax = syntax->binary_expression;
        int stack_offset = ctx->stack_offset;
        ctx->stack_offset -= WORD_SIZE;

        // emit_instr(out, "sub", "sp, sp, #16");
        write_syntax(out, binary_syntax->left, ctx);
        emit_instr_format(out, "str", "X0, [sp, #%d]", stack_offset);

        write_syntax(out, binary_syntax->right, ctx);

        if (binary_syntax->binary_type == MULTIPLICATION) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "mul", "X0, X0, X1");

        } else if (binary_syntax->binary_type == ADDITION) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "add", "X0, X0, X1");

        } else if (binary_syntax->binary_type == SUBTRACTION) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "sub", "X0, X1, X0", stack_offset);

        } else if (binary_syntax->binary_type == GREATER) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "cmp", "X1, X0", stack_offset);
            emit_instr_format(out, "cset", "X0, gt", stack_offset);

        } else if (binary_syntax->binary_type == LESS) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "cmp", "X1, X0", stack_offset);
            emit_instr_format(out, "cset", "X0, lt", stack_offset);

        } else if (binary_syntax->binary_type == AND) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "and", "X0, X0, X1", stack_offset);

        } else if (binary_syntax->binary_type == OR) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "orr", "X0, X0, X1", stack_offset);

        } else if (binary_syntax->binary_type == EQUALS) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "cmp", " X0, X1", stack_offset);
            emit_instr_format(out, "cset", " X0, eq", stack_offset);

        } else if (binary_syntax->binary_type == GREATER_EQUALS) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "cmp", " X1, X0", stack_offset);
            emit_instr_format(out, "cset", " X0, ge", stack_offset);

        } else if (binary_syntax->binary_type == LESS_EQUALS) {
            emit_instr_format(out, "ldr", "X1, [sp, #%d]", stack_offset);
            emit_instr_format(out, "cmp", " X1, X0", stack_offset);
            emit_instr_format(out, "cset", " X0, le", stack_offset);
        }

    
    } else if (syntax->type == RETURN_STATEMENT) {
        ReturnStatement *return_statement = syntax->return_statement;
        write_syntax(out, return_statement->expression, ctx);

        emit_return(out);
    } else if (syntax->type == IF_STATEMENT) {
        IfStatement *if_statement = syntax->if_statement;
        write_syntax(out, if_statement->condition, ctx);

        char *label_end = fresh_local_label("if_end", ctx);
        char *label_else = fresh_local_label("if_else", ctx);

        emit_instr(out, "cmp", "x0, #0");
        emit_instr_format(out, "beq", "%s", label_else);

        write_syntax(out, if_statement->then_stmts, ctx);
        emit_instr_format(out, "b", "%s", label_end);

        emit_label(out, label_else);
        if (if_statement->else_stmts != NULL) {
            write_syntax(out, if_statement->else_stmts, ctx);
        }
        emit_label(out, label_end);
    } else if (syntax->type == FUNCTION_CALL) {
        emit_instr_format(out, "call", syntax->function_call->function_name);

    } else if (syntax->type == DEFINE_VAR) {
        DefineVarStatement *define_var_statement = syntax->define_var_statement;
        int stack_offset = ctx->stack_offset;

        environment_set_offset(ctx->env, define_var_statement->var_name,
                               stack_offset);
        // emit_instr(out, "sub", "sp, sp, #16");

        ctx->stack_offset -= WORD_SIZE;
        write_syntax(out, define_var_statement->init_value, ctx);
        emit_instr_format(out, "str", "X0, [sp, #%d]\n", stack_offset);

    } else if (syntax->type == BLOCK) {
        List *statements = syntax->block->statements;
        for (int i = 0; i < list_length(statements); i++) {
            write_syntax(out, list_get(statements, i), ctx);
        }
    } else if (syntax->type == FUNCTION) {
        new_scope(ctx);

        if(is_M1){
            emit_function_declaration(out, syntax->function->name);
        } else{
            char *start = "start";
            emit_function_declaration(out, start);
        }
        // emit_function_prologue(out);
        write_syntax(out, syntax->function->root_block, ctx);
        emit_function_epilogue(out);

    } else if (syntax->type == TOP_LEVEL) {
        // TODO: treat the 'main' function specially.
        List *declarations = syntax->top_level->declarations;
        for (int i = 0; i < list_length(declarations); i++) {
            write_syntax(out, list_get(declarations, i), ctx);
        }

    } else {
        warnx("Unknown syntax %s", syntax_type_name(syntax));
        assert(false);
    }
}

void write_assembly(Syntax *syntax, char *filename)
{

    FILE *out = fopen(filename, "wb");

    write_header(out);

    Context *ctx = new_context();
    write_syntax(out, syntax, ctx);

    write_footer(out);

    context_free(ctx);
    fclose(out);
}
