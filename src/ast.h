#ifndef AST_H
#define AST_H

typedef enum {
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_PRINT,
    AST_BLOCK
} ASTNodeType;

struct ASTNode;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int int_val;
        double float_val;
        char *string_val; // For strings and identifiers
        struct {
            int op; // TokenType
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;
        struct {
            char *name;
            struct ASTNode *value;
        } assignment;
        struct {
            struct ASTNode *expr;
        } print_stmt;
        struct {
            struct ASTNode **statements;
            int count;
            int capacity;
        } block;
    };
} ASTNode;

ASTNode* ast_create_int(int value);
ASTNode* ast_create_float(double value);
ASTNode* ast_create_string(char *value);
ASTNode* ast_create_identifier(char *name);
ASTNode* ast_create_binary(int op, ASTNode *left, ASTNode *right);
ASTNode* ast_create_assignment(char *name, ASTNode *value);
ASTNode* ast_create_print(ASTNode *expr);
ASTNode* ast_create_block();
void ast_block_add(ASTNode *block, ASTNode *stmt);
void ast_free(ASTNode *node);

#endif
