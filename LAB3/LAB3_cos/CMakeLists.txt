# CMake version
# (3.0 din laborator da avertisment in CMake 3.29 si eroare in CMake 4.x, de aceea folosim 3.10)
cmake_minimum_required(VERSION 3.10)

# Project name
project(tema3_cos LANGUAGES CXX)

# Specify the required C++ standard (move constructor-ul exista de la C++11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Activam avertismentele compilatorului
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Biblioteca statica: clasele temei + demonstratiile
set(lib_files
    inc/tema/contor_heap.hpp
    inc/tema/cos.hpp
    inc/tema/exemple_gresite.hpp
    inc/tema/demo.hpp
    src/contor_heap.cpp
    src/cos.cpp
    src/exemple_gresite.cpp
    src/demo.cpp)

add_library(tema_lib STATIC ${lib_files})

# PUBLIC -> inc/ e folosit si la compilarea bibliotecii, si de cine o foloseste (executabilul)
target_include_directories(tema_lib
    PRIVATE src
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>)

# Executabilul contine doar main.cpp si se leaga de biblioteca
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE tema_lib)
