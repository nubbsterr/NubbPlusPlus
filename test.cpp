#include <iostream>
#include "lexer.cpp"

// tester functions below, DO NOT FORGET TO RUN INIT_SOURCE ON OBJECT BEFORE TESTING

void testGetToken() // tests Lexer token returning ability
{
    Lexer lex {"IF+-123 foo*THEN/"};
    lex.init_source(); 
    auto token = lex.getToken();

    while (token.tokenKind != TokenType::Token::ENDOFFILE)
    {
        std::cout << "Token Enum " << token.tokenKind << ": " << token.tokenText << '\n';
        token = lex.getToken(); // rinse and repeat getting token
    }
    std::cout << "Finished lexing";
}

void testCharacterReturn() // tests Lexer to return character in source code string until EOF is reached
{
    Lexer lex { "FOOBAR = 123" };
    lex.init_source();

    while (lex.peekChar() != '\0') // haven't reached null terminator/EOF
    {
        std::cout << lex.curChar << '\n'; // print current character spotted
        lex.nextChar();
    }
    std::cout << "Finished lexing";
}

int main()
{
    testGetToken();

    return 0;
}
