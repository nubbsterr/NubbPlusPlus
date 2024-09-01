#ifndef EMITTER_H
#define EMITTER_H

#include <iostream> // IO
#include <string>   // for std::string
#include <fstream>  // file IO operations

// Helper struct that works with the Parser to emit code to output file
struct Emitter
{
    std::string fullPath {}; // Contains filepath to file of outputted C++ code
    std::string header {};   // String containing content to prepend (add at rout) later in output file (like headers and variable declarartions)
    std::string code {};     // String holding all C++ code to be emitted

    void emit(std::string_view fragement_code); 
    void emitLine(std::string_view fragement_code); 
    void headerLine(std::string_view fragement_code);
    void writeFile(std::string_view _code, std::string_view _header);
};

#endif
