cmake_minimum_required(VERSION 3.14)

project(ssoo-ejemplos LANGUAGES C CXX ASM)

# Generar los ejecutables en el directorio 'bin' del directorio de compilación
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckPIESupported)
check_pie_supported()

# Opciones de nivel de aviso del compilador
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -fmessage-length=0)
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib
)

add_subdirectory(src/cap04)
add_subdirectory(src/cap06)
add_subdirectory(src/cap09)
add_subdirectory(src/cap10)
add_subdirectory(src/cap11)
add_subdirectory(src/cap12)
add_subdirectory(src/cap13)
add_subdirectory(src/cap17)
add_subdirectory(src/cap19)
add_subdirectory(src/otros)