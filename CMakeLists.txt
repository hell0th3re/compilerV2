cmake_minimum_required(VERSION 4.3)
project(compilerV2)

set(CMAKE_CXX_STANDARD 20)

add_executable(compilerV2 main.cpp
        Token.h
        Lexer.cpp
        Lexer.h
        Parser.cpp
        Parser.h
        semanticAnalyzer.cpp
        semanticAnalyzer.h
        IR.cpp
        IR.h
        CodeGenerator.cpp
        CodeGenerator.h
        Diagnostics.cpp
        Diagnostics.h)
