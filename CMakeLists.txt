cmake_minimum_required(VERSION 3.5.0)
project(cmakeNubb++ VERSION 0.1.0 LANGUAGES C CXX)

add_executable(cmakeNubb++ src/emitter.cpp src/lexer.cpp src/nubb++.cpp src/parser.cpp src/lexer.h src/parser.h src/emitter.h)
target_compile_features(cmakeNubb++ PUBLIC cxx_std_20)
set_target_properties(cmakeNubb++ PROPERTIES OUTPUT_NAME "nubb++3.1")
