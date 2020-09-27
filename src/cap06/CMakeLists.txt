if (CMAKE_ASM_COMPILER_ID STREQUAL "GNU" OR CMAKE_ASM_COMPILER_ID STREQUAL "Clang")
    add_executable(syscall syscall.s)
    target_compile_options(syscall PRIVATE)
    target_link_options(syscall PRIVATE -nostdlib -no-pie)

    add_executable(softstack-c softstack.c)
    add_executable(softstack-cpp softstack.cpp)
endif()