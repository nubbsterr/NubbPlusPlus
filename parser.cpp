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

// affirm that given type for LET statement is valid, abort parsing otherwise
std::string Parser::matchType()
{
    if ((curToken.tokenText == "int") || (curToken.tokenText == "float") || (curToken.tokenText == "double") || (curToken.tokenText == "bool") || (curToken.tokenText == "auto"))
    {
        std::string tempType { curToken.tokenText }; // implemented so we can run nextToken() before exiting       
        nextToken();
        return tempType;
    }
    else if (curToken.tokenText == "string")
    {    
        nextToken();
        return "std::string";
    }
    else
    {
        abort("LET statement couldn't use type: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
    }
    
    return "auto";
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
    else if (checkToken(TokenType::Token::TRUE)) // boolean literal
    {
        emit.emit("true");
        nextToken(); // continue parsing
    }
    else if (checkToken(TokenType::Token::FALSE)) // boolean literal
    {
        emit.emit("false");
        nextToken(); // continue parsing
    }
    else if (checkToken(TokenType::Token::NONE))  // boolean literal
    {
        emit.emit("NULL");
        nextToken(); // continue parsing
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
    else if (checkToken(TokenType::Token::STRING)) // string literal
    {
        emit.emit('\"' + curToken.tokenText + '\"'); // quotation marks cuz without them we have plain text in the output
        nextToken(); // continue parsing
    }
    else // an unknown value of unknown/imaginary type, abort
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
            emit.emit("std::cout << ");
            expression();
            emit.emitLine(";");                           // close expression
        }
    }
    else if (checkToken(TokenType::Token::ELSE)) // "ELSE" nl {statement} "ENDIF" nl 
    {
        nextToken();
        emit.emitLine("else");
        emit.emitLine("{");

        nl();

        // zero or more statements before next ENDIF statement
        while (!(checkToken(TokenType::Token::ENDIF)))
        {
            statement(); // parse all statements in THEN block before next ENDIF
        }
        match(TokenType::Token::ENDIF); // match for ENDIF keyword when no more statements are found in THEN block
        emit.emitLine("}");
    }
    else if (checkToken(TokenType::Token::ELIF)) // "ELIF" comparison "THEN" nl {statement} "ENDIF" nl 
    {
        nextToken();
        emit.emit("else if ("); // comparison goes inside paranthesis
        comparison();           // parse for comparison

        match(TokenType::Token::THEN); // match for THEN token after comparison
        nl();                          // check for valid newline leading to statements after THEN keyword
        emit.emitLine(")");
        emit.emitLine("{");

        // zero or more statements before next ENDIF statement
        while (!(checkToken(TokenType::Token::ENDIF)))
        {
            statement(); // parse all statements in THEN block before ENDIF
        }
        
        match(TokenType::Token::ENDIF); // match for ENDIF keyword when no more statements are found in THEN block
        emit.emitLine("}");
    }
    else if (checkToken(TokenType::Token::IF)) // "IF" comparison "THEN" nl {statement} "ENDIF" nl
    {

        /*
        
        MASSIVE VULNERABILITIY HERE.

        Nubb++ does NOT check for a valid IF/ELIF/ELSE chain, it only will ever validate that an ENDIF is closing
        the current IF/ELIF/ELSE statement. So technically, you can have something like ELSE {statement} ENDIF with
        no beginning IF statement and have valid code in Nubb++ according to the compiler. 

        However, when compiling to machine code, g++ WILL CATCH YOU and you'll have to go back and modify your original
        Nubb++ file :)

        */

        nextToken(); 
        emit.emit("if (");             // comparison goes inside paranthesis
        comparison();                  // parse for comparison

        match(TokenType::Token::THEN); // match for THEN token after comparison
        nl();                          // check for valid newline leading to statements after THEN keyword
        emit.emitLine(")");
        emit.emitLine("{");

        // zero or more statements before next ENDIF statement
        while (!(checkToken(TokenType::Token::ENDIF)))
        {
            statement(); // parse all statements in THEN block before ENDIF
        }

        match(TokenType::Token::ENDIF); // match for ENDIF keyword when no more statements are found in THEN block
        emit.emitLine("}");
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
    else if (checkToken(TokenType::Token::LET)) // "LET" type ident "=" expression nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText))) // if we see an undefined variable in LET statement
        {
            emit.emit(matchType()); // emit type of declared variable

            symbols.insert(curToken.tokenText);          // add undefined variable to set after fetching type
            emit.emit(" " + curToken.tokenText + " { "); // variable declaration with static type for type deduction

            match(TokenType::Token::IDENT); // match for identifier after LET keyword

            /*
            
            Nubb++ does NOT yet do any sort of type-checking during parsing. So statements like 
            LET bool b = 3 CAN get through but WILL CRASH when trying to compile since this syntactically makes
            no sense and is invalid per language rules.
            
            */

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
    else if (checkToken(TokenType::Token::INPUT)) // "INPUT" (type ident | ident)  nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText)))
        {   
            // must declare at header otherwise it'll look something like: 
            // std::cin >> float (curToken.text)
            
            /* 
             * Nubb++ has requires static type after INPUT statement. If not provided,
             * parsing will abort.
             * 
             * 
             * 
             * std::cin is diagnosed for invalid input and cleared properly as well.
             */
            
            if (curToken.tokenText == "auto") // attempting to use auto type on uninitialized variable declaration
            {
                nextToken(); // nextToken() to return variable in question to user so they can debug their dumb mistake
                abort("Cannot use variable of type 'auto' in INPUT: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
            }

            emit.headerLine(matchType() + " " + curToken.tokenText + " {};"); // emit type of given variable and identifier 
            symbols.insert(curToken.tokenText); // add undefined variable to set here then,
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
        abort("Invalid statement at: ", curToken.tokenText, ". Token enum: ", curToken.tokenKind);
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
    emit.headerLine("#include <limits>"); // limits for inalid input to clear buffer and reset cin
    emit.headerLine("#include <string>\n"); // for string variable usage with static types as of Nubb++ 1.4
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
