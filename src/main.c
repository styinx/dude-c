#include "lexer/lexer.h"
#include "parser/parser.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    Lexer lexer;
    initializeLexer(&lexer, argv[1]);

    Parser parser;
    initializeParser(&parser);

    int max = 0;
    while(parse(&parser, tokenize(&lexer), lexer.word) && max < 5000) {
        printf("%-20s -> %s\n", lexer.word, tokenToString(lexer.tok));
        printf("%-20c   -> %d\n", ' ', parser.state);
        max++;
    };

    if(parser.state == ASTInvalid)
        lexError(&lexer, "Invalid syntax");

    finalizeLexer(&lexer);
    return 0;
}
