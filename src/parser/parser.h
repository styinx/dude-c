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
    ASTHelper,

    ASTNopStatement,

    ASTAssignmentStatement,
    ASTAssignmentStatementAssign,
    ASTAssignmentStatementValue,

    ASTDatStatement,
    ASTDatStatementName,
    ASTDatStatementIdentifier,

    ASTFunStatement,
    ASTFunStatementName,
    ASTFunStatementArgs,
    ASTFunStatementComma,
    ASTFunStatementEnd,

    ASTVariableWithTypeName,
    ASTVariableWithTypeColon,
    ASTVariableWithTypeType,

    // ASTVariableOptionalTypeName, // TODO non-deterministic state (look ahead)
    // ASTVariableOptionalTypeColon,
    // ASTVariableOptionalTypeType,

    ASTInvalid,
    ASTEnd
} ASTState;

#define MAX_SCOPES 64

typedef struct Parser
{
    ASTState state;
    ASTState stack[MAX_SCOPES];
    unsigned char scope;
} Parser;

void initializeParser(Parser* parser);

bool parse(Parser* parser, Token tok, const char * word);

/*
 * Helper
 */

void parseError(Parser* parser, const char* fmt, ...);

#endif  // HEADER_PARSER