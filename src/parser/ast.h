#ifndef HEADER_AST
#define HEADER_AST

typedef struct Identifier
{
    const char* name;
} Identifier;

typedef struct AsignmentExpression
{
    Identifier Identifier;
} AsignmentExpression;

#endif // HEADER_AST