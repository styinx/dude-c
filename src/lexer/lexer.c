#include "lexer.h"
#include "keywords.h"
#include "types.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

bool isNumeric(char c)
{
    return c >= SYMZero && c <= SYMNine;
}

bool isHexDigit(char c)
{
    return isNumeric(c) || (c >= SYMBigA && c <= SYMBigF);
}

bool isBinaryDigit(char c)
{
    return c == SYMZero || c == SYMOne;
}

bool isSmallAlpha(char c)
{
    return c >= SYMSmallA && c <= SYMSmallZ;
}

bool isBigAlpha(char c)
{
    return c >= SYMBigA && c <= SYMBigZ;
}

bool isAlpha(char c)
{
    return isSmallAlpha(c) || isBigAlpha(c);
}

bool isAlphaNumeric(char c)
{
    return isAlpha(c) || isNumeric(c);
}

bool isHardTokenSeparator(char c)
{
    return c == SYMSpace || c == SYMNewline;
}

bool isBoolean(const char* word)
{
    return strcmp(word, "true") == 0 || strcmp(word, "false") == 0;
}

bool isNil(const char* word)
{
    return strcmp(word, "nil") == 0;
}

bool isNop(const char* word)
{
    return strcmp(word, "nop") == 0;
}

bool isAsm(const char* word)
{
    return strcmp(word, "asm") == 0;
}

/*
 * Buffer manipulation
 */

char* append(char* buffer, char c)
{
    unsigned len    = strlen(buffer);
    buffer[len]     = c;
    buffer[len + 1] = '\0';
    return buffer;
}

/*
 * Private lexer helpers
 */

void next(Lexer* lexer)
{
    lexer->c = fgetc(lexer->input);
    append(lexer->dbgLine, lexer->c);
    lexer->col++;
}

void previous(Lexer* lexer)
{
    fseek(lexer->input, -1, SEEK_CUR);
    lexer->dbgLine[strlen(lexer->dbgLine) - 1] = '\0';
    lexer->col--;
}

Token eatToken(Lexer* lexer, Token tok)
{
    append(lexer->word, lexer->c);
    lexer->tok = tok;
    return lexer->tok;
}

void consumeChar(Lexer* lexer)
{
    append(lexer->word, lexer->c);
    next(lexer);
}

/*
 * Lexer functions
 */

bool initializeLexer(Lexer* lexer, const char* filename)
{
    lexer->tok                           = TKUndefined;
    lexer->c                             = '\0';
    lexer->col                           = 0;
    lexer->line                          = 1;
    lexer->context.isComment             = false;
    lexer->context.isBlockComment        = false;
    lexer->context.lastIsComment         = false;
    lexer->context.floatPeriodRead       = false;
    lexer->context.lastIsSingleQuote     = false;
    lexer->context.lastIsDigit           = false;
    lexer->context.lastIsExponent        = false;
    lexer->context.floatExponentRead     = false;
    lexer->context.floatExponentSignRead = false;

    return fopen_s(&lexer->input, filename, "r");
}

bool finalizeLexer(Lexer* lexer)
{
    return fclose(lexer->input);
}

