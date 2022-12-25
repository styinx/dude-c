#ifndef HEADER_PARSER
#define HEADER_PARSER

#include <stdbool.h>
#include "../lexer/tokens.h"

/*
 * Parser
 */

typedef struct Parser
{
    int dummy;
} Parser;

bool parse(Parser * parser, Token tok);

/*
 * Helper
 */

void parseError(Parser* parser, const char* fmt, ...);

#endif // HEADER_PARSER