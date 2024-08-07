#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct Parser
{
    Lexer lex;
    Token peekToken;
    Token curToken;

    void abort(std::string_view message, int optional, std::string_view optional_, int optional__);
    void nextToken();
    auto checkToken(TokenType::Token tokenKind);
    auto checkPeek(TokenType::Token tokenKind);
    void match(TokenType::Token tokenKind);
    void nl();
    void expression();
    void statement();
    void program();
    void init();
};

#endif
