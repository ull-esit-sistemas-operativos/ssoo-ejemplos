# Funciones para declarar los ejemplos según la API del sistema operativo para la que están escritos.
#
# Cada ejemplo se declara con la función que corresponde a su API, de modo que el CMakeLists.txt de cada capítulo
# es una lista sin condiciones en la que cada línea dice por sí misma en qué sistemas se compila el ejemplo:
#
#   add_portable_example(threads SOURCES threads.cpp)            # C++ estándar: en todos los sistemas.
#   add_posix_example(fork SOURCES posix/fork.cpp)               # API POSIX: solo en sistemas UNIX.
#   add_win32_example(createprocess SOURCES win32/createprocess.cpp) # API Win32: solo en Microsoft Windows.
#
# Todas aceptan una lista opcional de bibliotecas con las que enlazar el ejemplo:
#
#   add_posix_example(mqueue SOURCES posix/mqueue.cpp LIBRARIES rt)
#
# Los ejemplos que no son para el sistema en el que se compila simplemente no se declaran, así que no generan
# ningún objetivo y el resto del CMakeLists.txt no tiene que preocuparse por ellos.

function(_ssoo_add_example nombre)
    cmake_parse_arguments(EJEMPLO "" "" "SOURCES;LIBRARIES" ${ARGN})

    if(NOT EJEMPLO_SOURCES)
        message(FATAL_ERROR "add_*_example(${nombre}): falta la lista de archivos SOURCES.")
    endif()

    add_executable(${nombre} ${EJEMPLO_SOURCES})

    if(EJEMPLO_LIBRARIES)
        target_link_libraries(${nombre} PRIVATE ${EJEMPLO_LIBRARIES})
    endif()
endfunction()

# Ejemplos escritos con la librería estándar de C++, que se compilan en cualquier sistema.
function(add_portable_example nombre)
    _ssoo_add_example(${nombre} ${ARGN})
endfunction()

# Ejemplos escritos con la API POSIX, que solo se compilan en sistemas UNIX.
function(add_posix_example nombre)
    if(UNIX)
        _ssoo_add_example(${nombre} ${ARGN})
    endif()
endfunction()

# Ejemplos escritos con la API Win32, que solo se compilan en Microsoft Windows.
function(add_win32_example nombre)
    if(WIN32)
        _ssoo_add_example(${nombre} ${ARGN})
    endif()
endfunction()
