#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_DEF: return "DEF";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MUL: return "MUL";
        case TOKEN_DIV: return "DIV";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NEQ: return "NEQ";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_LE: return "LE";
        case TOKEN_GE: return "GE";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_COLON: return "COLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_NEWLINE: return "NEWLINE";
        default: return "UNKNOWN";
    }
}

void lexer_init(Lexer *lexer, const char *source) {
    lexer->source = source;
    lexer->pos = 0;
    lexer->len = strlen(source);
    lexer->line = 1;
    lexer->col = 1;
}

static char peek(Lexer *lexer) {
    if (lexer->pos >= lexer->len) return 0;
    return lexer->source[lexer->pos];
}

static char advance(Lexer *lexer) {
    if (lexer->pos >= lexer->len) return 0;
    char c = lexer->source[lexer->pos++];
    lexer->col++;
    if (c == '\n') {
        lexer->line++;
        lexer->col = 1;
    }
    return c;
}

static Token make_token(Lexer *lexer, TokenType type, char *value) {
    Token token;
    token.type = type;
    token.value = value;
    token.line = lexer->line;
    token.col = lexer->col;
    return token;
}

void token_free(Token token) {
    if (token.value) {
        free(token.value);
    }
}

static int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static int is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

static Token lex_identifier_or_keyword(Lexer *lexer) {
    int start = lexer->pos - 1;
    while (is_alnum(peek(lexer))) {
        advance(lexer);
    }
    int length = lexer->pos - start;
    char *text = (char*)malloc(length + 1);
    strncpy(text, lexer->source + start, length);
    text[length] = '\0';

    TokenType type = TOKEN_IDENTIFIER;
    if (strcmp(text, "def") == 0) type = TOKEN_DEF;
    else if (strcmp(text, "return") == 0) type = TOKEN_RETURN;
    else if (strcmp(text, "if") == 0) type = TOKEN_IF;
    else if (strcmp(text, "else") == 0) type = TOKEN_ELSE;
    else if (strcmp(text, "while") == 0) type = TOKEN_WHILE;
    else if (strcmp(text, "print") == 0) type = TOKEN_PRINT;

    if (type != TOKEN_IDENTIFIER) {
        free(text);
        text = NULL;
    }
    
    return make_token(lexer, type, text);
}

static Token lex_number(Lexer *lexer) {
    int start = lexer->pos - 1;
    int is_float = 0;
    
    while (is_digit(peek(lexer))) {
        advance(lexer);
    }
    
    if (peek(lexer) == '.') {
        is_float = 1;
        advance(lexer);
        while (is_digit(peek(lexer))) {
            advance(lexer);
        }
    }
    
    int length = lexer->pos - start;
    char *text = (char*)malloc(length + 1);
    strncpy(text, lexer->source + start, length);
    text[length] = '\0';
    
    return make_token(lexer, is_float ? TOKEN_FLOAT : TOKEN_INT, text);
}

static Token lex_string(Lexer *lexer) {
    // Already consumed opening quote
    int start = lexer->pos;
    while (peek(lexer) != '"' && peek(lexer) != 0) {
        if (peek(lexer) == '\n') {
            // Error: multiline string not supported yet without triple quotes
            // But let's just handle it loosely
        }
        advance(lexer);
    }
    
    if (peek(lexer) == 0) {
        // Error: unterminated string
        return make_token(lexer, TOKEN_EOF, NULL); // Should return error token
    }
    
    int length = lexer->pos - start;
    char *text = (char*)malloc(length + 1);
    strncpy(text, lexer->source + start, length);
    text[length] = '\0';
    
    advance(lexer); // consume closing quote
    
    return make_token(lexer, TOKEN_STRING, text);
}

Token lexer_next_token(Lexer *lexer) {
    while (1) {
        char c = peek(lexer);
        
        if (c == 0) return make_token(lexer, TOKEN_EOF, NULL);
        
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lexer);
            continue;
        }
        
        if (c == '#') {
            while (peek(lexer) != '\n' && peek(lexer) != 0) {
                advance(lexer);
            }
            continue;
        }
        
        if (c == '\n') {
            advance(lexer);
            return make_token(lexer, TOKEN_NEWLINE, NULL);
        }
        
        if (is_alpha(c)) {
            advance(lexer);
            return lex_identifier_or_keyword(lexer);
        }
        
        if (is_digit(c)) {
            advance(lexer);
            return lex_number(lexer);
        }
        
        if (c == '"') {
            advance(lexer);
            return lex_string(lexer);
        }
        
        advance(lexer); // Consume symbol
        
        switch (c) {
            case '+': return make_token(lexer, TOKEN_PLUS, NULL);
            case '-': return make_token(lexer, TOKEN_MINUS, NULL);
            case '*': return make_token(lexer, TOKEN_MUL, NULL);
            case '/': return make_token(lexer, TOKEN_DIV, NULL);
            case '(': return make_token(lexer, TOKEN_LPAREN, NULL);
            case ')': return make_token(lexer, TOKEN_RPAREN, NULL);
            case ':': return make_token(lexer, TOKEN_COLON, NULL);
            case ',': return make_token(lexer, TOKEN_COMMA, NULL);
            case ';': return make_token(lexer, TOKEN_SEMICOLON, NULL);
            case '=': 
                if (peek(lexer) == '=') { advance(lexer); return make_token(lexer, TOKEN_EQ, NULL); }
                return make_token(lexer, TOKEN_ASSIGN, NULL);
            case '!':
                if (peek(lexer) == '=') { advance(lexer); return make_token(lexer, TOKEN_NEQ, NULL); }
                break; // Error
            case '<':
                if (peek(lexer) == '=') { advance(lexer); return make_token(lexer, TOKEN_LE, NULL); }
                return make_token(lexer, TOKEN_LT, NULL);
            case '>':
                if (peek(lexer) == '=') { advance(lexer); return make_token(lexer, TOKEN_GE, NULL); }
                return make_token(lexer, TOKEN_GT, NULL);
        }
        
        // Unknown token
        // return make_token(lexer, TOKEN_ERROR, NULL);
    }
}
