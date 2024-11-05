#ifndef LEXER_H
#define LEXER_H

#include <string> // for std::string 
#include <cstdlib>  // for std::exit
#include <iostream> // IO

struct TokenType
{
    enum Token
    {
        ENDOFFILE = -1, // EOF is apparently a macro already defined, so this has to be the enum identifier lmao
        NEWLINE = 0,
        NUMBER = 1,
        IDENT = 2,
        STRING = 3,
        // Keywords.
        LABEL = 101,
        GOTO = 102,
        PRINT = 103,
        INPUT = 104,
        LET = 105,
        CAST = 106,
        IF = 107,
        THEN = 108,
        ENDIF = 109,
        ELIF = 110,
        ELSE = 111,
        WHILE = 112,
        REPEAT = 113,
        ENDWHILE = 114,
        FOR = 115,
        ENDFOR = 116,
        ADD_ARRAY = 117,      // "Function call" to add element to end of array
        POP_ARRAY = 118,      // "Function call" to pop element from back of array
        FUNCTION = 119,
        VOID_SPECIFIER = 120, // Specifies whether a function is of type 'void', otherwise 'auto' or 'int main' 
        ENDFUNCTION = 121,
        RETURN = 122,
        CALL = 123,
        // Operators.
        EQ = 201,       // Single Equal '=' 
        PLUS = 202,
        PLUSPLUS = 203,
        PLUSEQ = 204,
        MINUS = 205,
        MINUSMINUS = 206,
        MINUSEQ = 207, 
        ASTERISK = 208,
        SLASH = 209,
        EQEQ = 210,     // Double Equal '=='
        NOTEQ = 211,    // Not Equal '!='
        LT = 212,       // Less Than '<'
        LTEQ = 213,     // Less than or Equal To '<='
        GT = 214,       // Greater Than '>'
        GTEQ = 215,     // Greater Than or Equal To '>='
        // Logical Operators.
        OR = 301,
        AND = 302,
        NOT = 303,
        // Boolean Values.
        TRUE = 401,
        FALSE = 402,
        NONE = 403,
        // Data Types.
        INT_T = 501,
        FLOAT_T = 502,
        DOUBLE_T = 503,
        STRING_T = 504,
        BOOL_T = 505,
        AUTO_T = 506,
        ARRAY_T = 507,
        // Miscellaneous.
        COLON = 601,
        COMMA = 602,
        // Unknown Token.
        UNKNOWN = 700,  // default value of token variable in getToken() method
    };
};

struct Token
{
    std::string tokenText;  // Empty to start
    int tokenKind;          // Unknown enum value to start
};

struct Lexer
{
    std::string source;     // String containing source file contents
    size_t curPos { 0 };    // Current index position in source string 
    char curChar { ' ' } ;  // Current character found in source string

    void init_source();
    TokenType::Token isKeywordorType(std::string_view tokText);
    void nextChar();
    char peekChar();
    void abort(std::string_view message);
    void skipWhitespace();
    void skipComments();
    Token getToken();
};

#endif
