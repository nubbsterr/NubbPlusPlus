#include "emitter.h"

// append fragment of code to std::string code
void Emitter::emit(std::string_view fragement_code) 
{
    code += fragement_code; 
}

// append fragment of code and newline to std::string code
void Emitter::emitLine(std::string_view fragement_code) 
{
    code += fragement_code;
    code += '\n';
}

// append fragment of code to root of C++ file -- include headers and main() function
void Emitter::headerLine(std::string_view fragment_code) 
{
    header += fragment_code;
    header += '\n'; 
}

// write C++ code to file, called in parser.cpp since Emitter member variables are not shared between structs
// takes emit.code and emit.header variables from Parser
void Emitter::writeFile(std::string_view _code, std::string_view _header) 
{ 
    std::ofstream outFile(fullPath); // write to file of specified path given
    
    if (outFile.is_open())
    {
        std::cout << "[INFO] Writing to C++ File...\n";

        outFile << _header; // write headers
        outFile << _code;   // and emitted code to file
        outFile.close();   // close file
    }
    else
    {
        std::cerr << "[FATAL] Unable to access file of filepath: " << fullPath << '\n';
        std::exit(1); // abort compilation/writing 
    }
}
