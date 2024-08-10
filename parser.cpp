#include <cstdlib> // std::exit
#include <iostream> // IO

#include "lexer.h" // forward-declaration of lexer componenets
#include "parser.h" 

// exit on fatal error in parser
void Parser::abort(std::string_view message, auto optional, std::string_view optional_, auto optional__)
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

// return true if current token is a comparison operator like <=, ==, etc. false otherwise
bool Parser::isComparisonOperator()
{
    return checkToken(TokenType::Token::GT) || checkToken(TokenType::Token::GTEQ) || checkToken(TokenType::Token::LT) || checkToken(TokenType::Token::LTEQ) || checkToken(TokenType::Token::EQEQ) || checkToken(TokenType::Token::NOTEQ);
}

// parse for newlines, nl ::= '\n'+
void Parser::nl()
{
    std::cout << "[TRACE] NEWLINE\n";

    match(TokenType::Token::NEWLINE); // ensure we have a newline with our current statement so it's valid
    while(checkToken(TokenType::Token::NEWLINE)) // can handle multiple newlines as well, i.e. empty lines after statements
    {
        nextToken();
    }
}

// parse for identifier or integral type or constant integral literal, primary ::= number | ident
void Parser::primary()
{
    std::cout << "[TRACE] PRIMARY (" << curToken.tokenText << ")\n"; // prints primary trace with text of primary

    if (checkToken(TokenType::Token::NUMBER)) // constant literal
    {
        nextToken(); // continue parsing
    }
    else if (checkToken(TokenType::Token::IDENT)) // identifier of integral type
    {
        nextToken(); // continue parsing
    }
    else // an unknown value or otherwise non-integral type, abort
    {
        abort("Unexpected token at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }
}

// parse for division/multiplcation operations, term ::= unary {( "/" | "*" ) unary}
void Parser::term()
{
    std::cout << "[TRACE] TERM\n";

    unary(); // parse for number at beginning of term expression
    // ensure we have one * or / symbol for valid term expression
    while (checkToken(TokenType::Token::ASTERISK) || checkToken(TokenType::Token::SLASH))
    {
        nextToken(); // fetch * or / symbol
        unary(); // then parse for other number in expression
    }
}

// parse for numbers or identifiers used for mathematical expression, signed positive by default or specified in code,  unary ::= ["+" | "-"] primary
void Parser::unary()
{
    std::cout << "[TRACE] UNARY\n";

    // can have + or - symbol next to integral value/number
    if (checkToken(TokenType::Token::PLUS) || checkToken(TokenType::Token::MINUS))
    {
        nextToken(); // fetch integral value/number after sign
    }
    primary();
}

// parse for expressions, expression ::= term {( "-" | "+" ) term}
void Parser::expression()
{
    std::cout << "[TRACE] EXPRESSION\n";

    term();
    // ensure we have one MINUS or PLUS symbol for valid mathematical expression
    while (checkToken(TokenType::Token::PLUS) || checkToken(TokenType::Token::MINUS))
    {
        nextToken();
        term();
    }
}

// parse for comparison/boolean statement, comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
void Parser::comparison()
{
    std::cout << "[TRACE] COMPARISON\n";

    expression(); // parse for expression in comparison
    if (isComparisonOperator()) // see if there is a valid operator for comparison
    {
        nextToken(); // parse for comparison token
        expression(); // parse for other expression
    }
    else
    {
        abort("Expected comparison at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }
}

// parse for statements, statement ::= "PRINT" (expression | string) nl | IF comparison, etc.
void Parser::statement()
{
    // evaluate current token to see what statement we're parsing
    if (checkToken(TokenType::Token::PRINT)) // "PRINT" (expression | string) nl
    {
        std::cout << "[TRACE] STATEMENT-PRINT\n";
        nextToken(); // see if expression or string is given

        if (checkToken(TokenType::Token::STRING)) // if string is given for PRINT argument
        {
            nextToken(); // continue to next statement/parse for string
        }
        else // expression given otherwise
        {
            expression();
        }
    }
    else if (checkToken(TokenType::Token::IF)) // "IF" comparison "THEN" nl {statement} "ENDIF" nl
    {
        std::cout << "[TRACE] STATEMENT-IF\n";
        nextToken(); 
        comparison(); // parse for comparison operation for if statement

        match(TokenType::Token::THEN); // match for THEN token after comparison
        nl(); // check for valid newline leading to statements after THEN keyword

        while (!(checkToken(TokenType::Token::ENDIF))) // zero or more statements in endif
        {
            statement(); // parse all statements in THEN block before ENDIF
        }
        match(TokenType::Token::ENDIF); // match for ENDIF keyword when no more statements are found in THEN block
    }
    else if (checkToken(TokenType::Token::WHILE)) // "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
    {
        std::cout << "[TRACE] STATEMENT-WHILE\n";
        nextToken();
        comparison(); // parse for comparison then match for REPEAT keyword

        match(TokenType::Token::REPEAT); // match for REPEAT keyword after comparison
        nl(); // newline after REPEAT keyword

        while (!(checkToken(TokenType::Token::ENDWHILE))) // zero or more statements in while-loop body
        {
            statement();
        }
        match(TokenType::Token::ENDWHILE); // match for ENDWHILE after all statements in while-loop body are parsed
    }
    else if (checkToken(TokenType::Token::LABEL)) // "LABEL" ident nl
    {
        std::cout << "[TRACE] STATEMENT-LABEL\n";
        nextToken();
        match(TokenType::Token::IDENT); // match for identifier after label, newline inserted at end of statement() function
    }
    else if (checkToken(TokenType::Token::GOTO)) // "GOTO" ident nl
    {
        std::cout << "[TRACE] STATEMENT-GOTO\n";
        nextToken();
        match(TokenType::Token::IDENT); // match for identifier after goto, newline inserted at end of statement() function
    }
    else if (checkToken(TokenType::Token::LET)) // "LET" ident "=" expression nl
    {
        std::cout << "[TRACE] STATEMENT-LET\n";
        nextToken();
        match(TokenType::Token::IDENT); // match for identifier after LET keyword
        match(TokenType::Token::EQ); // then match for EQ sign 

        expression(); // then parse for expression
    }
    else if (checkToken(TokenType::Token::INPUT)) // "INPUT" ident nl
    {
        std::cout << "[TRACE] STATEMENT-INPUT\n";
        nextToken();
        match(TokenType::Token::IDENT); // match for identifier after INPUT keyword
    }
    else // invalid staement occured somehow, effectively a syntax error
    {
        abort("Invalid staement at: ", curToken.tokenText, ". Token enum ", curToken.tokenKind);
    }

    nl(); // output newline
}

// parse program source, program ::= {statement}
void Parser::program()
{
    std::cout << "[TRACE] PROGRAM\n";

    // skip ALL newlines at the beginning of source file until valid token/statement/keyword is reached
    // this will let us have comments at the root of our files now
    while (checkToken(TokenType::Token::NEWLINE))
    {
        nextToken();
    }

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
