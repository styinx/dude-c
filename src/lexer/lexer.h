#ifndef HEADER_LEXER
#define HEADER_LEXER

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "symbols.h"
#include "tokens.h"

/*
 * Helper functions
 */

bool isNumeric(char c);

bool isHexDigit(char c);

bool isBinaryDigit(char c);

bool isSmallAlpha(char c);

bool isBigAlpha(char c);

bool isAlpha(char c);

bool isAlphaNumeric(char c);

bool isHardTokenSeparator(char c);

bool isBoolean(const char* word);

bool isNil(const char* word);

bool isNop(const char* word);

bool isAsm(const char* word);

/*
 * Buffer manipulation
 */

char* append(char* buffer, char c);

/*
 * Lexer
 */

typedef struct Context
{
    bool isComment;
    bool isBlockComment;
    bool lastIsComment;
    bool lastIsSingleQuote;
    bool lastIsDigit;
    bool lastIsExponent;
    bool floatPeriodRead;
    bool floatExponentRead;
    bool floatExponentSignRead;
} Context;

#define MAX_IDENTIFIER_LENGTH 255
#define LINE_LENGTH 255

typedef struct Lexer
{
    FILE*    input;
    unsigned col;
    unsigned line;
    char     dbgLine[LINE_LENGTH];
    char     word[MAX_IDENTIFIER_LENGTH];
    Token    tok;
    char     c;
    Context  context;
} Lexer;

bool initializeLexer(Lexer* lexer, const char* filename);

bool finalizeLexer(Lexer* lexer);

Token tokenize(Lexer* lexer);

/*
 * Tokenizing constant values
 */

bool isNumberSeparatorAtStart(Lexer* lexer);

bool isNumberSeparatorAtEnd(Lexer* lexer);

bool isNumberSeparatorDuplication(Lexer* lexer);

Token tokenizeIdentifier(Lexer* lexer);

Token tokenizeNumericConstant(Lexer* lexer);

Token tokenizeBinaryConstant(Lexer* lexer);

Token tokenizeHexadecimalConstant(Lexer* lexer);

Token tokenizeDecimalOrFloatConstant(Lexer* lexer);

/*
 * Helper
 */

Token lexError(Lexer* lexer, const char* fmt, ...);

const char* tokenToString(Token tok);

#endif  // HEADER_LEXER