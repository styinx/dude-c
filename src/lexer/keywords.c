#include "keywords.h"
#include <string.h>

bool isKeyword(const char* word)
{
    return strcmp(word, AND) == 0 || strcmp(word, AS) == 0 || strcmp(word, DAT) == 0 ||
           strcmp(word, ELIF) == 0 || strcmp(word, ELSE) == 0 || strcmp(word, END) == 0 ||
           strcmp(word, FOR) == 0 || strcmp(word, FUN) == 0 || strcmp(word, IF) == 0 ||
           strcmp(word, IN) == 0 || strcmp(word, IS) == 0 || strcmp(word, MOD) == 0 ||
           strcmp(word, OR) == 0 || strcmp(word, RET) == 0 || strcmp(word, WHILE) == 0 ||
           strcmp(word, USE) == 0;
}
