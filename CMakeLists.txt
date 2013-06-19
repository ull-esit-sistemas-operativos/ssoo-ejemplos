# CMakeLists.txt - Archivo de proyecto CMake

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ssoo-clases)

#
# Configuration global
#
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Opciones del compilador
ADD_DEFINITIONS(-O0 -g -Wall -fmessage-length=0 -std=c++0x)

#
# Construir el proyecto
#
ADD_SUBDIRECTORY(src)
