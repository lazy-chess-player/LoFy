#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"

int main(int argc, char **argv)
{
    printf("LoFy Interpreter v0.1\n");
    printf("Type 'exit' to quit.\n");

    Environment env;
    env_init(&env);

    char buffer[1024];
    while (1)
    {
        printf(">>> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        if (strncmp(buffer, "exit", 4) == 0 && (buffer[4] == '\n' || buffer[4] == 0))
        {
            break;
        }

        Lexer lexer;
        lexer_init(&lexer, buffer);

        Parser parser;
        parser_init(&parser, &lexer);

        ASTNode *program = parser_parse(&parser);

        if (program)
        {
            // REPL behavior: if single expression, print result
            if (program->type == AST_BLOCK && program->block.count == 1)
            {
                ASTNode *stmt = program->block.statements[0];
                // Check if it's an expression that should be printed
                // Assignments and Print statements shouldn't auto-print
                if (stmt->type != AST_ASSIGNMENT && stmt->type != AST_PRINT)
                {
                    Value v = eval(stmt, &env);
                    if (v.type != VAL_NONE)
                    {
                        value_print(v);
                        printf("\n");
                    }
                    value_free(v);
                }
                else
                {
                    Value v = eval(program, &env);
                    value_free(v);
                }
            }
            else
            {
                Value v = eval(program, &env);
                value_free(v);
            }
            ast_free(program);
        }
    }

    return 0;
}
