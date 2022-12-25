#include "parser.h"
#include <stdio.h>

bool parse(Parser* parser, Token tok)
{
    if(tok == End || tok == Invalid)
    {
        return false;
    }
    return true;
}