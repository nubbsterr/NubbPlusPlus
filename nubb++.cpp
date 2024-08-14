#include <iostream>  // IO
#include <cstdlib>   // std::exit
#include <fstream>   // IO operations for files

#include "lexer.h"   // forward-declaration of lexer components
#include "parser.h"  // forward-declaration of parser component
#include "emitter.h" // forward-declaration of emitter component

int main(int argc, char **argv)
{
    // must copy function into main with argc, argv parameters to work
    std::string source;
    std::cout << "[INFO] PRE-COMPILE: Nubb++ Compiler 1.1\n";

    if (argc != 2) // too few arguments, no source file given
    {
        std::cerr << "[FATAL] PRE-COMPILE: Cannot retrieve source file argument.\n";
        std::exit(1);
    }
    else // source file given
    {
        std::ifstream inputFile(argv[1]); // open described file from command-line argument
        std::string lineContent; 
        
        if (inputFile.is_open())
        {
            while (std::getline(inputFile, lineContent))
            {
                lineContent += '\n';   // adds newline after extracting line because getline doesn't do it for us :(
                source += lineContent; // append every line to source string to feed to lexer
            }

            inputFile.close(); 
        }
        else
        {
            std::cout << "[FATAL] PRE-COMPILE: Unable to access file of filepath: " << argv[1];
        }
    }

    Lexer lex { source }; // take source file as std::string
    lex.init_source();    // append newline to source file then pass to parser

    Emitter emit { "out.cpp" }; // construct emitter with given filename to output as C++ code

    /*
    
    Emitter cannot access Parser's copy of its member variables, 
    so upon calling emit.writeFile, we lose everything.

    Letting the parser do the magic for us works just fine since we can
    pass the header and code strings back to the emitter anyways, and not break anything :)
    
    */

    /* 
     
    Cannot use std::move(emit) because it will effectively pass EVERYTHING including the fullPath variable, 
    which is now inaccessible in the emitter.cpp file and only accessible in parser.cpp
    
    */

    Parser parse { std::move(lex), emit, Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init();     // call nextToken to initialize curToken and peekToken 
    parse.program();  // then start parsing source, then writes emitted code by emitter to output file

    std::cout << "[INFO] Compiling complete.";
    return 0;
}

// Below are a bunch of extra test functions for the compiler, feel free to try them out as you wish :)

/*

int main() // no argv version
{
    std::string source;
    std::cout << "[INFO] PRE-COMPILE: Nubb++ Compiler 1.1\n";

    std::ifstream inputFile("code.nubb++"); // hard-coded file path in local directory of executable
    std::string lineContent; 
    
    while (std::getline(inputFile, lineContent))
    {
        lineContent += '\n';   // adds newline after extracting line because getline doesn't do it for us :(
        source += lineContent; // append every line to source string to feed to lexer
    }

    inputFile.close(); 

    Lexer lex { source }; // take source file as std::string
    lex.init_source();    // append newline to source file then pass to parser

    Emitter emit { "out.cpp", "", "" }; // construct emitter with given filename to output as C++ code

    Parser parse { std::move(lex), emit, Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init();     // call nextToken to initialize curToken and peekToken 
    parse.program();  // then start parsing source, then writes emitted code by emitter to output file

    std::cout << "[INFO] Compiling complete.";
    return 0;
}

// test full compiler functionality using emitter to emit C++ file
void testCompileNoArgs()
{
    std::string source;
    std::cout << "[INFO] PRE-COMPILE: Nubb++ Compiler 1.1\n";

    std::ifstream inputFile("code.nubb++"); // hard-coded file path in local directory of executable
    std::string lineContent; 
    
    while (std::getline(inputFile, lineContent))
    {
        lineContent += '\n';   // adds newline after extracting line because getline doesn't do it for us :(
        source += lineContent; // append every line to source string to feed to lexer
    }

    inputFile.close(); 

    Lexer lex { source }; // take source file as std::string
    lex.init_source();    // append newline to source file then pass to parser

    Emitter emit { "out.cpp", "", "" }; // construct emitter with given filename to output as C++ code

    Parser parse { std::move(lex), emit, Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init();     // call nextToken to initialize curToken and peekToken 
    parse.program();  // then start parsing source, then writes emitted code by emitter to output file

    std::cout << "[INFO] Compiling complete.";
}

// tests Lexer token returning ability
void testGetToken() 
{
    Lexer lex {"LET a = 1 # a = 1"};
    lex.init_source(); 
    auto token = lex.getToken();

    while (token.tokenKind != TokenType::Token::ENDOFFILE)
    {
        std::cout << "[TRACE] Token Enum " << token.tokenKind << ": " << token.tokenText << '\n';
        token = lex.getToken(); // rinse and repeat getting token
    }
    std::cout << "[INFO] Finished lexing";
}

// tests Lexer to return character in source file string until EOF is reached
void testCharacterReturn()
{
    Lexer lex { "FOOBAR = 123" };
    lex.init_source();

    while (lex.peekChar() != '\0') // haven't reached null terminator/EOF
    {
        std::cout << lex.curChar << '\n'; // print current character spotted
        lex.nextChar();
    }
    std::cout << "[INFO] Finished lexing";
}

*/
