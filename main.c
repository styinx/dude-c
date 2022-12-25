#include "lexer/lexer.h"
#include "parser/parser.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    Lexer lexer;
    initializeLexer(&lexer, argv[1]);

    Parser parser;

    int max = 0;
    while(parse(&parser, tokenize(&lexer)) && max < 5000) {
        printf("%-20s -> %s\n", lexer.word, tokenToString(lexer.tok));
        max++;
    };

    finalizeLexer(&lexer);
    return 0;
}
