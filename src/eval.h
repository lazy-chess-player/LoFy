#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "value.h"

// Simple environment
typedef struct EnvNode {
    char *name;
    Value value;
    struct EnvNode *next;
} EnvNode;

typedef struct {
    EnvNode *head;
} Environment;

void env_init(Environment *env);
void env_set(Environment *env, const char *name, Value value);
Value env_get(Environment *env, const char *name); 

Value eval(ASTNode *node, Environment *env);

#endif
