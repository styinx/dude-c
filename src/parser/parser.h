#ifndef HEADER_PARSER
#define HEADER_PARSER

#include "../lexer/tokens.h"
#include <stdbool.h>

/*
 * Parser
 */

// A bad state machine
typedef enum ASTState
{
    ASTUndefined,

    ASTNopStatement,

    ASTAssignmentStatement,
    ASTAssignmentStatementColon,
    ASTAssignmentStatementType,
    ASTAssignmentStatementAssign,

    ASTDatStatement,
    ASTDatStatementName,
    ASTDatStatementIdentifier,
    ASTDatStatementIdentifierColon,
    ASTDatStatementIdentifierType,
    ASTDatStatementIdentifierComma,

    ASTInvalid
} ASTState;

typedef struct Parser
{
    ASTState state;
} Parser;

void initializeParser(Parser* parser);

bool parse(Parser* parser, Token tok, const char * word);

/*
 * Helper
 */

void parseError(Parser* parser, const char* fmt, ...);

#endif  // HEADER_PARSER