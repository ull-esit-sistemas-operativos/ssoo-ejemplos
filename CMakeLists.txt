cmake_minimum_required(VERSION 3.14)

project(ssoo-ejemplos LANGUAGES C CXX ASM)

# Compilar en configuración Debug si no se indica otra con -DCMAKE_BUILD_TYPE=...
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Generar los ejecutables en el directorio 'bin' del directorio de compilación
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

# En Debug, activar las comprobaciones adicionales de la librería estándar de C++ (límites en operator[], front(),
# back()...), para que los errores se detecten donde ocurren en lugar de corromper la memoria en silencio:
#  - libstdc++ (GCC y Clang en Linux): _GLIBCXX_ASSERTIONS.
#  - libc++ (Clang en macOS, LLVM >= 18): _LIBCPP_HARDENING_MODE.
#  - MSVC: no hace falta; su runtime de Debug ya define _ITERATOR_DEBUG_LEVEL=2 (iteradores comprobados).
# Cada macro se ignora en la librería que no la reconoce, así que se pueden definir ambas.
#
# No usamos opciones de hardening más agresivas, como _MSVC_STL_HARDENING o -fhardened (GCC >= 14), porque
# son más adecuadas para software en producción. Más sobre el hardening de la librería estándar:
# https://www.cppstories.com/2026/hardening-experiments/
if(NOT MSVC)
    add_compile_options(
        "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-D_GLIBCXX_ASSERTIONS>"
        "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG>"
    )
endif()

include(CheckPIESupported)
check_pie_supported()

# Opciones de nivel de aviso del compilador
include(cmake/CompilerWarnings.cmake)

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
