#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void parser_init(Parser *parser, Lexer *lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
}

static void advance(Parser *parser)
{
    token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static void eat(Parser *parser, TokenType type)
{
    if (parser->current_token.type == type)
    {
        advance(parser);
    }
    else
    {
        printf("Syntax Error: Expected %s, got %s at line %d col %d\n",
               token_type_to_string(type),
               token_type_to_string(parser->current_token.type),
               parser->current_token.line,
               parser->current_token.col);
        // Basic error recovery: just advance or exit?
        // For now, let's just advance to avoid infinite loops if possible,
        // but often it's better to panic in simple interpreters.
        // advance(parser);
    }
}

// Forward declarations
static ASTNode *parse_expression(Parser *parser);

static ASTNode *parse_factor(Parser *parser)
{
    Token token = parser->current_token;

    if (token.type == TOKEN_INT)
    {
        int val = atoi(token.value);
        advance(parser);
        return ast_create_int(val);
    }

    if (token.type == TOKEN_FLOAT)
    {
        double val = atof(token.value);
        advance(parser);
        return ast_create_float(val);
    }

    if (token.type == TOKEN_STRING)
    {
        ASTNode *node = ast_create_string(token.value);
        advance(parser);
        return node;
    }

    if (token.type == TOKEN_IDENTIFIER)
    {
        ASTNode *node = ast_create_identifier(token.value);
        advance(parser);
        return node;
    }

    if (token.type == TOKEN_LPAREN)
    {
        advance(parser);
        ASTNode *node = parse_expression(parser);
        eat(parser, TOKEN_RPAREN);
        return node;
    }

    printf("Syntax Error: Unexpected token %s in factor\n", token_type_to_string(token.type));
    advance(parser);
    return NULL;
}

static ASTNode *parse_term(Parser *parser)
{
    ASTNode *node = parse_factor(parser);

    while (parser->current_token.type == TOKEN_MUL || parser->current_token.type == TOKEN_DIV)
    {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_factor(parser);
        node = ast_create_binary(op, node, right);
    }

    return node;
}

static ASTNode *parse_expression(Parser *parser)
{
    ASTNode *node = parse_term(parser);

    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS ||
           parser->current_token.type == TOKEN_EQ ||
           parser->current_token.type == TOKEN_NEQ ||
           parser->current_token.type == TOKEN_LT ||
           parser->current_token.type == TOKEN_GT ||
           parser->current_token.type == TOKEN_LE ||
           parser->current_token.type == TOKEN_GE)
    {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_term(parser);
        node = ast_create_binary(op, node, right);
    }

    return node;
}

static ASTNode *parse_statement(Parser *parser)
{
    // Handle empty lines
    while (parser->current_token.type == TOKEN_NEWLINE)
    {
        advance(parser);
    }

    if (parser->current_token.type == TOKEN_PRINT)
    {
        advance(parser);
        eat(parser, TOKEN_LPAREN);
        ASTNode *expr = parse_expression(parser);
        eat(parser, TOKEN_RPAREN);
        eat(parser, TOKEN_NEWLINE);
        return ast_create_print(expr);
    }

    if (parser->current_token.type == TOKEN_IDENTIFIER)
    {
        // Check lookahead for assignment
        // Since we don't have lookahead buffer, we have to cheat or refactor.
        // Or we can parse identifier, then check next token.
        // But parse_expression consumes the identifier.
        // So we can parse_expression, and if the result is an IDENTIFIER node,
        // and the next token is =, then we convert it to assignment.

        // This is a bit tricky with pure recursive descent without lookahead.
        // Let's implement a "save" or just parse expression and see.
        // If we parse "x", we get Identifier Node.
        // Then we see "=".

        // But parse_expression parses "x + 1".
        // If we have "x = 1", parse_expression will parse "x" and stop at "=".

        ASTNode *expr = parse_expression(parser);

        if (parser->current_token.type == TOKEN_ASSIGN)
        {
            if (expr->type != AST_IDENTIFIER)
            {
                printf("Syntax Error: Cannot assign to non-identifier\n");
                // Cleanup
                return NULL;
            }

            char *name = strdup(expr->string_val);
            ast_free(expr); // Free the identifier node as we are replacing it

            advance(parser); // eat '='

            ASTNode *value = parse_expression(parser);
            eat(parser, TOKEN_NEWLINE);

            ASTNode *assign = ast_create_assignment(name, value);
            free(name);
            return assign;
        }

        // It was just an expression statement (e.g. "x + 1" or function call)
        // Consume newline
        if (parser->current_token.type == TOKEN_NEWLINE)
        {
            advance(parser);
        }
        else if (parser->current_token.type != TOKEN_EOF)
        {
            printf("Syntax Error: Expected newline after expression\n");
        }

        return expr;
    }

    // Fallback for other expressions
    if (parser->current_token.type == TOKEN_EOF)
        return NULL;

    ASTNode *expr = parse_expression(parser);
    if (parser->current_token.type == TOKEN_NEWLINE)
    {
        advance(parser);
    }
    return expr;
}

ASTNode *parser_parse(Parser *parser)
{
    ASTNode *block = ast_create_block();

    while (parser->current_token.type != TOKEN_EOF)
    {
        if (parser->current_token.type == TOKEN_NEWLINE)
        {
            advance(parser);
            continue;
        }

        ASTNode *stmt = parse_statement(parser);
        if (stmt)
        {
            ast_block_add(block, stmt);
        }
        else
        {
            // Error or EOF reached inside statement
            break;
        }
    }

    return block;
}
