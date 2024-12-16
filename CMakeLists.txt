cmake_minimum_required(VERSION 3.10)
project(YourProjectName CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Main application source files
set(MAIN_SOURCES
    main.cpp
        PostFixEvaluator.cpp
        Scanner.cpp
        SourceReader.cpp
        StringParser.cpp
        DescentParser.cpp
        SymbolTable.cpp
        CodeNode.cpp
        Tools.cpp
)

# Create the main executable
add_executable(a.exe ${MAIN_SOURCES})

# Include directories
target_include_directories(a.exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set working directory for a.exe to the project root
set_target_properties(a.exe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# TestSourceReader
set(TEST_SOURCE_READER_SOURCES
    tests/SourceReader/main.cpp
    SourceReader.cpp
    StringParser.cpp
)

add_executable(TestSourceReader ${TEST_SOURCE_READER_SOURCES})
target_include_directories(TestSourceReader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set working directory for a.exe to the project root
set_target_properties(TestSourceReader PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# TestScanner
set(TEST_SCANNER_SOURCES
    tests/Scanner/main.cpp
    Scanner.cpp
    SourceReader.cpp
    StringParser.cpp
)

add_executable(TestScanner ${TEST_SCANNER_SOURCES})
target_include_directories(TestScanner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set working directory for a.exe to the project root
set_target_properties(TestScanner PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# TestExpressionConverter
set(TEST_EXPRESSION_CONVERTER_SOURCES
    tests/ShuntingYard/main.cpp       # Update with correct path if different
    PostFixEvaluator.cpp
    Scanner.cpp
    SourceReader.cpp
    StringParser.cpp
)

add_executable(TestExpressionConverter ${TEST_EXPRESSION_CONVERTER_SOURCES})
target_include_directories(TestExpressionConverter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set working directory for TestExpressionConverter to the project root
set_target_properties(TestExpressionConverter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# TestSymbolTable
set(TEST_SYMBOL_TABLE_SOURCES
    tests/SymbolTable/main.cpp           
    PostFixEvaluator.cpp
    Scanner.cpp
    SourceReader.cpp
    StringParser.cpp
    DescentParser.cpp
    SymbolTable.cpp
)

add_executable(TestSymbolTable ${TEST_SYMBOL_TABLE_SOURCES})
target_include_directories(TestSymbolTable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set working directory for TestSymbolTable to the project root
set_target_properties(TestSymbolTable PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Custom targets for assignments
add_custom_target(assignment1
    COMMAND ${CMAKE_MAKE_PROGRAM} -f ${CMAKE_CURRENT_SOURCE_DIR}/deliverables/assignment1/Makefile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deliverables/assignment1
)

add_custom_target(assignment2
    COMMAND ${CMAKE_MAKE_PROGRAM} -f ${CMAKE_CURRENT_SOURCE_DIR}/deliverables/assignment2/Makefile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deliverables/assignment2
)

# Optional: Add a custom target for cleaning
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.o
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/a.exe
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/TestSourceReader
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/TestScanner
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/TestExpressionConverter
)
