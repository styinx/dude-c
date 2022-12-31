#include "types.h"
#include <string.h>

bool isType(const char* word)
{
    return strcmp(word, BOOL) == 0 || strcmp(word, CHAR) == 0 || strcmp(word, U8) == 0 ||
           strcmp(word, U16) == 0 || strcmp(word, U32) == 0 || strcmp(word, U64) == 0 ||
           strcmp(word, S8) == 0 || strcmp(word, S16) == 0 || strcmp(word, S32) == 0 ||
           strcmp(word, S64) == 0 || strcmp(word, F32) == 0 || strcmp(word, F64) == 0 ||
           strcmp(word, F) == 0;
}
