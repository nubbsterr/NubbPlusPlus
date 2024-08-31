#include "lexer.h"

// verify if string in source is identifier, keyword, or type
TokenType::Token Lexer::isKeywordorType(std::string_view tokText)
{
    if (tokText == "LABEL")
        return TokenType::Token::LABEL;
    else if (tokText == "GOTO")
        return TokenType::Token::GOTO;
    else if (tokText == "PRINT")
        return TokenType::Token::PRINT;
    else if (tokText == "INPUT")
        return TokenType::Token::INPUT;
    else if (tokText == "LET")
        return TokenType::Token::LET;
    else if (tokText == "CAST")
        return TokenType::Token::CAST;
    else if (tokText == "IF")
        return TokenType::Token::IF;
    else if (tokText == "THEN")
        return TokenType::Token::THEN;
    else if (tokText == "ENDIF")
        return TokenType::Token::ENDIF;
    else if (tokText == "ELIF")
        return TokenType::Token::ELIF;
    else if (tokText == "ELSE")
        return TokenType::Token::ELSE;
    else if (tokText == "WHILE")
        return TokenType::Token::WHILE;
    else if (tokText == "REPEAT")
        return TokenType::Token::REPEAT;
    else if (tokText == "ENDWHILE")
        return TokenType::Token::ENDWHILE;
    else if (tokText == "FOR")
        return TokenType::Token::FOR;
    else if (tokText == "ENDFOR")
        return TokenType::Token::ENDFOR;
    else if (tokText == "OR")
        return TokenType::Token::OR;
    else if (tokText == "AND")
        return TokenType::Token::AND;
    else if (tokText == "False")
        return TokenType::Token::FALSE;
    else if (tokText == "True")
        return TokenType::Token::TRUE;
    else if (tokText == "None")
        return TokenType::Token::NONE;
    else if (tokText == "int")
        return TokenType::Token::INT_T;
    else if (tokText == "float")
        return TokenType::Token::FLOAT_T;
    else if (tokText == "double")
        return TokenType::Token::DOUBLE_T;
    else if (tokText == "string")
        return TokenType::Token::STRING_T;
    else if (tokText == "bool")
        return TokenType::Token::BOOL_T;
    else if (tokText == "auto")
        return TokenType::Token::AUTO_T;
    else
        return TokenType::Token::IDENT; // no keywords match, return identifier token enum
}

// append newline to source string to help parse last token, then start searching source
void Lexer::init_source()
{
    source += '\n';
    nextChar();

    std::cout << "[INFO] LEXER: Source initialized.\n";
}

// used before creating new Token struct on token return in getToken() method, convert char symbols to std::string
std::string Lexer::convertChartoString(char c) 
{
    std::string str {c};
    return str;
}

// find next character in source, stop search on EOF
void Lexer::nextChar()
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

// peek for next character for multi-symbol tokens like >=, <=, etc.
char Lexer::peekChar()
{
    if (((static_cast<int>(curPos)) + 1) > static_cast<int>(source.length())) // reached end of source or about to exceed bounds
    {
        return '\0';
    }
    return source[curPos]; 
}

// exit on fatal error in lexer
void Lexer::abort(std::string_view message, std::string_view optional)
{
    std::cout << "[FATAL] Lexing error. " << message << optional << '\n';
    std::exit(1); 
}

// skip whitespace while searching source
void Lexer::skipWhitespace()
{
    while (curChar == ' ' || curChar == '\t' || curChar == '\r') // all forms of whitespace chars + carriage return
    {
        nextChar(); // skip whitespace character to avoid returning invalid tokens in getToken()
    }
}

// skip comments while searching source
void Lexer::skipComments()
{
    if (curChar == '#')
    {
        while (curChar != '\n') // haven't reached end of comment yet
        {
            nextChar();
        }
    }
}

