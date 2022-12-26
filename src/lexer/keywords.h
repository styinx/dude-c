#ifndef HEADER_KEYWORDS
#define HEADER_KEYWORDS

#include <stdbool.h>

static const char* AND   = "and";
static const char* AS    = "as";
static const char* DAT   = "dat";
static const char* ELIF  = "elif";
static const char* ELSE  = "else";
static const char* END   = "end";
static const char* FOR   = "for";
static const char* FUN   = "fun";
static const char* IF    = "if";
static const char* IN    = "in";
static const char* IS    = "is";
static const char* MOD   = "mod";
static const char* OR    = "or";
static const char* RET   = "ret";
static const char* WHILE = "while";
static const char* USE   = "use";

bool isKeyword(const char* word);

#endif  // HEADER_KEYWORDS