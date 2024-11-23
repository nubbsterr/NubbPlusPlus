// Thank you for using Nubb++ ❤️
// https://github.com/nubbsterr/NubbPlusPlus | https://nubb.pythonanywhere.com
#include <iostream>  // IO
#include <cstdlib>   // std::exit
#include <fstream>   // file IO operations 
#include <chrono>    // Compile time of compilation from Nubb++ to C++

#include "lexer.h"   // forward-declaration of lexer components
#include "parser.h"  // forward-declaration of parser component
#include "emitter.h" // forward-declaration of emitter component

int main(int argc, char **argv)
{
    std::string source;
    std::cout << "[INFO] PRE-COMPILE: Nubb++ Compiler 3.1\n";
    auto startCompileTime = std::chrono::high_resolution_clock::now(); // get start time of compilation

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
            std::exit(1); 
        }
    }

    Lexer lex { source }; // take source file as std::string
    lex.init_source();    // append newline to source file then pass to parser

    Emitter emit { "out.cpp" }; // construct emitter with given filename to output as C++ code
    
    Parser parse { std::move(lex), emit, Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init();     // call nextToken to initialize curToken and peekToken 
    parse.program();  // then start parsing source, then writes emitted code by emitter to output file

    auto stopCompileTime = std::chrono::high_resolution_clock::now(); // get stop time of compilation
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopCompileTime - startCompileTime);
    
    std::cout << "[INFO] Compiling complete. [" << duration.count() << "ms]";
    return 0;
}


/*

int main() // no argv version (for testing only)
{
    std::string source;
    std::cout << "[INFO] PRE-COMPILE: Nubb++ Compiler 3.1\n";

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

*/
