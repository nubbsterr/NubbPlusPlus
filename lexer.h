#ifndef LEXER_H
#define LEXER_H

#include <string> // for std::string to convert chars, and for member variables of structs
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
        // Operators.
        EQ = 201,       // Single Equal '=' 
        PLUS = 202,
        MINUS = 203,
        ASTERISK = 204,
        SLASH = 205,
        EQEQ = 206,     // Double Equal '=='
        NOTEQ = 207,    // Not Equal '!='
        LT = 208,       // Less Than '<'
        LTEQ = 209,     // Less than or Equal To '<='
        GT = 210,       // Greater Than '>'
        GTEQ = 211,     // Greater Than or Equal To '>='
        // Logical Operators.
        OR = 301,
        AND = 302,
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
        // Unknown Token.
        UNKNOWN = 600,  // default value of token variable in getToken() method
    };
};

struct Token
{
    std::string tokenText;  // empty to start
    int tokenKind;          // unknown token to start
};

struct Lexer
{
    std::string source;
    size_t curPos { 0 };
    char curChar { ' ' } ;

    void init_source();
    TokenType::Token isKeywordorType(std::string_view tokText);
    std::string convertChartoString(char c);
    void nextChar();
    char peekChar();
    void abort(std::string_view message, std::string_view optional);
    void skipWhitespace();
    void skipComments();
    Token getToken();
};

#endif
