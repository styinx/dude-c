#ifndef HEADER_TOKENS
#define HEADER_TOKENS

typedef enum Token
{
    TKUndefined,

    TKComment,

    TKNumericConstant,
    TKBinaryConstant,
    TKHexadecimalConstant,
    TKDecimalConstant,
    TKFloatConstant,
    TKBooleanConstant,
    TKNilConstant,

    TKNop,

    TKAsm,

    TKIdentifier,
    TKKeyword,
    TKType,
    
    TKAssignment,

    TKOperatorMultiplication,
    TKOperatorDivision,
    TKOperatorAddition,
    TKOperatorSubtraction,
    TKOperatorModulo,
    TKOperatorAND,
    TKOperatorOR,
    TKOperatorXOR,
    TKOperatorCOMP,
    TKOperatorGreaterThan,
    TKOperatorLessThan,

    TKStringBorder,
    TKCharacterBorder,

    TKBlockBegin,
    TKBlockEnd,
    TKSliceBegin,
    TKSliceEnd,
    TKExpressionBegin,
    TKExpressionEnd,
    TKCommaSeparator,
    TKColonSeparator,

    TKEmpty,

    TKInvalid,
    TKEnd,
} Token;

#endif  // HEADER_TOKENS