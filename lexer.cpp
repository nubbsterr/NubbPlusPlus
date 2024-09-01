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

// Used before creating new Token struct on token return in getToken() method, convert char symbols to std::string
std::string Lexer::convertChartoString(char c) 
{
    std::string str {c};
    return str;
}

// find next character in source, stop search on EOF
void Lexer::nextChar()
{
    if ((static_cast<int>(curPos)) >= static_cast<int>(source.length())) // Reached end of source or about to exceed bounds
    {
        curChar = '\0'; 
    }
    else
    {
        curChar = source[curPos];
        curPos+=1;
    }
}

// Peek for next character for multi-chaacter tokens
char Lexer::peekChar()
{
    if (((static_cast<int>(curPos)) + 1) > static_cast<int>(source.length())) // Reached end of source or about to exceed bounds
    {
        return '\0';
    }
    return source[curPos]; 
}

// Exit on fatal error in Lexing process
void Lexer::abort(std::string_view message)
{
    std::cout << "[FATAL] Lexing error. " << message << '\n';
    std::exit(1); 
}

// Skip whitespace while searching source
void Lexer::skipWhitespace()
{
    while (curChar == ' ' || curChar == '\t' || curChar == '\r') // All forms of whitespace chars + carriage return
    {
        nextChar(); 
    }
}

// Skip comments while searching source
void Lexer::skipComments()
{
    if (curChar == '#')
    {
        while (curChar != '\n') // Haven't reached end of comment yet
        {
            nextChar();
        }
    }
}

// Retrieve and return tokens in source to parser/user
Token Lexer::getToken()
{
    skipWhitespace(); 
    skipComments(); 
    auto token = Token {"Unknown Token", TokenType::Token::UNKNOWN}; 

    if (curChar == '+') // PLUS 
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"+=", TokenType::Token::PLUSEQ};
            // nextChar runs twice to get to next character, then again to go to next token in source
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
            auto token = Token {"+", TokenType::Token::PLUS}; 
            nextChar(); 
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
            auto token = Token {"-", TokenType::Token::MINUS}; 
            nextChar(); 
            return token;
        }
    }
    else if (curChar == '/') // SLASH 
    {
        auto token = Token {"/", TokenType::Token::SLASH};
        nextChar(); 
        return token;
    }
    else if (curChar == '*') // ASTERISK 
    {
        auto token = Token {"*", TokenType::Token::ASTERISK};
        nextChar();
        return token;
    }
    else if (curChar == '\n') // NEWLINE 
    {
        auto token = Token {"NEWLINE CHARACTER", TokenType::Token::NEWLINE};
        nextChar(); 
        return token;
    }
    else if (curChar == '=') // =/EQ or ==/EQEQ operators
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"==", TokenType::Token::EQEQ};
            nextChar();
            return token;
        }
        else
        {
            auto token = Token {"=", TokenType::Token::EQ};
            nextChar();
            return token;
        }
    }
    else if (curChar == '>') // >/GT or >=/GTEQ operators
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {">=", TokenType::Token::GTEQ};
            nextChar();
            return token;
        }
        else
        {
            auto token = Token {">", TokenType::Token::GT};
            nextChar();
            return token;
        }
    }
    else if (curChar == '<') // </LT or <=/LTEQ operators
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"<=", TokenType::Token::LTEQ};
            nextChar();
            return token;
        }
        else
        {
            auto token = Token {"<", TokenType::Token::LT};
            nextChar();
            return token;
        }
    }
    else if (curChar == '!') // NOTEQ/!= operator
    {
        if (peekChar() == '=')
        {
            nextChar();
            auto token = Token {"!=", TokenType::Token::NOTEQ};
            nextChar();
            return token;
        }
        else // given unexpected logical NOT, not yet supported :(
        {
            abort("Expected Token NOTEQ or !=, got: " + std::to_string(peekChar()));
        }
    }
    else if (curChar == '\"') // Parsing strings
    {
        size_t startPosStr = curPos; // Mark start of string to later extract
        nextChar();                  // Check content of string starting from here

        while (curChar != '\"') // Search string until illegal character is found or end of string is reached
        {
            if (curChar == '\r' || curChar == '\n' || curChar == '\t' || curChar == '\\' || curChar == '%') // prevent some special characters in string to make C++ compilation easier
            {
                abort("Illegal character found in string: " + std::to_string(curChar));
            }
            nextChar();
        }
        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the total length of the string - 1, to discard quotation mark
        auto token = Token {std::string(source).substr(startPosStr, (curPos-startPosStr)-1), TokenType::Token::STRING}; 
        nextChar();
        return token;
    }
    else if (std::isdigit(curChar)) // Parse numbers (int/floating-point)
    {
        size_t startPosStr = curPos - 1; // Mark start of number(s) to extract
            
        while (isdigit(peekChar()))
        {
            nextChar();
        }
        if (peekChar() == '.') // Decimal point
        {
            nextChar();

            if (!(isdigit(peekChar()))) // Non-integral element after decimal point
            {
                abort("Illegal character in number: " + std::to_string(curChar));
            }
            while ((isdigit(peekChar())))
            {
                nextChar();
            }
        }
        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the last number found
        auto token = Token {std::string(source).substr(startPosStr, (curPos-startPosStr)), TokenType::Token::NUMBER}; 
        nextChar(); 
        return token;
    }
    else if (curChar == ':') // COLON
    {
        auto token = Token {":", TokenType::Token::COLON};
        nextChar();
        return token; 
    }
    else if (isalpha(curChar)) // Parsing identifiers or keywords
    {
        size_t startPosStr = curPos - 1; // Mark start of character(s) to extract for keyword/identifier

        while (isalnum(curChar)) // Retrieve all consective alphanumeric characters, until non-alphanumeric character is reached
        {
            nextChar();
        }
        // Create substring of keyword or identifier, then check if substring is keyword or identifier
        auto subStrToken = std::string(source).substr(startPosStr, (curPos-startPosStr) - 1);
        auto keyword = Lexer::isKeywordorType(subStrToken);

        // substr starts from first parameter index then extracts until it reaches (2nd param value) length of characters, not to the index of the second parameter!
        // 2nd param is to the last character in keyword/identifier
        auto token = Token {subStrToken, keyword}; 
        return token;
    }
    else if (curChar == '\0') // EOF token
    {
        auto token = Token {"EOF CHARACTER", TokenType::Token::ENDOFFILE};
        nextChar(); 
        return token;
    }
    else // Unknown token
        abort("Unknown token: " + convertChartoString(curChar)); 

    return token; // solely here to satisfy g++, will never actually execute since abort() gets called otherwise
}
