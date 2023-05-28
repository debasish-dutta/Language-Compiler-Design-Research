#include <stdlib.h>

#ifndef ENV_HEADER
#define ENV_HEADER

typedef struct VarWithOffset {
    char *var_name;
    int offset;
} VarWithOffset;

typedef struct Environment {
    size_t size;
    VarWithOffset *items;
} Environment;

Environment *environment_new();

void environment_set_offset(Environment *env, char *var_name, int offset);

int environment_get_offset(Environment *env, char *var_name);

void environment_free(Environment *env);

#endif