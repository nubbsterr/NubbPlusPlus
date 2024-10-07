#include "parser.h"

// Exit on fatal error in Parsing process
void Parser::abort(std::string_view message)
{
    std::cout << "[FATAL] Parsing error. " << message << '\n';
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

// affirm that given type for LET or CAST statement is valid, abort parsing otherwise
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
    else if (curToken.tokenText == "array")
    {
        nextToken();
        return "std::vector";
    }
    else
    {
        abort("LET, FOR, or CAST statement couldn't use type: " + curToken.tokenText);
    }
    
    return "auto";
}

// match tokens in statements with source file, abort if token is invalid or otherwise not present
void Parser::match(TokenType::Token tokenKind)
{
    if (!(checkToken(tokenKind)))
    { 
       abort("Expected token enum: " + std::to_string(tokenKind) + ", got: " + std::to_string(curToken.tokenKind));
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
    if (checkToken(TokenType::Token::NUMBER)) // constant integral literal
    {
        emit.emit(curToken.tokenText);
        nextToken();
    }
    else if (checkToken(TokenType::Token::TRUE)) // boolean literal
    {
        emit.emit("true");
        nextToken();
    }
    else if (checkToken(TokenType::Token::FALSE)) // boolean literal
    {
        emit.emit("false");
        nextToken();
    }
    else if (checkToken(TokenType::Token::NONE))  // boolean literal
    {
        emit.emit("NULL");
        nextToken();
    }
    else if (checkToken(TokenType::Token::IDENT)) // identifier of integral type
    {
        if (!(symbols.contains(curToken.tokenText)))
        {
            abort("Referencing variable before assignment: " + curToken.tokenText);
        }

        emit.emit(curToken.tokenText);
        nextToken(); 
    }
    else if (checkToken(TokenType::Token::STRING)) // string literal
    {
        emit.emit('\"' + curToken.tokenText + '\"'); // quotation marks cuz without them we have plain text in the output
        nextToken();
    }
    else // an unknown value of unknown/imaginary type
    {
        abort("Unexpected primary token at: " + curToken.tokenText);
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

// expression ::= term {( "-" | "+" | "-=" | "+-" ) term} | term ( "++" | "--")
void Parser::expression()
{
    term();
    // ensure we have one MINUS or PLUS symbol for valid mathematical expression
    while (checkToken(TokenType::Token::PLUS) || checkToken(TokenType::Token::MINUS) || checkToken(TokenType::Token::PLUSEQ) || checkToken(TokenType::Token::MINUSEQ))
    {
        emit.emit(curToken.tokenText);
        nextToken();
        term();
    }

    // Handle ++ or -- expressions with a single term/identifier
    if (checkToken(TokenType::Token::PLUSPLUS) || checkToken(TokenType::Token::MINUSMINUS))
    {
        emit.emit(curToken.tokenText);
        nextToken();
    }
}

// comparison ::= ["NOT"] expression {("++" | "--") | ("==" | "!=" | ">" | ">=" | "<" | "<=") expression}
// Zero or more NOT operator, 1 or more expressions total, zero or more comparison/increment/decrement operator(s)
void Parser::comparison()
{
    bool hasNOToperator = false; // used to end comparison with additional closing bracket

    if (curToken.tokenText == "NOT") // logical NOT
    {
        // emit logical not in C++ w/ bracket for following expression call
        emit.emit("!(");
        nextToken(); // go to expression
        
        hasNOToperator = true;
    }

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
        // Inspect for end to IF statement given 'IF NOT ident' type statement, otherwise abort
        if (curToken.tokenKind == TokenType::Token::THEN)
        {
            ; // do nothing and return to IF statement call
        }
        else
        {
            abort("Expected comparison at: " + curToken.tokenText);
        } 
    }

    // more than one comparison operator: ==, <=, ...
    while (isComparisonOperator())
    {
        emit.emit(curToken.tokenText);
        nextToken();
        expression();
    }

    // emit extra closing bracket when a NOT operator is used
    if (hasNOToperator)
        emit.emit(")"); 
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
            if (peekToken.tokenKind == TokenType::Token::COLON) // printing an element from an array
            {
                if (!(symbols.contains(curToken.tokenText))) // array undefined
                {
                    abort("Cannot print index content from undefined array: " + curToken.tokenText);
                }

                emit.emit(curToken.tokenText + "[");
                
                nextToken();
                nextToken();
                // skip over colon to get to index number, index number CAN EXCEED ARRAY BOUNDS, there is no checking for
                // that since doing so will require a bunch of testing and debugging :P (wouldn't be hard, just tiresome)
                expression(); 

                emit.emit("]");
            }
            else
            {
                expression();
            }
            emit.emitLine(";"); // close expression
        }
    }
    else if (checkToken(TokenType::Token::ELSE)) // "ELSE" nl {statement} "ENDIF" nl 
    {
        if (hasTrailingIf == false)
            abort("Cannot have ELSE statement without trailing IF statement.");

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

        hasTrailingIf = false; // prevent another ELIF or ELSE after current ELSE statement
    }
    else if (checkToken(TokenType::Token::ELIF)) // "ELIF" comparison "THEN" nl {statement} "ENDIF" nl 
    {
        if (hasTrailingIf == false)
            abort("Cannot have ELIF statement without trailing IF statement.");

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

        hasTrailingIf = true;
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
    else if (checkToken(TokenType::Token::FOR)) // "FOR" ident ":" comparison ":" expression "THEN" nl {statement} "ENDFOR" nl
    {
        nextToken();
        emit.emit("for (");

        /*
        
        FOR statements do NOT allow global/static variables previously defined to be used. Everything must
        be defined within the scope of the FOR statement. (Local variables is good practice anyways :P)

        e.g. 'FOR a: a <= 10: a++ THEN' will not execute since no type can be found for the identifier.
        You'd have to change it to something like 'FOR int a: a <= 10: a++' for the FOR statement to compile.

        This can be fixed by creating a new Variable struct/class system where we verify types of variables, so that previously defined
        variables can be recognized and evaluated properly (i.e. deemed legal/illegal for the FOR statement).
        
        */

        // FOR loops only support integral identifiers, no string loops :P
        // This emits the type of the iterator variable, since matchType calls nextToken, we emit here so we don't skip the type
        emit.emit(curToken.tokenText);

        if ((matchType() == "int") || (matchType() == "float") || (matchType() == "double"))
        {
            emit.emit(" " + curToken.tokenText + "; ");
        }
        else 
        {
            abort("Illegal use of type: \'" + curToken.tokenText + "\' in FOR statement.");
        }

        // temporarily add FOR statement identifier so parser can use local variable in FOR statement
        // otherwise parser will freak out since it doesn't understand local scope
        std::string localForIterator { curToken.tokenText };
        symbols.insert(localForIterator);
            
        nextToken();        // skip colon after init-statement/ident
        nextToken();        // called twice to skip over ident then colon, which will then land on comparison

        comparison();
        emit.emit(";");

        nextToken();        // skip colon after comparison/condition

        expression();
        emit.emitLine(")"); // close FOR statement after end-expression parsed

        match(TokenType::Token::THEN); 
        nl();               // match for newline when FOR statement is closed
        emit.emitLine("{");

        // parse all statements until ENDFOR found
        while (!(checkToken(TokenType::Token::ENDFOR)))
        {
            statement();
        }

        match(TokenType::Token::ENDFOR);    // match for ENDFOR after statements are parsed
        symbols.erase(localForIterator);    // erase local FOR identifier 
        emit.emitLine("}");                 // close FOR statement
    }
    else if (checkToken(TokenType::Token::LABEL)) // "LABEL" ident nl
    {
        nextToken();

        if (labelsDeclared.contains(curToken.tokenText)) // ensure LABEL given doesn't exist to prevent redefiniton, otherwise add to set
        {
            abort("Redefition of label: " + curToken.tokenText);
        }
        labelsDeclared.insert(curToken.tokenText);

        emit.emitLine(curToken.tokenText + ":"); 
        match(TokenType::Token::IDENT);          // match for identifier after LABEL
    }
    else if (checkToken(TokenType::Token::GOTO)) // "GOTO" ident nl
    {
        nextToken();
        labelsGotoed.insert(curToken.tokenText); // add LABEL identifier that has been gotoed
        
        emit.emitLine("goto " + curToken.tokenText + ';');
        match(TokenType::Token::IDENT);          // match for identifier after GOTO
    }
    else if (checkToken(TokenType::Token::LET)) // "LET" type ident "=" expression nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText))) // if we see an undefined variable in LET statement
        {
            std::string var_type { matchType() }; // save type from matchType to initialize variables properly, mainly arrays and normal integral/string variables
            
            if (var_type != "std::vector") 
            {
                emit.emit(var_type); // emit type of declared variable if NOT an array (using type deduction in C++ for std::vector, no explicit types)
                symbols.insert(curToken.tokenText);          // add undefined variable to set after fetching type
                emit.emit(" " + curToken.tokenText + " { "); // variable declaration with static type
            }
            else
            {
                symbols.insert(curToken.tokenText);          // add undefined variable to set after fetching type
                emit.emit("std::vector " + curToken.tokenText + " { "); // variable declaration with static type
            }

            match(TokenType::Token::IDENT); // match for identifier after LET keyword

            /*
            
            Nubb++ does NOT yet do any sort of type-checking during parsing. So statements like 
            LET bool b = 3 CAN get through but WILL CRASH when trying to compile since this syntactically makes
            no sense and is invalid per C++ rules.
            
            */

            match(TokenType::Token::EQ);   // then match for EQ sign 
            if (var_type == "std::vector") // handle array initialization
            {
                while (curToken.tokenKind != TokenType::Token::NEWLINE) // until a newline character is reached
                {
                    expression();                   // get variables/literals to be added into array
                    match(TokenType::Token::COMMA); // match comma after every expression
                    emit.emit(","); 
                }
            }
            else
            {
                expression(); // then parse for expression, will return variable value
            }

            emit.emitLine(" };"); 
        }
        else
        {

            /*
            
            Slight vulnerability here with arrays. You can manipulate them with an expression like 'arr = 4'
            and it will compile, however, this will obviously not run with g++. So for the time being (and unless asked),
            I'll just leave this as is.

            If you wanna add/remove stuff in arrays, do so with the ADD/POP statements.
            
            */

            emit.emit(curToken.tokenText + " = "); // known variable, reference without auto keyword
            
            match(TokenType::Token::IDENT); // match for identifier after LET keyword
            match(TokenType::Token::EQ);    // then match for EQ sign 

            expression(); // then parse for expression, will return variable value
            emit.emitLine(";"); 
        }
    }
    else if (checkToken(TokenType::Token::CAST)) // "CAST" ident ":" type
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText))) // identifier to cast isn't defined
            abort("Cannot cast undefined variable: " + curToken.tokenText);

        std::string cast_ident { curToken.tokenText }; // save cast identifier to check validity later
        
        nextToken();                                   // continue parsing since we've verified there is an identifier to cast
        match(TokenType::Token::COLON);                // match for colon before type
        
        emit.emit("static_cast<");
        std::string cast_type { matchType() };         // get type to cast identifier to

        if (cast_type == "auto") // cannot use 'auto' for type casting
            abort("Cannot cast variable to type 'auto'");

        emit.emitLine(cast_type + ">(" + cast_ident + ");"); // emit rest of CAST statement

    }
    else if (checkToken(TokenType::Token::INPUT)) // "INPUT" (type ident | ident)  nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText)))
        {   
            /* 
             * Nubb++ has requires static type after INPUT statement. If not provided,
             * parsing will abort.
             */
            
            if (curToken.tokenText == "auto") // attempting to use auto type on uninitialized variable declaration
            {
                nextToken(); // nextToken() to return variable in question to user so they can debug their dumb mistake
                abort("Cannot use variable of type 'auto' in INPUT: " + curToken.tokenText);
            }

            emit.headerLine(matchType() + " " + curToken.tokenText + " {};"); // emit input variable at header of source
            symbols.insert(curToken.tokenText);
        }
        // to circumvent std::cin failing on invalid input 
        // we implement input validation to ever std::cin/INPUT call
        emit.emitLine("\tstd::cin >> " + curToken.tokenText + ';');

        emit.emitLine("\tif (std::cin.fail()) // invalid input given, crashes std::cin");
        emit.emitLine("\t{");
        
        emit.emitLine("\t\tstd::cin.clear(); // reset std::cin back to normal mode");
        emit.emitLine("\t\tstd::cin.ignore(std::numeric_limits<std::streamsize>::max(), \'\\n\'); // clear input buffer up to next newline character ");
        
        emit.emitLine("\t}");
        
        match(TokenType::Token::IDENT); // match for identifier after INPUT keyword
    }
    else if (checkToken(TokenType::Token::ADD_ARRAY)) // "ADD" array ":" expression nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText)))
            abort("Cannot add element to undefined array: " + curToken.tokenText);

        emit.emit(curToken.tokenText + ".push_back(");

        nextToken();
        match(TokenType::Token::COLON);
        expression();

        emit.emitLine(");");
    }
    else if (checkToken(TokenType::Token::POP_ARRAY)) // "POP" array nl
    {
        nextToken();

        if (!(symbols.contains(curToken.tokenText)))
            abort("Cannot pop element from undefined array: " + curToken.tokenText);
        
        emit.emitLine(curToken.tokenText + ".pop_back();");
        nextToken();
    }
    else // invalid staement occured somehow, effectively a syntax error
    {
        abort("Invalid statement at: " + curToken.tokenText);
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
    emit.headerLine("#include <limits>");   // for invalid input to clear buffer and reset cin
    emit.headerLine("#include <string>");   // for string variable usage with static types as of Nubb++ 1.4
    emit.headerLine("#include <vector>\n"); // for array/vector usage as of Nubb++ 2.0
    emit.headerLine("int main()");
    emit.headerLine("{"); // yes we put the curly brace on the next line, WE ARE NORMAL PEOPLE

    std::cout << "[INFO] PROGRAM: Finished prepping C++ source.\n";

    // skip ALL newlines at the beginning of source file until valid token/statement/keyword is reached
    // this will let us have comments at the root of our files now
    while (checkToken(TokenType::Token::NEWLINE))
    {
        nextToken();
    }

    // parse all statements in program until EOF is reached
    while (!(checkToken(TokenType::Token::ENDOFFILE)))
    {
        statement();
    }

    std::cout << "[INFO] PROGRAM: Finishing up C++ code file, closing main()..\n";

    emit.emitLine("\treturn 0;");
    emit.emitLine("}");

    std::cout << "[INFO] PROGRAM: main() closed. Checking for undefined LABELS...\n";

    // When parsing is finished, check for undefined labels
    for (auto itr : labelsGotoed)
    {
        if (!(labelsDeclared.contains(itr)))
        {
            abort("Attempting to GOTO an undefined label: " + itr);
        }
    }

    std::cout << "[INFO] PROGRAM: Parsing complete. Pushing to Emitter...\n";
    emit.writeFile(emit.code, emit.header); // write emitted code to output file, see test.cpp for why we call writeFile here
}

// Initalizes peekToken and curToken 
void Parser::init()
{
    nextToken();
    nextToken();

    std::cout << "[INFO] PARSER: Parser initialized. Parsing starting...\n";
}
