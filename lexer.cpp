/*
To-do/notes:

- rework getToken() to use switch-case statement instead of horrendous if-else chain
- potentionally change TokenType struct to return string of character, like 'Token.PLUS' rather than 202, would require change of enum to separate logic
- potentially revamp std::string convertChartoString() to be more performant (std::string creation is expensive?)
    - aim to revamp std::string creation performance, remove as many intializations as possible hopefully
- ensure all function/struct descriptions are up-to-date!!
- start

*/


#include <string> // for std::string
#include <cstdlib> // for std::exit
#include <iostream> // for abort() error output

// Create new token including raw text of token and Enum value in TokenType::Token
struct Token
{
    std::string tokenText {}; // raw text of token
    int tokenKind; // enum value of token from TokenType::Token
};

// holds value of tokens, much like a dictionary, used for organizing tokens
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
        IF = 106,
        THEN = 107,
        ENDIF = 108,
        WHILE = 109,
        REPEAT = 110,
        ENDWHILE = 111,
        // Operators.
        EQ = 201, // Single Equal '=' 
        PLUS = 202,
        MINUS = 203,
        ASTERISK = 204,
        SLASH = 205,
        EQEQ = 206, // Double Equal '=='
        NOTEQ = 207, // Not Equal '!='
        LT = 208, // Less Than '<'
        LTEQ = 209, // Less than or Equal To '<='
        GT = 210, // Greater Than '>'
        GTEQ = 211, // Greater Than or Equal To '>='
        // Unknown Token.
        UNKNOWN = 300, // default value of token variable in getToken() method
    };
};

// Main lexer made to parse source code string and return tokens, strings, etc.
struct Lexer
{
    std::string source {}; // source string
    size_t curPos = 0; // current index in string array/source, size_t used to ensure no conversions (see post: www.reddit.com/r/cpp_questions/comments/1eg6qhl/odd_conversion_error_when_indexing_string/)
    char curChar = ' '; // current character in string

    // retrieve source code as C-style string (char [])
    void init_source()
    {
        source += '\n'; // append newline to C-style string source code to help parse last token
        nextChar();
    }

    std::string convertChartoString(char c) // used before creating new Token struct on token return in getToken() method
    {
        std::string str {c};
        return str;
    }
    
    // process next character in string of source code
    void nextChar()
    {
        if ((static_cast<int>(curPos)) >= static_cast<int>(source.length())) // reached end of source or about to exceed bounds
        {
            curChar = '\0'; // EOF reached
        }
        else
        {
            curChar = source[curPos];
            curPos+=1;
        }
    }
    
    // peek and see what next character is unless EOF (end of file) is reached
    char peekChar()
    {
        if (((static_cast<int>(curPos)) + 1) > static_cast<int>(source.length())) // reached end of source or about to exceed bounds
        {
            return '\0';
        }
        return source[curPos]; 
    }
   
    // exit on invalid token with error message
    void abort(std::string_view message, std::string_view optional) // optional is for sending extra info for exceptions/abort reason
    {
        std::cout << "Lexing error. " << message << optional;
        std::exit(1); 
    }
    
    // skip whitespace except newlines, marks end of statements/tokens
    void skipWhitespace()
    {
        while (curChar == ' ' || curChar == '\t' || curChar == '\r') // all forms of whitespace chars + carriage return
        {
            nextChar(); // skip whitespace character to avoid returning invalid tokens in getToken()
        }
    }
    
    // skip comments (duh)
    void skipComments()
    {
        if (curChar == '#')
        {
            while (curChar != '\n') // haven't reached end of comment yet
            {
                nextChar();
            }
        }
    }
    
    // determine token in source
    auto getToken()
    {
        skipWhitespace(); // to prevent getToken() from seeing whitespace and erroring out
        skipComments(); // skip comments/pound symbol to prevent errors in getToken()
        auto token = Token {"Unknown Token", TokenType::Token::UNKNOWN}; // basically undefined token, returned only if an unknown token is found

        if (curChar == '+') // PLUS token
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::PLUS}; // effectively returns the token character and the type of token
            nextChar(); // keep moving through source string
            return token;
        }
        else if (curChar == '-') // MINUS token
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::MINUS};
            nextChar(); // keep moving through source string
            return token;
        }
        else if (curChar == '/') // SLASH token
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::SLASH};
            nextChar(); // keep moving through source string
            return token;
        }
        else if (curChar == '*') // ASTERISK token
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::ASTERISK};
            nextChar(); // keep moving through source string
            return token;
        }
        else if (curChar == '\n') // NEWLINE token
        {
            auto token = Token {"NEWLINE CHARACTER", TokenType::Token::NEWLINE};
            nextChar(); // keep moving through source string
            return token;
        }
        else if (curChar == '=') // check whether token is single equal sign or double equal sign
        {
            if (peekChar() == '=')
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::EQEQ};
                // nextChar runs twice to get to next equal sign, then again to go to next token in source
                nextChar();
                nextChar();
                return token;
            }
            else
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::EQ};
                nextChar();
                return token;
            }
        }
        else if (curChar == '>') // check whether token is > or >=
        {
            if (peekChar() == '=')
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::GTEQ};
                nextChar();
                nextChar();
                return token;
            }
            else
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::GT};
                nextChar();
                return token;
            }
        }
        else if (curChar == '<') // check whether token is < or <=
        {
            if (peekChar() == '=')
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::LTEQ};
                nextChar();
                nextChar();
                return token;
            }
            else
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::LT};
                nextChar();
                return token;
            }
        }
        else if (curChar == '!') // check if token is !=, otherwise abort lexer
        {
            if (peekChar() == '=')
            {
                auto token = Token {convertChartoString(curChar), TokenType::Token::NOTEQ};
                nextChar();
                nextChar();
                return token;
            }
            else
            {
                abort("Expected Token NOTEQ or !=, got: !", std::to_string(peekChar()));
            }
        }
        else if (curChar == '\"') // Quotation mark, strings start with single quotation mark and end with anothe one
        {
            size_t startPosStr = curPos; // mark start of string to later extract
            nextChar(); // check content of string starting from here

            while (curChar != '\"')
            {
                if (curChar == '\r' || curChar == '\n' || curChar == '\t' || curChar == '\\' || curChar == '%') // prevent some special characters in string to make Cpp compilation easier
                {
                    abort("Illegal character found in string.", " No extra diagnostic info.");
                }
                nextChar(); // keep going until we reached end of string, marked by second quotation mark
            }
            std::string subStrToken {source}; // need to make copy here since Token struct takes std::string parameter, conversion possible?

            // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
            // 2nd param is to the total length of the string - 1, to discard quotation mark
            auto token = Token {std::string(subStrToken).substr(startPosStr, (curPos-startPosStr)-1), TokenType::Token::STRING}; 
            nextChar(); // skip over second quotation mark to prevent abort
            return token;
        }
        else if (curChar == '\0') // EOF token
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::ENDOFFILE}; // "no token" returned on EOF
            nextChar(); // keep moving through source string
            return token;
        }
        else // unknown token
            abort("Unknown token: ", std::to_string(curChar)); 

        return token; // solely here to satisfy g++
    }
};
