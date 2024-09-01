#include "emitter.h"

// Append fragment of code from Parser to std::string code
void Emitter::emit(std::string_view fragement_code) 
{
    code += fragement_code; 
}

// Append fragment of code from Parser with a newline to std::string code
void Emitter::emitLine(std::string_view fragement_code) 
{
    code += fragement_code;
    code += '\n';
}

// Append fragment of code to root of C++ file
void Emitter::headerLine(std::string_view fragment_code) 
{
    header += fragment_code;
    header += '\n'; 
}


// Write C++ code provided by Parser to output file
void Emitter::writeFile(std::string_view _code, std::string_view _header) 
{ 
    std::ofstream outFile(fullPath); // write to file of specified path given
    
    if (outFile.is_open())
    {
        std::cout << "[INFO] EMITTER: Writing to C++ File...\n";

        outFile << _header; // write headers
        outFile << _code;   // and emitted code to file
        outFile.close();    // close file

        std::cout << "[INFO] EMITTER: Writing complete.\n";
    }
    else
    {
        std::cerr << "[FATAL] EMITTER: Couldn't access file of filepath: " << fullPath << '\n';
        std::exit(1);
    }
}
