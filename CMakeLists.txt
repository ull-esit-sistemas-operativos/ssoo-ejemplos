cmake_minimum_required(VERSION 3.14)

project(ssoo-ejemplos LANGUAGES C CXX ASM)

# Generar los ejecutables en el directorio 'bin' del directorio de compilación
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Indicar a MSVC que los archivos de código fuente están codificados en UTF-8.
# GCC y Clang lo asumen de forma predeterminada.
if(MSVC)
    add_compile_options(/utf-8)
endif()

include(CheckPIESupported)
check_pie_supported()

# Opciones de nivel de aviso del compilador y sanitizadores en tiempo de ejecución
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

# Funciones con las que cada capítulo declara sus ejemplos según la API del sistema para la que están escritos.
include(cmake/AddExample.cmake)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/lib
)

add_subdirectory(src/cap04)
add_subdirectory(src/cap06)
add_subdirectory(src/cap09)
add_subdirectory(src/cap11/mqueue)
add_subdirectory(src/cap11/signals)
add_subdirectory(src/cap11/tuberías)
add_subdirectory(src/cap11/sockets)
add_subdirectory(src/cap12)
add_subdirectory(src/cap13)
add_subdirectory(src/cap14/mutex)
add_subdirectory(src/cap14/semáforos)
add_subdirectory(src/cap17)
add_subdirectory(src/cap19)
add_subdirectory(src/otros)
