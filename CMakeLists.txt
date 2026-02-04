cmake_minimum_required(VERSION 3.22)
project(LogicLanguage VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define la librería principal de nuestro lenguaje
add_library(logic_language INTERFACE)
target_include_directories(logic_language INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --- EJECUTABLE PRINCIPAL ---
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE logic_language)

# Configuración de flags según el compilador (MSVC vs GCC/Clang)
if(MSVC)
    target_compile_options(main PRIVATE /utf-8)
else()
    target_compile_options(main PRIVATE 
        -fextended-identifiers
        -finput-charset=UTF-8
    )
endif()

# --- TESTS ---
enable_testing()

# Agrega un ejecutable para los tests
add_executable(initial_tests tests/initial_tests.cpp)
target_link_libraries(initial_tests PRIVATE logic_language)

if(MSVC)
    target_compile_options(initial_tests PRIVATE /utf-8)
else()
    target_compile_options(initial_tests PRIVATE 
        -fextended-identifiers
        -finput-charset=UTF-8
    )
endif()

# Agrega un test de CTest
add_test(NAME initial_tests COMMAND initial_tests)