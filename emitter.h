#ifndef EMITTER_H
#define EMITTER_H

#include <iostream> // IO
#include <string>   // for std::string
#include <fstream>  // file IO operations

// emitter keeps track of generated code and outputs it to out.cpp
struct Emitter
{
    std::string fullPath {}; // contains filepath to file of outputted C++ code
    std::string header {};   // std::string contains anything to prepend (add at rout) later in output file (like headers)
    std::string code {};     // std::string holding all C++ code to be emitted

    void emit(std::string_view fragement_code); 
    void emitLine(std::string_view fragement_code); 
    void headerLine(std::string_view fragement_code);
    void writeFile(std::string_view _code, std::string_view _header);
};

#endif
