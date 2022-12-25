#include "lexer.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char* AS    = "as";
static const char* DAT   = "dat";
static const char* ELIF  = "elif";
static const char* ELSE  = "else";
static const char* END   = "end";
static const char* FOR   = "for";
static const char* FUN   = "fun";
static const char* IF    = "if";
static const char* IN    = "in";
static const char* MOD   = "mod";
static const char* RET   = "ret";
static const char* WHILE = "while";
static const char* USE   = "use";

bool isNumeric(char c)
{
    return c >= '0' && c <= '9';
}

bool isHexDigit(char c)
{
    return isNumeric(c) || (c >= 'A' && c <= 'F');
}

bool isBinaryDigit(char c)
{
    return c == '0' || c == '1';
}

bool isSmallAlpha(char c)
{
    return c >= 'a' && c <= 'z';
}

bool isBigAlpha(char c)
{
    return c >= 'A' && c <= 'Z';
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
    return c == Space || c == Newline;
}

bool isKeyword(const char* word)
{
    return strcmp(word, AS) == 0 || strcmp(word, DAT) == 0 || strcmp(word, ELIF) == 0 ||
           strcmp(word, ELSE) == 0 || strcmp(word, END) == 0 || strcmp(word, FOR) == 0 ||
           strcmp(word, FUN) == 0 || strcmp(word, IF) == 0 || strcmp(word, IN) == 0 ||
           strcmp(word, MOD) == 0 || strcmp(word, RET) == 0 || strcmp(word, WHILE) == 0 ||
           strcmp(word, USE) == 0;
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
    lexer->tok                           = Undefined;
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
    lexer->tok                           = Undefined;
    lexer->context.floatPeriodRead       = false;
    lexer->context.lastIsSingleQuote     = false;
    lexer->context.lastIsDigit           = false;
    lexer->context.lastIsExponent        = false;
    lexer->context.floatExponentRead     = false;
    lexer->context.floatExponentSignRead = false;
    memset(lexer->word, 0, 255);

    next(lexer);

    while(lexer->c != EOF)
    {
        if(lexer->c == Newline)
        {
            lexer->context.isComment = false;

            lexer->col = 0;
            lexer->line += 1;
            memset(lexer->dbgLine, 0, 255);
            lexer->tok = Empty;
            return lexer->tok;
        }
        else if(lexer->c == NumberSign)
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

            return eatToken(lexer, Comment);
        }

        lexer->context.lastIsComment = false;

        if(lexer->context.isComment || lexer->context.isBlockComment)
        {
            lexer->tok = Comment;
            return lexer->tok;
        }

        switch(lexer->c)
        {
            // Valid control characters
            case AtSign:
            case DollarSign:
            case Period:
                return eatToken(lexer, Undefined);

            // Valid operator characters
            case Ampersand:
            case Asterisk:
            case Circumflex:
            case EqualSign:
            case ExclamationMark:
            case GreaterThanSign:
            case LessThanSign:
            case MinusSign:
            case PercentSign:
            case PlusSign:
            case QuestionMark:
            case Slash:
            case Tilde:
            case VerticalBar:
                return eatToken(lexer, Operator);

            // String constant
            case DoubleQuote:
                return eatToken(lexer, StringBorder);

            // Character constant
            case SingleQuote:
                return eatToken(lexer, CharacterBorder);

            // Block
            case CurlyBracesOpen:
                return eatToken(lexer, BlockBegin);
            case CurlyBracesClose:
                return eatToken(lexer, BlockEnd);

            // Expression
            case ParenthesisOpen:
                return eatToken(lexer, ExpressionBegin);
            case ParenthesisClose:
                return eatToken(lexer, ExpressionEnd);

            // Separators
            case Comma:
                return eatToken(lexer, CommaSeparator);
            case Colon:
                return eatToken(lexer, ColonSeparator);

            // Slice
            case BracketOpen:
                return eatToken(lexer, SliceBegin);
            case BracketClose:
                return eatToken(lexer, SliceBegin);

            // Hard Token separation
            case Space:
                lexer->tok = Empty;
                return lexer->tok;

            default:
                if(isAlphaNumeric(lexer->c))
                {
                    if(lexer->tok == Undefined)
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

    lexer->tok = End;
    return lexer->tok;
}

Token tokenizeIdentifier(Lexer* lexer)
{
    lexer->tok = Identifier;

    consumeChar(lexer);

    while(isAlphaNumeric(lexer->c))
        consumeChar(lexer);

    if(isKeyword(lexer->word))
        lexer->tok = Keyword;
    else if(isBoolean(lexer->word))
        lexer->tok = BooleanConstant;
    else if(isNil(lexer->word))
        lexer->tok = NilConstant;
    else if(isNop(lexer->word))
        lexer->tok = Nop;
    else if(isAsm(lexer->word))
        lexer->tok = Asm;

    previous(lexer);
    return lexer->tok;
}

/*
 * Tokenizing constant values
 */

bool isNumberSeparatorAtStart(Lexer* lexer)
{
    if(lexer->c == SingleQuote)
    {
        lexError(lexer, "Digit separators at start of number");
        return true;
    }
    return false;
}

bool isNumberSeparatorAtEnd(Lexer* lexer)
{
    if(lexer->word[strlen(lexer->word) - 1] == SingleQuote)
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
    if(lexer->c == SingleQuote)
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
    lexer->tok = NumericConstant;

    consumeChar(lexer);

    if(lexer->c == 'b')
        lexer->tok = tokenizeBinaryConstant(lexer);
    else if(lexer->c == 'x')
        lexer->tok = tokenizeHexadecimalConstant(lexer);
    else if(isNumeric(lexer->c) || lexer->c == Period || lexer->c == SingleQuote)
        lexer->tok = tokenizeDecimalOrFloatConstant(lexer);
    else if(lexer->c == EOF)
        return lexer->tok;
    else
        previous(lexer);

    return lexer->tok;
}

Token tokenizeBinaryConstant(Lexer* lexer)
{
    lexer->tok = BinaryConstant;

    // 'b'
    consumeChar(lexer);

    if(isNumberSeparatorAtStart(lexer))
    {
        return lexer->tok;
    }

    while(isBinaryDigit(lexer->c) || lexer->c == SingleQuote)
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
    lexer->tok = HexadecimalConstant;

    // 'x'
    consumeChar(lexer);

    if(isNumberSeparatorAtStart(lexer))
    {
        return lexer->tok;
    }

    while(isHexDigit(lexer->c) || lexer->c == SingleQuote)
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
    lexer->tok = DecimalConstant;

    while(isNumeric(lexer->c) || lexer->c == Period || lexer->c == SingleQuote ||
          lexer->c == SmallE || lexer->c == PlusSign || lexer->c == MinusSign)
    {
        if(lexer->c == Period)
        {
            if(isNumberSeparatorAtEnd(lexer))
            {
                return lexer->tok;
            }

            if(!lexer->context.floatPeriodRead)
            {
                lexer->context.floatPeriodRead = true;
                lexer->tok                     = FloatConstant;
            }
            else
            {
                return lexError(lexer, "Multiple periods in floating point number");
            }
        }
        else if(lexer->c == SingleQuote)
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
        else if(lexer->c == SmallE)
        {
            if(lexer->tok == DecimalConstant)
                return lexError(lexer, "Exponent in decimal number");

            if(lexer->context.floatExponentRead)
                return lexError(lexer, "Multiple exponents in floating point number");

            if(!lexer->context.lastIsDigit)
                return lexError(lexer, "Invalid exponent position");

            lexer->context.floatExponentRead = true;
            lexer->context.lastIsExponent    = true;
        }
        else if(lexer->c == PlusSign || lexer->c == MinusSign)
        {
            if(lexer->tok == DecimalConstant)
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
        if(lexer->c != SingleQuote)
            lexer->context.lastIsSingleQuote = false;
        if(lexer->c != SmallE)
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

    lexer->tok = Invalid;
    return lexer->tok;
}

const char* tokenToString(Token tok)
{
    switch(tok)
    {
        case Undefined:
            return "Undefined";

        case Comment:
            return "Comment";

        case NumericConstant:
            return "NumericConstant";
        case BinaryConstant:
            return "BinaryConstant";
        case HexadecimalConstant:
            return "HexadecimalConstant";
        case DecimalConstant:
            return "DecimalConstant";
        case FloatConstant:
            return "FloatConstant";
        case BooleanConstant:
            return "BooleanConstant";
        case NilConstant:
            return "NilConstant";

        case Nop:
            return "Nop";

        case Asm:
            return "Asm";

        case Identifier:
            return "Identifier";
        case Keyword:
            return "Keyword";
        case Operator:
            return "Operator";

        case StringBorder:
            return "StringBorder";
        case CharacterBorder:
            return "CharacterBorder";

        case BlockBegin:
            return "BlockBegin";
        case BlockEnd:
            return "BlockEnd";
        case SliceBegin:
            return "SliceBegin";
        case SliceEnd:
            return "SliceEnd";
        case ExpressionBegin:
            return "ExpressionBegin";
        case ExpressionEnd:
            return "ExpressionEnd";
        case ColonSeparator:
            return "ColonSeparator";
        case CommaSeparator:
            return "CommaSeparator";

        case Empty:
            return "Empty";

        case Invalid:
            return "Invalid";
        case End:
            return "End";
        default:
            return "UNKOWN TOKEN (TODO: add it!)";
    }
}
