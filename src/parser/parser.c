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

bool push(Parser* parser, ASTState current, ASTState new)
{
    parser->scope += 1;
    parser->stack[parser->scope] = current;
    return setState(parser, new);
}

ASTState pop(Parser* parser)
{
    parser->state = parser->stack[parser->scope];
    parser->scope -= 1;
    return parser->state;
}

/*
 * Parser
 */

void initializeParser(Parser* parser)
{
    parser->state = ASTUndefined;
    memset(parser->stack, ASTUndefined, MAX_SCOPES);
    parser->scope = 0;
}

bool parse(Parser* parser, Token tok, const char* word)
{
    // Recover previous scope
    if(parser->state == ASTHelper && parser->scope > 0)
    {
        pop(parser);
    }

    if(tok == TKInvalid)
    {
        return setState(parser, ASTInvalid);
    }

    else if(tok == TKEnd)
    {
        setState(parser, ASTEnd);
        return false;
    }

    else if(tok == TKComment || tok == TKEmpty)
        return setState(parser, parser->state);
    
    /*
     * Helper states
     */
    
    // Identifier with type
    else if(parser->state == ASTVariableWithTypeName)
    {
        if(tok == TKColonSeparator)
            return setState(parser, ASTVariableWithTypeColon);
    }
    else if(parser->state == ASTVariableWithTypeColon)
    {
        if(tok == TKType)
            return setState(parser, ASTHelper);
    }

    /*
     * Statement states
     */

    // New statement
    else if(parser->state == ASTUndefined)
    {
        // Nop
        if(tok == TKNop)
            return setState(parser, ASTUndefined);

        // Assignment
        else if(tok == TKIdentifier)
            return push(parser, ASTAssignmentStatement, ASTVariableWithTypeName);

        // * Statement
        else if(tok == TKKeyword)
        {
            if(strcmp(word, DAT) == 0)
                return setState(parser, ASTDatStatement);
        }

        // Recover previous scope
        if(parser->scope > 0)
        {
            pop(parser);
            return parse(parser, tok, word);
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
            return push(parser, ASTDatStatementIdentifier, ASTVariableWithTypeName);
    }
    else if(parser->state == ASTDatStatementIdentifier)
    {
        if(tok == TKCommaSeparator)
            return setState(parser, ASTDatStatementName);
        else if(tok == TKKeyword && strcmp(word, END) == 0)
            return setState(parser, ASTUndefined);
    }

    // Assignment statement
    else if(parser->state == ASTAssignmentStatement)
    {
        if(tok == TKAssignment)
            return setState(parser, ASTAssignmentStatementAssign);
    }
    else if(parser->state == ASTAssignmentStatementAssign)
    {
        if(tok == TKBinaryConstant || tok == TKHexadecimalConstant || tok == TKDecimalConstant ||
           tok == TKFloatConstant || tok == TKBooleanConstant || tok == TKNilConstant)
            return setState(parser, ASTUndefined);
        else if(tok == TKKeyword && strcmp(word, FUN) == 0)
            return setState(parser, ASTFunStatement);
    }

    // Function Statement
    else if(parser->state == ASTFunStatement)
    {
        if(tok == TKIdentifier)
            return setState(parser, ASTFunStatementName);
        else if(tok == TKExpressionBegin)
            return setState(parser, ASTFunStatementArgs);
    }
    else if(parser->state == ASTFunStatementName)
    {
        if(tok == TKExpressionBegin)
            return setState(parser, ASTFunStatementArgs);
    }
    else if(parser->state == ASTFunStatementArgs)
    {
        if(tok == TKIdentifier)
            return push(parser, ASTFunStatementComma, ASTVariableWithTypeName);
        else if(tok == TKExpressionEnd)
            return push(parser, ASTFunStatementEnd, ASTUndefined);
    }
    else if(parser->state == ASTFunStatementComma)
    {
        if(tok == TKCommaSeparator)
            return setState(parser, ASTFunStatementArgs);
        else if(tok == TKExpressionEnd)
            return push(parser, ASTFunStatementEnd, ASTUndefined);
    }
    else if(parser->state == ASTFunStatementEnd)
    {
        if(tok == TKKeyword && strcmp(word, END) == 0)
            return setState(parser, ASTUndefined);
    }

    // Error state
    return setState(parser, ASTInvalid);
}