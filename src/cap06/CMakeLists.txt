add_posix_example(softstack-c SOURCES posix/softstack.c)

# La versión en C++ solo usa la librería estándar, así que se compila en cualquier sistema.
add_portable_example(softstack-cpp SOURCES softstack.cpp)

# El ejemplo en ensamblador está escrito para el ensamblador de GNU.
if(CMAKE_ASM_COMPILER_ID STREQUAL "GNU" OR CMAKE_ASM_COMPILER_ID STREQUAL "Clang")
    add_posix_example(syscall SOURCES posix/syscall.s)
    if(TARGET syscall)
        target_link_options(syscall PRIVATE -nostdlib -no-pie)
    endif()
endif()

# Los ejemplos de este capítulo se compilan sin código independiente de la posición, para que las direcciones que
# muestran strace y ltrace sean fáciles de relacionar con el código.
foreach(objetivo softstack-c softstack-cpp syscall)
    if(TARGET ${objetivo})
        set_property(TARGET ${objetivo} PROPERTY POSITION_INDEPENDENT_CODE FALSE)
    endif()
endforeach()