Token tokenize(Lexer* lexer)
{
    lexer->tok                           = TKUndefined;
    lexer->context.floatPeriodRead       = false;
    lexer->context.lastIsSingleQuote     = false;
    lexer->context.lastIsDigit           = false;
    lexer->context.lastIsExponent        = false;
    lexer->context.floatExponentRead     = false;
    lexer->context.floatExponentSignRead = false;
    memset(lexer->word, 0, MAX_IDENTIFIER_LENGTH);

    next(lexer);

    while(lexer->c != EOF)
    {
        if(lexer->c == SYMNewline)
        {
            lexer->context.isComment = false;

            lexer->col = 0;
            lexer->line += 1;
            memset(lexer->dbgLine, 0, LINE_LENGTH);
            lexer->tok = TKEmpty;
            return lexer->tok;
        }
        else if(lexer->c == SYMNumberSign)
        {
            // Block comment handling
            if(lexer->context.lastIsComment)
            {
                if(lexer->context.isBlockComment)
                    lexer->context.isBlockComment = false;
                else
                    lexer->context.isBlockComment = true;
            }
            // Single line comment
            else
            {
                lexer->context.isComment = true;
            }

            lexer->context.lastIsComment = true;

            return eatToken(lexer, TKComment);
        }

        lexer->context.lastIsComment = false;

        if(lexer->context.isComment || lexer->context.isBlockComment)
        {
            lexer->tok = TKComment;
            return lexer->tok;
        }

        switch(lexer->c)
        {
            // Valid control characters
            case SYMAtSign:
            case SYMDollarSign:
            case SYMPeriod:
                return eatToken(lexer, TKUndefined);

            case SYMEqualSign:
                return eatToken(lexer, TKAssignment);

            // Operator characters
            case SYMAsterisk:
                return eatToken(lexer, TKOperatorMultiplication);
            case SYMSlash:
                return eatToken(lexer, TKOperatorDivision);
            case SYMPlusSign:
                return eatToken(lexer, TKOperatorAddition);
            case SYMMinusSign:
                return eatToken(lexer, TKOperatorDivision);
            case SYMPercentSign:
                return eatToken(lexer, TKOperatorModulo);
            case SYMAmpersand:
                return eatToken(lexer, TKOperatorAND);
            case SYMVerticalBar:
                return eatToken(lexer, TKOperatorOR);
            case SYMCircumflex:
                return eatToken(lexer, TKOperatorXOR);
            case SYMTilde:
                return eatToken(lexer, TKOperatorCOMP);
            case SYMGreaterThanSign:
                return eatToken(lexer, TKOperatorGreaterThan);
            case SYMLessThanSign:
                return eatToken(lexer, TKOperatorLessThan);

            // String constant
            case SYMDoubleQuote:
                return eatToken(lexer, TKStringBorder);

            // Character constant
            case SYMSingleQuote:
                return eatToken(lexer, TKCharacterBorder);

            // Block
            case SYMCurlyBracesOpen:
                return eatToken(lexer, TKBlockBegin);
            case SYMCurlyBracesClose:
                return eatToken(lexer, TKBlockEnd);

            // Expression
            case SYMParenthesisOpen:
                return eatToken(lexer, TKExpressionBegin);
            case SYMParenthesisClose:
                return eatToken(lexer, TKExpressionEnd);

            // Separators
            case SYMComma:
                return eatToken(lexer, TKCommaSeparator);
            case SYMColon:
                return eatToken(lexer, TKColonSeparator);

            // Slice
            case SYMBracketOpen:
                return eatToken(lexer, TKSliceBegin);
            case SYMBracketClose:
                return eatToken(lexer, TKSliceBegin);

            // Hard Token separation
            case SYMSpace:
                lexer->tok = TKEmpty;
                return lexer->tok;

            default:
                if(isAlphaNumeric(lexer->c))
                {
                    if(lexer->tok == TKUndefined)
                    {
                        if(isNumeric(lexer->c))
                            return tokenizeNumericConstant(lexer);
                        else
                            return tokenizeIdentifier(lexer);
                    }
                }
                else
                {
                    return lexError(lexer, "Unkwon symbol");
                }
        }

        next(lexer);
    }

    lexer->tok = TKEnd;
    return lexer->tok;
}

Token tokenizeIdentifier(Lexer* lexer)
{
    lexer->tok = TKIdentifier;

    consumeChar(lexer);

    while(isAlphaNumeric(lexer->c))
        consumeChar(lexer);

    if(isKeyword(lexer->word))
        lexer->tok = TKKeyword;
    else if(isType(lexer->word))
        lexer->tok = TKType;
    else if(isBoolean(lexer->word))
        lexer->tok = TKBooleanConstant;
    else if(isNil(lexer->word))
        lexer->tok = TKNilConstant;
    else if(isNop(lexer->word))
        lexer->tok = TKNop;
    else if(isAsm(lexer->word))
        lexer->tok = TKAsm;

    previous(lexer);
    return lexer->tok;
}

/*
 * Tokenizing constant values
 */

bool isNumberSeparatorAtStart(Lexer* lexer)
{
    if(lexer->c == SYMSingleQuote)
    {
        lexError(lexer, "Digit separators at start of number");
        return true;
    }
    return false;
}

bool isNumberSeparatorAtEnd(Lexer* lexer)
{
    if(lexer->word[strlen(lexer->word) - 1] == SYMSingleQuote)
    {
        lexer->c = lexer->word[strlen(lexer->word) - 1];
        previous(lexer);
        lexError(lexer, "Digit separators at end of number");
        return true;
    }
    return false;
}

