#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>  // std::exit
#include <iostream> // IO

#include "lexer.h"   // Forward/include lexer so parser can use Lexer object
#include "emitter.h" // Forward/include emitter so parser can use Emitter object 
#include <set>       // To use sets for storing defined variables, labels, and goto'ed labels

struct Parser
{
    Lexer lex;
    Emitter emit;

    Token peekToken;
    Token curToken;

    bool hasTrailingIf { false };           // Verifies correct IF/ELIF/ELSE structure 
    
    std::set<std::string>symbols {};        // Declared variables so far
    std::set<std::string>labelsDeclared {}; // Labels declared so far (prevent goto'ing an undefined label)
    std::set<std::string>labelsGotoed {};   // Labels gotoed so far (prevent goto'ing an undefined label)

    void abort(std::string_view message);
    void nextToken();
    auto checkToken(TokenType::Token tokenKind);
    auto checkPeek(TokenType::Token tokenKind);
    std::string matchType();
    void match(TokenType::Token tokenKind);
    bool isComparisonOperator();
    void nl();
    void primary();
    void term();
    void unary();
    void expression();
    void comparison();
    void statement();
    void program();
    void init();
};

#endif
