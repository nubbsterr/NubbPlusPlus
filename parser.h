#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct Parser
{
    Lexer lex;
    Token peekToken;
    Token curToken;

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