bool isNumberSeparatorDuplication(Lexer* lexer)
{
    if(lexer->c == SYMSingleQuote)
    {
        if(lexer->context.lastIsSingleQuote)
        {
            lexError(lexer, "Repeated digit separators");
            return true;
        }
        else
        {
            lexer->context.lastIsSingleQuote = true;
        }
    }
    else
    {
        lexer->context.lastIsSingleQuote = false;
    }
    return false;
}

Token tokenizeNumericConstant(Lexer* lexer)
{
    lexer->tok = TKNumericConstant;

    consumeChar(lexer);

    if(lexer->c == SYMSmallB)
        lexer->tok = tokenizeBinaryConstant(lexer);
    else if(lexer->c == SYMSmallX)
        lexer->tok = tokenizeHexadecimalConstant(lexer);
    else if(isNumeric(lexer->c) || lexer->c == SYMPeriod || lexer->c == SYMSingleQuote)
        lexer->tok = tokenizeDecimalOrFloatConstant(lexer);
    else if(lexer->c == EOF || lexer->c == SYMNewline || lexer->c == SYMSpace)
        lexer->tok = TKDecimalConstant;
    else
        previous(lexer);

    return lexer->tok;
}

Token tokenizeBinaryConstant(Lexer* lexer)
{
    lexer->tok = TKBinaryConstant;

    // 'b'
    consumeChar(lexer);

    if(isNumberSeparatorAtStart(lexer))
    {
        return lexer->tok;
    }

    while(isBinaryDigit(lexer->c) || lexer->c == SYMSingleQuote)
    {
        if(isNumberSeparatorDuplication(lexer))
        {
            return lexer->tok;
        }

        consumeChar(lexer);
    }

    if(isNumberSeparatorAtEnd(lexer))
    {
        return lexer->tok;
    }

    previous(lexer);
    return lexer->tok;
}

Token tokenizeHexadecimalConstant(Lexer* lexer)
{
    lexer->tok = TKHexadecimalConstant;

    // 'x'
    consumeChar(lexer);

    if(isNumberSeparatorAtStart(lexer))
    {
        return lexer->tok;
    }

    while(isHexDigit(lexer->c) || lexer->c == SYMSingleQuote)
    {
        if(isNumberSeparatorDuplication(lexer))
        {
            return lexer->tok;
        }

        consumeChar(lexer);
    }

    if(isNumberSeparatorAtEnd(lexer))
    {
        return lexer->tok;
    }

    previous(lexer);
    return lexer->tok;
}

Token tokenizeDecimalOrFloatConstant(Lexer* lexer)
{
    lexer->tok = TKDecimalConstant;

    while(isNumeric(lexer->c) || lexer->c == SYMPeriod || lexer->c == SYMSingleQuote ||
          lexer->c == SYMSmallE || lexer->c == SYMPlusSign || lexer->c == SYMMinusSign)
    {
        if(lexer->c == SYMPeriod)
        {
            if(isNumberSeparatorAtEnd(lexer))
            {
                return lexer->tok;
            }

            if(!lexer->context.floatPeriodRead)
            {
                lexer->context.floatPeriodRead = true;
                lexer->tok                     = TKFloatConstant;
            }
            else
            {
                return lexError(lexer, "Multiple periods in floating point number");
            }
        }
        else if(lexer->c == SYMSingleQuote)
        {
            if(lexer->context.lastIsSingleQuote)
            {
                return lexError(lexer, "Repeated digit separators");
            }
            else
            {
                if(lexer->context.floatPeriodRead || lexer->context.floatExponentRead)
                    return lexError(lexer, "Digit separators in mantissa or exponent");

                lexer->context.lastIsSingleQuote = true;
            }
        }
        else if(lexer->c == SYMSmallE)
        {
            if(lexer->tok == TKDecimalConstant)
                return lexError(lexer, "Exponent in decimal number");

            if(lexer->context.floatExponentRead)
                return lexError(lexer, "Multiple exponents in floating point number");

            if(!lexer->context.lastIsDigit)
                return lexError(lexer, "Invalid exponent position");

            lexer->context.floatExponentRead = true;
            lexer->context.lastIsExponent    = true;
        }
        else if(lexer->c == SYMPlusSign || lexer->c == SYMMinusSign)
        {
            if(lexer->tok == TKDecimalConstant)
                return lexError(lexer, "Sign in decimal number");

            if(lexer->context.floatExponentSignRead)
                return lexError(lexer, "Multiple exponents signs in floating point number");

            if(!lexer->context.lastIsExponent)
                return lexError(lexer, "Invalid exponent sign position");

            lexer->context.floatExponentSignRead = true;
        }
        else
        {
            lexer->context.lastIsDigit = true;
        }

        if(!isNumeric(lexer->c))
            lexer->context.lastIsDigit = false;
        if(lexer->c != SYMSingleQuote)
            lexer->context.lastIsSingleQuote = false;
        if(lexer->c != SYMSmallE)
            lexer->context.lastIsExponent = false;

        consumeChar(lexer);
    }

    if(isNumberSeparatorAtEnd(lexer))
        return lexer->tok;

    previous(lexer);
    return lexer->tok;
}

