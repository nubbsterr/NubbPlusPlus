#include <cstdlib> // std::exit
#include <iostream> // IO

#include "lexer.h" // forward-declaration of lexer componenets
#include "parser.h" 

// exit on fatal error in parser
void Parser::abort(std::string_view message, int optional, std::string_view optional_, int optional__)
{
    std::cout << "[FATAL] Parsing error. " << message << optional << optional_ << optional__ << '\n';
    std::exit(1); 
}

// fetch next token and peek for next token in source
void Parser::nextToken()
{
    curToken = peekToken;
    peekToken = lex.getToken();
}

// verify token match for valid statements
auto Parser::checkToken(TokenType::Token tokenKind)
{
    return tokenKind == curToken.tokenKind;
}

// ensure following token matches for valid statements
auto Parser::checkPeek(TokenType::Token tokenKind)
{
    return tokenKind == peekToken.tokenKind;
}

// match tokens for valid statements
void Parser::match(TokenType::Token tokenKind)
{
    if (!(checkToken(tokenKind)))
    { 
       abort("Expected token enum ", tokenKind, ", got ", curToken.tokenKind);
    }
    nextToken();
}

// parse for newlines
void Parser::nl()
{
    std::cout << "[TRACE] NEWLINE\n";

    match(TokenType::Token::NEWLINE); // ensure we have a newline with our current statement so it's valid
    while (checkToken(TokenType::Token::NEWLINE)) // can handle multiple newlines as well, i.e. empty lines after statements
    {
        nextToken();
    }
}

// parse for expressions
void Parser::expression()
{
    std::cout << "[TRACE] EXPRESSION\n";
}

// parse for statements
void Parser::statement()
{
    // evaluate current token to see what statement we're parsing
    if (checkToken(TokenType::Token::PRINT))
    {
        std::cout << "[TRACE] STATEMENT-PRINT\n";
        nextToken();

        if (checkToken(TokenType::Token::STRING)) // if string is given for PRINT argument
        {
            nextToken();
        }
        else // expression given otherwise
        {
            expression();
        }
    }

    nl(); // output newline
}

// parse program source
void Parser::program()
{
    std::cout << "[TRACE] PROGRAM\n";

    // parse all statements in program until EOF is reached in source file
    while (!(checkToken(TokenType::Token::ENDOFFILE)))
    {
        statement();
    }
}

// initalizes peekToken and curToken
void Parser::init()
{
    nextToken();
    nextToken();
}
