cmake_minimum_required(VERSION 3.22)
project(LogicLanguage VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define la librería principal
add_library(logic_language INTERFACE)
target_include_directories(logic_language INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --- EJECUTABLE PRINCIPAL ---
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE logic_language)

if(MSVC)
    target_compile_options(main PRIVATE /utf-8)
else()
    target_compile_options(main PRIVATE -fextended-identifiers -finput-charset=UTF-8)
endif()

# --- TESTS ---
enable_testing()

# Helper macro para añadir tests
macro(add_logic_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE logic_language)
    
    if(MSVC)
        target_compile_options(${test_name} PRIVATE /utf-8)
    else()
        target_compile_options(${test_name} PRIVATE -fextended-identifiers -finput-charset=UTF-8)
    endif()
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# Test Fase 2 (Axiomático - Legacy adaptado)
add_logic_test(initial_tests tests/initial_tests.cpp)

# Test Fase 3 (Deducción Natural - Nuevo)
add_logic_test(natural_deduction tests/natural_deduction_tests.cpp)