#include <iostream> // IO
#include <cstdlib> // std::exit
#include <fstream> // open files for cpp

#include "lexer.h" // forward-declaration of lexer components
#include "parser.h" // forward-declaration of parser component

// tester functions below, DO NOT FORGET TO RUN INIT_SOURCE ON OBJECT BEFORE TESTING

// testParser() but with hardcoded file path instead of using command-line argument to supply path
void testParserNoArgs()
{
    std::string source;
    std::cout << "[INFO] Nubb++ Compiler, starting.\n";

    std::ifstream inputFile("code.nubb++"); // open described file from command-line argument

    std::string lineContent; 
    while (std::getline(inputFile, lineContent))
    {
        lineContent += '\n'; // adds newline after extracting line because getline doesn't do it for us :(
        source += lineContent; // append every line to source string to feed to lexer
    }

    Lexer lex { source }; // take source file as std::string
    lex.init_source(); // append newline to source file then pass to parser

    Parser parse { std::move(lex), Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init(); // call nextToken to initialize curToken and peekToken 
    
    /*
    Only include for debugging file content before parsing
    std::cout << source << '\n';
    */

    parse.program(); // start parsing source

    std::cout << "[INFO] Parsing complete.";
}

// output parse tree of given source file, uses command-line argument
void testParser(int argc, char **argv) // argc is ARGUMENT COUNT, num of arguments given, argv is ARGUMENT VECTOR, string of argument given
{ // must copy function into main with argc, argv parameters to work
    std::string source;
    std::cout << "[INFO] Nubb++ Compiler, starting.\n";

    if (argc != 2) // too few arguments, no source file given
    {
        std::cerr << "[ERROR] Cannot retrieve source file argument.\n";
        std::exit(1);
    }
    else // source file given
    {
        std::ifstream inputFile(argv[1]); // open described file from command-line argument

        std::string lineContent; 
        while (std::getline(inputFile, lineContent))
        {
            source += lineContent; // append every line to source string to feed to lexer
        }
    }

    Lexer lex { source }; // take source file as std::string
    lex.init_source(); // append newline to source file then pass to parser

    Parser parse { std::move(lex), Token {"Unknown Token", TokenType::Token::UNKNOWN}, Token {"Unknown Token", TokenType::Token::UNKNOWN} };
    parse.init(); // call nextToken to initialize curToken and peekToken 

    /*
    Only include for debugging file content before parsing
    std::cout << source << '\n';
    */

    parse.program(); // start parsing source

    std::cout << "[INFO] Parsing complete.";
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

int main()
{
    testParserNoArgs();

    return 0;
}
