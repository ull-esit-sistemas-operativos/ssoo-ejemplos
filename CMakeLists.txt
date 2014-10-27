# CMakeLists.txt - Archivo de proyecto CMake

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ssoo-clases)

#
# Configuration global
#
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Opciones del compilador
ADD_DEFINITIONS(-O0 -g -Wall -fmessage-length=0)

#
# Activar soporte de C++11
#
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    SET(CXX11_FLAGS ${CXX11_FLAGS} -std=c++11)
elseif(COMPILER_SUPPORTS_CXX0X)
    SET(CXX11_FLAGS ${CXX11_FLAGS} -std=c++0x)
else()
        message(STATUS "El compilador ${CMAKE_CXX_COMPILER} no tiene soporte de C++11. Por favor, utiliza otro compilador de C++.")
endif()

#
# Construir el proyecto
#
ADD_SUBDIRECTORY(src)
