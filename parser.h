#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <set>

struct Parser
{
    Lexer lex;
    Token peekToken;
    Token curToken;
    
    std::set<std::string>symbols {}; // declared variables so far
    std::set<std::string>labelsDeclared {}; // labels declared so far (so we can't goto an undefined label)
    std::set<std::string>labelsGotoed {}; // labels gotoed so far 

    void abort(std::string_view message, auto optional, std::string_view optional_, auto optional__);
    void nextToken();
    auto checkToken(TokenType::Token tokenKind);
    auto checkPeek(TokenType::Token tokenKind);
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
