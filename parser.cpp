#include <cstdlib>  // std::exit
#include <iostream> // IO

#include "parser.h" // includes forward-declaration of Parser struct, member functions and member variables

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
    return checkToken(TokenType::Token::GT) || checkToken(TokenType::Token::GTEQ) || checkToken(TokenType::Token::LT) || checkToken(TokenType::Token::LTEQ) || checkToken(TokenType::Token::EQEQ) || checkToken(TokenType::Token::NOTEQ) || checkToken(TokenType::Token::OR) || checkToken(TokenType::Token::AND);
}

// nl ::= '\n'+
void Parser::nl()
{
    match(TokenType::Token::NEWLINE);            // ensure we have a newline with current statement
    while(checkToken(TokenType::Token::NEWLINE)) // handle newlines until all newlines are parsed
    {
        nextToken();
    }
}

// primary ::= number | ident | bool
void Parser::primary()
{
    if (checkToken(TokenType::Token::NUMBER)) // constant literal
    {
        emit.emit(curToken.tokenText);
        nextToken(); // continue parsing
    }
    else if (checkToken(TokenType::Token::TRUE))
    {
        emit.emit("true");
        nextToken();
    }
    else if (checkToken(TokenType::Token::FALSE))
    {
        emit.emit("false");
        nextToken();
    }
    else if (checkToken(TokenType::Token::NONE))
    {
        emit.emit("NULL");
        nextToken();
    }
    else if (checkToken(TokenType::Token::IDENT)) // identifier of integral type
    {
        if (!(symbols.contains(curToken.tokenText)))
        {
            abort("Referencing variable before assignment: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
        }

        emit.emit(curToken.tokenText);
        nextToken(); // continue parsing
    }
    else // an unknown value or otherwise non-integral type, abort
    {
        abort("Unexpected token at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }
}

// unary ::= ["+" | "-"] primary
void Parser::unary()
{
    // can have + or - symbol next to integral value/number
    if (checkToken(TokenType::Token::PLUS) || checkToken(TokenType::Token::MINUS))
    {
        emit.emit(curToken.tokenText);
        nextToken(); // fetch integral value/number after sign
    }

    primary();
}

// term ::= unary {( "/" | "*" ) unary}
void Parser::term()
{
    unary(); // parse for number at beginning of term expression
    // ensure we have 0 or more * or / symbol for valid term expression
    while (checkToken(TokenType::Token::ASTERISK) || checkToken(TokenType::Token::SLASH))
    {
        emit.emit(curToken.tokenText);
        nextToken(); // fetch * or / symbol
        unary(); // then parse for other number in expression
    }
}

// expression ::= term {( "-" | "+" ) term}
void Parser::expression()
{
    term();
    // ensure we have one MINUS or PLUS symbol for valid mathematical expression
    while (checkToken(TokenType::Token::PLUS) || checkToken(TokenType::Token::MINUS))
    {
        emit.emit(curToken.tokenText);
        nextToken();
        term();
    }
}

// comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
void Parser::comparison()
{
    expression(); // parse for expression in comparison
    if (isComparisonOperator()) // see if there is a valid operator for comparison
    {
        if (curToken.tokenText == "AND") // logical AND
        {
            emit.emit(" && ");
            nextToken();
            expression();
        }
        else if (curToken.tokenText == "OR") // logical OR
        {
            emit.emit(" || ");
            nextToken();
            expression();
        }
        else // parse for other expression
        {
            emit.emit(curToken.tokenText); 
            nextToken();                   
            expression();  
        }                
    }
    else
    {
        abort("Expected comparison at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }

    // more than one comparison operator: ==, <=, ...
    while (isComparisonOperator())
    {
        emit.emit(curToken.tokenText);
        nextToken();
        expression();
    }
}

// statement ::= "PRINT" (expression | string) nl | IF comparison, etc.
void Parser::statement()
{
    if (checkToken(TokenType::Token::PRINT)) // "PRINT" (expression | string) nl
    {
        nextToken();                              // see if expression or string is given
        if (checkToken(TokenType::Token::STRING)) // if string is given for PRINT argument
        {
            // normal print statement with given text
            emit.emitLine("std::cout << \"" + curToken.tokenText + "\\n" + "\";");
            nextToken();
        }
        else // expression given otherwise
        {
            emit.emit("std::cout << static_cast<float>("); // static_cast for floating-point support
            expression();
            emit.emitLine(");");                           // close expression
        }
    }
    else if (checkToken(TokenType::Token::IF)) // "IF" comparison "THEN" nl {statement} "ENDIF" nl
    {
        nextToken(); 
        emit.emit("if (");             // comparison goes inside paranthesis
        comparison();                  // parse for comparison

        match(TokenType::Token::THEN); // match for THEN token after comparison
        nl();                          // check for valid newline leading to statements after THEN keyword
        emit.emitLine(")");
        emit.emitLine("{");

        while (!(checkToken(TokenType::Token::ENDIF))) // zero or more statements in endif
        {
            statement();                // parse all statements in THEN block before ENDIF
        }
        match(TokenType::Token::ENDIF); // match for ENDIF keyword when no more statements are found in THEN block
        emit.emitLine("}");             // closing if statement block
    }
    else if (checkToken(TokenType::Token::WHILE)) // "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
    {
        nextToken();
        emit.emit("while (");
        comparison();                    // parse for comparison then match for REPEAT keyword

        match(TokenType::Token::REPEAT); // match for REPEAT keyword after comparison
        nl();                            // newline after REPEAT keyword
        emit.emitLine(")");
        emit.emitLine("{");

        while (!(checkToken(TokenType::Token::ENDWHILE))) // zero or more statements in while-loop body
        {
            statement();
        }
        match(TokenType::Token::ENDWHILE); // match for ENDWHILE after all statements in while-loop body
        emit.emitLine("}");                // closing while loop block
    }
    else if (checkToken(TokenType::Token::LABEL)) // "LABEL" ident nl
    {
        nextToken();

        if (labelsDeclared.contains(curToken.tokenText)) // ensure LABEL given doesn't exist to prevent redefiniton, otherwise add to set
        {
            abort("Label already exists: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
        }
        labelsDeclared.insert(curToken.tokenText); // add LABEL to set since there's no redefinition

        emit.emitLine(curToken.tokenText + ":"); // statement label for goto
        match(TokenType::Token::IDENT);          // match for identifier after LABEL
    }
    else if (checkToken(TokenType::Token::GOTO)) // "GOTO" ident nl
    {
        nextToken();
        labelsGotoed.insert(curToken.tokenText); // add LABEL that has been gotoed, i.e. the identifier of the LABEL
        
        emit.emitLine("goto " + curToken.tokenText + ';');
        match(TokenType::Token::IDENT);         // match for identifier after GOTO
    }
    else if (checkToken(TokenType::Token::LET)) // "LET" ident "=" expression nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText))) // if we see an undefined variable in LET statement
        {
            symbols.insert(curToken.tokenText);      // add to set
            emit.emit("auto " + curToken.tokenText + " { "); // variable declaration with auto type for type deduction

            match(TokenType::Token::IDENT); // match for identifier after LET keyword
            match(TokenType::Token::EQ);    // then match for EQ sign 

            expression(); // then parse for expression, will return variable value
            emit.emitLine(" };"); 
        }
        else
        {
            emit.emit(curToken.tokenText + " = "); // known variable, reference without auto keyword
            
            match(TokenType::Token::IDENT); // match for identifier after LET keyword
            match(TokenType::Token::EQ);    // then match for EQ sign 

            expression(); // then parse for expression, will return variable value
            emit.emitLine(";"); 
        }
    }
    else if (checkToken(TokenType::Token::INPUT)) // "INPUT" ident nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText)))
        {
            symbols.insert(curToken.tokenText);
            
            // must declare at header otherwise it'll look something like: 
            // std::cin >> float (curToken.text)
            
            /* 
             * Nubb++ has inputs as floats by default, no char or string inputs :(
             * BASIC has very goofy variable definiton rules and type deduction that
             * cannot be replicated in C++ without (i imagine) a bunch of function overloading.
             * 
             * std::cin is diagnosed for invalid input and cleared on non-integral inputs.
             */
            
            // double by default, can be circumvented by defining variable before input 
            emit.headerLine("\tfloat " + curToken.tokenText + " {};");
        }
        // to circumvent std::cin failing on invalid input 
        // we implement input validation to ever std::cin/INPUT call
        emit.emitLine("\tstd::cin >> " + curToken.tokenText + ';');

        emit.emitLine("\tif (std::cin.fail()) // invalid input given, crashes std::cin");
        emit.emitLine("\t{");
        
        emit.emitLine("\t\t" + curToken.tokenText + " = -1; // default value on extraction failure"); 
        emit.emitLine("\t\tstd::cin.clear(); // reset std::cin back to normal mode");
        emit.emitLine("\t\tstd::cin.ignore(std::numeric_limits<std::streamsize>::max(), \'\\n\'); // clear input buffer up to next newline character ");
        
        emit.emitLine("\t}");
        
        match(TokenType::Token::IDENT); // match for identifier after INPUT keyword
    }
    else // invalid staement occured somehow, effectively a syntax error
    {
        abort("Invalid staement at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }

    nl(); // output newline
}

// parse program source, program ::= {statement}
void Parser::program()
{
    std::cout << "[INFO] PROGRAM: Prepping C++ source...\n";

    // start appending basic includes and main() function to header
    emit.headerLine("// Thank you for using Nubb++ ❤️\n");
    emit.headerLine("#include <iostream>");
    emit.headerLine("#include <limits>\n"); // limits for inalid input to clear buffer and reset cin
    emit.headerLine("int main()");
    emit.headerLine("{"); // yes we put the curly brace on the next line, WE ARE NORMAL PEOPLE

    std::cout << "[INFO] PROGRAM: Finished prepping C++ source.\n";

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

    std::cout << "[INFO] PROGRAM: Finishing up C++ code file, closing main()..\n";

    emit.emitLine("\treturn 0;");
    emit.emitLine("}");

    std::cout << "[INFO] PROGRAM: main() closed. Checking for undefined LABELS...\n";

    // when parsing is finished, check for undefined variables
    for (auto itr : labelsGotoed)
    {
        if (!(labelsDeclared.contains(itr))) // if LABEL not declared but is trying to be GOTO'ed
        {
            abort("Attempting to GOTO an undefined label: ", itr, " ", " "); // empty spaces cuz nothing else to add + abort requires 4 arguments
        }
    }

    std::cout << "[INFO] PROGRAM: Parsing complete. Writing to C++ source...\n";
    emit.writeFile(emit.code, emit.header); // write emitted code to output file, see test.cpp for why we call writeFile here
}

// initalizes peekToken and curToken
void Parser::init()
{
    nextToken();
    nextToken();

    std::cout << "[INFO] PARSER: Parser initialized. Parsing starting...\n";
}