/*
 * Helper
 */

Token lexError(Lexer* lexer, const char* fmt, ...)
{
    printf("\n");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf(" while lexing token '\33[33m%s\033[0m'", tokenToString(lexer->tok));
    printf(
        " in line \33[36m%d\033[0m at position \033[36m%d\033[0m: '\33[31m%c\033[0m'\n\n",
        lexer->line,
        lexer->col,
        lexer->c);
    printf("%*c%s\n", 10, ' ', lexer->dbgLine);
    printf("%*c\n\n", 10 + lexer->col, '^');

    lexer->tok = TKInvalid;
    return lexer->tok;
}

const char* tokenToString(Token tok)
{
    switch(tok)
    {
        case TKUndefined:
            return "TKUndefined";

        case TKComment:
            return "TKComment";

        case TKNumericConstant:
            return "TKNumericConstant";
        case TKBinaryConstant:
            return "TKBinaryConstant";
        case TKHexadecimalConstant:
            return "TKHexadecimalConstant";
        case TKDecimalConstant:
            return "TKDecimalConstant";
        case TKFloatConstant:
            return "TKFloatConstant";
        case TKBooleanConstant:
            return "TKBooleanConstant";
        case TKNilConstant:
            return "TKNilConstant";

        case TKNop:
            return "TKNop";

        case TKAsm:
            return "TKAsm";

        case TKIdentifier:
            return "TKIdentifier";
        case TKKeyword:
            return "TKKeyword";
        case TKType:
            return "TKType";

        case TKAssignment:
            return "TKAssignment";

        case TKOperatorMultiplication:
            return "TKOperatorMultiplication";
        case TKOperatorDivision:
            return "TKOperatorDivision";
        case TKOperatorAddition:
            return "TKOperatorAddition";
        case TKOperatorSubtraction:
            return "TKOperatorSubtraction";
        case TKOperatorModulo:
            return "TKOperatorModulo";
        case TKOperatorAND:
            return "TKOperatorAND";
        case TKOperatorOR:
            return "TKOperatorOR";
        case TKOperatorXOR:
            return "TKOperatorXOR";
        case TKOperatorCOMP:
            return "TKOperatorCOMP";
        case TKOperatorGreaterThan:
            return "TKOperatorGreaterThan";
        case TKOperatorLessThan:
            return "TKOperatorLessThan";

        case TKStringBorder:
            return "TKStringBorder";
        case TKCharacterBorder:
            return "TKCharacterBorder";

        case TKBlockBegin:
            return "TKBlockBegin";
        case TKBlockEnd:
            return "TKBlockEnd";
        case TKSliceBegin:
            return "TKSliceBegin";
        case TKSliceEnd:
            return "TKSliceEnd";
        case TKExpressionBegin:
            return "TKExpressionBegin";
        case TKExpressionEnd:
            return "TKExpressionEnd";
        case TKColonSeparator:
            return "TKColonSeparator";
        case TKCommaSeparator:
            return "TKCommaSeparator";

        case TKEmpty:
            return "TKEmpty";

        case TKInvalid:
            return "TKInvalid";
        case TKEnd:
            return "TKEnd";
        default:
            return "TKUNKOWN TOKEN (TODO: add it!)";
    }
}
