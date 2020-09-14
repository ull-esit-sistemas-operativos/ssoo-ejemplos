cmake_minimum_required(VERSION 3.11)

project(ssoo-ejemplos LANGUAGES C CXX ASM)

# Generar los ejecutables en el directorio 'Debug' del directorio de compilación
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de nivel de aviso del compilador
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -fmessage-length=0)
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_subdirectory(src)
add_subdirectory(src/cap06)
add_subdirectory(src/cap09)
