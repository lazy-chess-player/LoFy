#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode* ast_create_node(ASTNodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* ast_create_int(int value) {
    ASTNode *node = ast_create_node(AST_INT);
    node->int_val = value;
    return node;
}

ASTNode* ast_create_float(double value) {
    ASTNode *node = ast_create_node(AST_FLOAT);
    node->float_val = value;
    return node;
}

ASTNode* ast_create_string(char *value) {
    ASTNode *node = ast_create_node(AST_STRING);
    node->string_val = strdup(value);
    return node;
}

ASTNode* ast_create_identifier(char *name) {
    ASTNode *node = ast_create_node(AST_IDENTIFIER);
    node->string_val = strdup(name);
    return node;
}

ASTNode* ast_create_binary(int op, ASTNode *left, ASTNode *right) {
    ASTNode *node = ast_create_node(AST_BINARY_OP);
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* ast_create_assignment(char *name, ASTNode *value) {
    ASTNode *node = ast_create_node(AST_ASSIGNMENT);
    node->assignment.name = strdup(name);
    node->assignment.value = value;
    return node;
}

ASTNode* ast_create_print(ASTNode *expr) {
    ASTNode *node = ast_create_node(AST_PRINT);
    node->print_stmt.expr = expr;
    return node;
}

ASTNode* ast_create_block() {
    ASTNode *node = ast_create_node(AST_BLOCK);
    node->block.statements = NULL;
    node->block.count = 0;
    node->block.capacity = 0;
    return node;
}

void ast_block_add(ASTNode *block, ASTNode *stmt) {
    if (block->type != AST_BLOCK) return;
    
    if (block->block.count >= block->block.capacity) {
        int new_capacity = block->block.capacity == 0 ? 4 : block->block.capacity * 2;
        block->block.statements = (ASTNode**)realloc(block->block.statements, new_capacity * sizeof(ASTNode*));
        block->block.capacity = new_capacity;
    }
    block->block.statements[block->block.count++] = stmt;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_STRING:
        case AST_IDENTIFIER:
            if (node->string_val) free(node->string_val);
            break;
        case AST_BINARY_OP:
            ast_free(node->binary.left);
            ast_free(node->binary.right);
            break;
        case AST_ASSIGNMENT:
            if (node->assignment.name) free(node->assignment.name);
            ast_free(node->assignment.value);
            break;
        case AST_PRINT:
            ast_free(node->print_stmt.expr);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->block.count; i++) {
                ast_free(node->block.statements[i]);
            }
            if (node->block.statements) free(node->block.statements);
            break;
        default:
            break;
    }
    free(node);
}
