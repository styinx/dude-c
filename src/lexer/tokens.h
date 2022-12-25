#ifndef HEADER_TOKENS
#define HEADER_TOKENS

typedef enum Token
{
    Undefined,

    Comment,

    NumericConstant,
    BinaryConstant,
    HexadecimalConstant,
    DecimalConstant,
    FloatConstant,
    BooleanConstant,
    NilConstant,

    Nop,

    Asm,

    Identifier,
    Keyword,
    Operator,

    StringBorder,
    CharacterBorder,

    BlockBegin,
    BlockEnd,
    SliceBegin,
    SliceEnd,
    ExpressionBegin,
    ExpressionEnd,
    CommaSeparator,
    ColonSeparator,

    Empty,

    Invalid,
    End,
} Token;

#endif // HEADER_TOKENS