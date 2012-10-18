# CMakeLists.txt - Archivo de proyecto CMake

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ssoo-ejemplos)

#
# Configuration global
#
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Opciones del compilador
ADD_DEFINITIONS(-O0 -g -Wall -fmessage-length=0)

#
# Buscar Boost. Es un requisito para algunos programas
#
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0"
                              "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0"
                              "1.46.1")
FIND_PACKAGE(Boost REQUIRED COMPONENTS thread)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF (Boost_FOUND)

#
# Construir el proyecto
#
ADD_SUBDIRECTORY(src)
