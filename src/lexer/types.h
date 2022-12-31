#ifndef HEADER_TYPES
#define HEADER_TYPES

#include <stdbool.h>

static const char* BOOL = "Bool";
static const char* CHAR = "Char";
static const char* U8   = "U8";
static const char* U16  = "U16";
static const char* U32  = "U32";
static const char* U64  = "U64";
static const char* S8   = "S8";
static const char* S16  = "S16";
static const char* S32  = "S32";
static const char* S64  = "S64";
static const char* F32  = "F32";
static const char* F64  = "F64";
static const char* F    = "F";

bool isType(const char* word);

#endif  // HEADER_TYPES