// retrieve and return tokens in source to parser/user
Token Lexer::getToken()
{
    skipWhitespace(); 
    skipComments(); 
    auto token = Token {"Unknown Token", TokenType::Token::UNKNOWN}; // dummy token made to satisfy compiler

    if (curChar == '+') // PLUS token
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"+=", TokenType::Token::PLUSEQ};
            nextChar();
            return token;
        }
        else if (peekChar() == '+')
        {
            nextChar();
            auto token = Token {"++", TokenType::Token::PLUSPLUS};
            nextChar();
            return token;
        }
        else
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::PLUS}; // effectively returns the token character and the type of token
            nextChar(); // keep moving through source string
            return token;
        }
    }
    else if (curChar == '-') // MINUS token
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"-=", TokenType::Token::MINUSEQ};
            nextChar();
            return token;
        }
        else if (peekChar() == '-')
        {
            nextChar();
            auto token = Token {"--", TokenType::Token::MINUSMINUS};
            nextChar();
            return token;
        }
        else
        {
            auto token = Token {convertChartoString(curChar), TokenType::Token::MINUS}; // effectively returns the token character and the type of token
            nextChar(); // keep moving through source string
            return token;
        }
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
            nextChar();
            auto token = Token {"==", TokenType::Token::EQEQ};
            // nextChar runs twice to get to next equal sign, then again to go to next token in source
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
            nextChar();
            auto token = Token {">=", TokenType::Token::GTEQ};
            // nextChar runs twice to get to next equal sign, then again to go to next token in source
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
            nextChar();
            auto token = Token {"<=", TokenType::Token::LTEQ};
            // nextChar runs twice to get to next equal sign, then again to go to next token in source
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
            nextChar();
            auto token = Token {"!=", TokenType::Token::NOTEQ};
            // nextChar runs twice to get to next equal sign, then again to go to next token in source
            nextChar();
            return token;
        }
        else // given unexpected logical NOT, not yet supported :(
        {
            abort("Expected Token NOTEQ or !=, got: ", std::to_string(peekChar()));
        }
    }
    else if (curChar == '\"') // Quotation mark, strings start with single quotation mark and end with anothe one
    {
        size_t startPosStr = curPos; // mark start of string to later extract
        nextChar();                  // check content of string starting from here

        while (curChar != '\"')
        {
            if (curChar == '\r' || curChar == '\n' || curChar == '\t' || curChar == '\\' || curChar == '%') // prevent some special characters in string to make C++ compilation easier
            {
                abort("Illegal character found in string.", " No extra diagnostic info.");
            }
            nextChar(); // keep going until we reached end of string, marked by second quotation mark
        }
        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the total length of the string - 1, to discard quotation mark
        auto token = Token {std::string(source).substr(startPosStr, (curPos-startPosStr)-1), TokenType::Token::STRING}; 
        nextChar(); // skip over second quotation mark to prevent abort
        return token;
    }
    else if (std::isdigit(curChar)) // isdigit function takes int parameter, implicit conversion
    {
        size_t startPosStr = curPos - 1; // mark start of number(s) to extract
            
        while (isdigit(peekChar()))
        {
            nextChar();
        }
        if (peekChar() == '.') // decimal point
        {
            nextChar();

            if (!(isdigit(peekChar()))) // not integral element after decimal point, WILL NOT CATCH NON-INTEGRAL ELEMENT AFTER 1ST NUMBER, unknown token will be returned instead on else statement below
            {
                abort("Illegal character in number: ", std::to_string(curChar)); // send additional diagnostic info
            }
            while ((isdigit(peekChar())))
            {
                nextChar();
            }
        }
        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the last number in token
        auto token = Token {std::string(source).substr(startPosStr, (curPos-startPosStr)), TokenType::Token::NUMBER}; 
        nextChar(); // continue to next character to avoid checking another number and aborting
        return token;
    }
    else if (curChar == ':')
    {
        auto token = Token {convertChartoString(curChar), TokenType::Token::COLON};
        nextChar();
        return token; 
    }
    else if (isalpha(curChar)) // next string of character might be an identifier or keyword
    {
        size_t startPosStr = curPos - 1; // mark start of character(s) to extract for keyword/identifier

        while (isalnum(curChar)) // retrieve all consective alphanumeric characters, until non-alphanumeric character is reached (e.g. whitespace)
        {
            nextChar();
        }
        // create substring of keyword or identifier, then check if substring is keyword or identifier
        auto subStrToken = std::string(source).substr(startPosStr, (curPos-startPosStr) - 1);
        auto keyword = Lexer::isKeywordorType(subStrToken);

        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the last character in keyword/identifier
        auto token = Token {subStrToken, keyword}; 
        return token;
    }
    else if (curChar == '\0') // EOF token
    {
        auto token = Token {"EOF CHARACTER", TokenType::Token::ENDOFFILE}; // "no token" returned on EOF
        nextChar(); // keep moving through source string
        return token;
    }
    else // unknown token
        abort("Unknown token: ", convertChartoString(curChar)); // send additional diagnostic info

    return token; // solely here to satisfy g++, will never actually execute since abort() gets called otherwise
}
