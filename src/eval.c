#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "eval.h"
#include "token.h"

void env_init(Environment *env)
{
    env->head = NULL;
}

void env_set(Environment *env, const char *name, Value value)
{
    EnvNode *current = env->head;
    while (current)
    {
        if (strcmp(current->name, name) == 0)
        {
            // Update
            value_free(current->value);
            // Deep copy string if needed
            if (value.type == VAL_STRING && value.string_val)
            {
                value.string_val = strdup(value.string_val);
            }
            current->value = value;
            return;
        }
        current = current->next;
    }

    // New entry
    EnvNode *node = (EnvNode *)malloc(sizeof(EnvNode));
    node->name = strdup(name);
    // Deep copy string if needed
    if (value.type == VAL_STRING && value.string_val)
    {
        value.string_val = strdup(value.string_val);
    }
    node->value = value;
    node->next = env->head;
    env->head = node;
}

Value env_get(Environment *env, const char *name)
{
    EnvNode *current = env->head;
    while (current)
    {
        if (strcmp(current->name, name) == 0)
        {
            Value v = current->value;
            if (v.type == VAL_STRING && v.string_val)
            {
                v.string_val = strdup(v.string_val);
            }
            return v;
        }
        current = current->next;
    }
    Value v;
    v.type = VAL_NONE;
    return v;
}

Value eval(ASTNode *node, Environment *env)
{
    Value v = {0};
    v.type = VAL_NONE;

    if (!node)
        return v;

    switch (node->type)
    {
    case AST_INT:
        v.type = VAL_INT;
        v.int_val = node->int_val;
        return v;

    case AST_FLOAT:
        v.type = VAL_FLOAT;
        v.float_val = node->float_val;
        return v;

    case AST_STRING:
        v.type = VAL_STRING;
        v.string_val = strdup(node->string_val);
        return v;

    case AST_IDENTIFIER:
        return env_get(env, node->string_val);

    case AST_ASSIGNMENT:
    {
        Value val = eval(node->assignment.value, env);
        env_set(env, node->assignment.name, val);
        // Return a copy because env_set made a copy, and we return the original 'val'
        // But wait, if 'val' is string, we return it, and caller frees it.
        // env_set made its own copy. So this is safe.
        return val;
    }

    case AST_IF:
    {
        Value cond = eval(node->if_stmt.condition, env);
        int is_true = 0;
        if (cond.type == VAL_BOOL)
            is_true = cond.int_val;
        else if (cond.type == VAL_INT)
            is_true = (cond.int_val != 0);
        else if (cond.type == VAL_FLOAT)
            is_true = (cond.float_val != 0.0);

        value_free(cond);

        if (is_true)
        {
            return eval(node->if_stmt.then_branch, env);
        }
        else if (node->if_stmt.else_branch)
        {
            return eval(node->if_stmt.else_branch, env);
        }
        v.type = VAL_NONE;
            return v;
        }
        
        case AST_WHILE: {
            Value v_ret;
            v_ret.type = VAL_NONE;
            
            while (1) {
                Value cond = eval(node->while_loop.condition, env);
                int is_true = 0;
                if (cond.type == VAL_BOOL) is_true = cond.int_val;
                else if (cond.type == VAL_INT) is_true = (cond.int_val != 0);
                else if (cond.type == VAL_FLOAT) is_true = (cond.float_val != 0.0);
                
                value_free(cond);
                
                if (!is_true) break;
                
                Value body_val = eval(node->while_loop.body, env);
                value_free(body_val);
            }
            
            return v_ret;
        }
        
        case AST_PRINT: {
        Value val = eval(node->print_stmt.expr, env);
        value_print(val);
        printf("\n");
        value_free(val);
        v.type = VAL_NONE;
        return v;
    }

    case AST_BLOCK:
    {
        // Block returns the value of the last statement? Or None?
        // Usually statements return None, but expressions return values.
        // For REPL, we might want to print the last value if it's an expression.
        // But here AST_BLOCK is a sequence.
        for (int i = 0; i < node->block.count; i++)
        {
            Value res = eval(node->block.statements[i], env);
            // In a real language, return statement would break here.
            value_free(res);
        }
        v.type = VAL_NONE;
        return v;
    }

    case AST_BINARY_OP:
    {
        Value left = eval(node->binary.left, env);
        Value right = eval(node->binary.right, env);

        // Handle numeric ops
        if (left.type == VAL_INT && right.type == VAL_INT)
        {
            switch (node->binary.op)
            {
            case TOKEN_PLUS:
                v.type = VAL_INT;
                v.int_val = left.int_val + right.int_val;
                break;
            case TOKEN_MINUS:
                v.type = VAL_INT;
                v.int_val = left.int_val - right.int_val;
                break;
            case TOKEN_MUL:
                v.type = VAL_INT;
                v.int_val = left.int_val * right.int_val;
                break;
            case TOKEN_DIV:
                v.type = VAL_INT;
                if (right.int_val != 0)
                    v.int_val = left.int_val / right.int_val;
                else
                    printf("Runtime Error: Division by zero\n");
                break;
            case TOKEN_EQ:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val == right.int_val);
                break;
            case TOKEN_NEQ:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val != right.int_val);
                break;
            case TOKEN_LT:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val < right.int_val);
                break;
            case TOKEN_GT:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val > right.int_val);
                break;
            case TOKEN_LE:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val <= right.int_val);
                break;
            case TOKEN_GE:
                v.type = VAL_BOOL;
                v.int_val = (left.int_val >= right.int_val);
                break;
            default:
                v.type = VAL_NONE;
                break;
            }
        }
        else if ((left.type == VAL_INT || left.type == VAL_FLOAT) &&
                 (right.type == VAL_INT || right.type == VAL_FLOAT))
        {
            double l = (left.type == VAL_INT) ? (double)left.int_val : left.float_val;
            double r = (right.type == VAL_INT) ? (double)right.int_val : right.float_val;

            switch (node->binary.op)
            {
            case TOKEN_PLUS:
                v.type = VAL_FLOAT;
                v.float_val = l + r;
                break;
            case TOKEN_MINUS:
                v.type = VAL_FLOAT;
                v.float_val = l - r;
                break;
            case TOKEN_MUL:
                v.type = VAL_FLOAT;
                v.float_val = l * r;
                break;
            case TOKEN_DIV:
                v.type = VAL_FLOAT;
                if (r != 0)
                    v.float_val = l / r;
                else
                    printf("Runtime Error: Division by zero\n");
                break;
            case TOKEN_EQ:
                v.type = VAL_BOOL;
                v.int_val = (l == r);
                break;
            case TOKEN_NEQ:
                v.type = VAL_BOOL;
                v.int_val = (l != r);
                break;
            case TOKEN_LT:
                v.type = VAL_BOOL;
                v.int_val = (l < r);
                break;
            case TOKEN_GT:
                v.type = VAL_BOOL;
                v.int_val = (l > r);
                break;
            case TOKEN_LE:
                v.type = VAL_BOOL;
                v.int_val = (l <= r);
                break;
            case TOKEN_GE:
                v.type = VAL_BOOL;
                v.int_val = (l >= r);
                break;
            default:
                v.type = VAL_NONE;
                break;
            }
        }

        value_free(left);
        value_free(right);
        return v;
    }

    default:
        return v;
    }
}
