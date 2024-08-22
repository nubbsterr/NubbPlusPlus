#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>  // std::exit
#include <iostream> // IO

#include "lexer.h"   // forward/include lexer so parser can use Lexer object and use its methods
#include "emitter.h" // forward/include emitter so parser can use Emitter object and its methods
#include <set>       // to use sets for storing defined variables, labels and goto'ed labels to prevent goto's on undefined labels

struct Parser
{
    Lexer lex;
    Emitter emit;

    Token peekToken;
    Token curToken;
    
    std::set<std::string>symbols {};        // declared variables so far
    std::set<std::string>labelsDeclared {}; // labels declared so far (so we can't goto an undefined label)
    std::set<std::string>labelsGotoed {};   // labels gotoed so far 

    void abort(std::string_view message, auto optional, std::string_view optional_, auto optional__);
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
