#include "parser.h"
#include "../lexer/keywords.h"
#include "../lexer/types.h"
#include <stdio.h>
#include <string.h>

/*
 * Private helpers
 */

bool setState(Parser* parser, ASTState state)
{
    parser->state = state;
    return parser->state != ASTInvalid;
}

/*
 * Parser
 */

void initializeParser(Parser* parser)
{
    parser->state = ASTUndefined;
}

bool parse(Parser* parser, Token tok, const char* word)
{
    if(tok == TKEnd || tok == TKInvalid)
    {
        return setState(parser, ASTInvalid);
    }

    else if(tok == TKComment || tok == TKEmpty)
        return setState(parser, parser->state);

    // New statement
    else if(parser->state == ASTUndefined)
    {
        // Nop
        if(tok == TKNop)
            return setState(parser, ASTUndefined);

        // Assignment
        else if(tok == TKIdentifier)
            return setState(parser, ASTAssignmentStatement);

        // * Statement
        else if(tok == TKKeyword)
        {
            if(strcmp(word, DAT) == 0)
                return setState(parser, ASTDatStatement);
        }
    }

    // Dat statement
    else if(parser->state == ASTDatStatement)
    {
        if(tok == TKIdentifier)
            return setState(parser, ASTDatStatementName);
    }
    else if(parser->state == ASTDatStatementName)
    {
        if(tok == TKIdentifier)
            return setState(parser, ASTDatStatementIdentifier);
    }
    else if(parser->state == ASTDatStatementIdentifier)
    {
        if(tok == TKColonSeparator)
            return setState(parser, ASTDatStatementIdentifierColon);
    }
    else if(parser->state == ASTDatStatementIdentifierColon)
    {
        if(tok == TKType)
            return setState(parser, ASTDatStatementIdentifierType);
    }
    else if(parser->state == ASTDatStatementIdentifierType)
    {
        if(tok == TKCommaSeparator)
            return setState(parser, ASTDatStatementIdentifierComma);
        else if(tok == TKKeyword && strcmp(word, END) == 0)
            return setState(parser, ASTUndefined);
    }
    else if(parser->state == ASTDatStatementIdentifierComma)
    {
        if(tok == TKIdentifier)
            return setState(parser, ASTDatStatementIdentifier);
    }

    // Assignment statement
    else if(parser->state == ASTAssignmentStatement)
    {
        if(tok == TKColonSeparator)
            return setState(parser, ASTAssignmentStatementColon);
        else if(tok == TKAssignment)
            return setState(parser, ASTAssignmentStatementAssign);
    }
    else if(parser->state == ASTAssignmentStatementColon)
    {
        if(tok == TKType)
            return setState(parser, ASTAssignmentStatementType);
    }
    else if(parser->state == ASTAssignmentStatementType)
    {
        if(tok == TKAssignment)
            return setState(parser, ASTAssignmentStatementAssign);
    }
    else if(parser->state == ASTAssignmentStatementAssign)
    {
        if(tok == TKBinaryConstant || tok == TKHexadecimalConstant || tok == TKDecimalConstant ||
           tok == TKFloatConstant || tok == TKBooleanConstant || tok == TKNilConstant)
            return setState(parser, ASTUndefined);
    }

    return setState(parser, ASTInvalid);
}