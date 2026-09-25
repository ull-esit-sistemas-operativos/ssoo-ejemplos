add_portable_example(threads SOURCES threads.cpp)
add_portable_example(threads-factorial SOURCES threads-factorial.cpp)
add_portable_example(threads-cancel-factorial SOURCES threads-cancel-factorial.cpp)

if(CMAKE_USE_PTHREADS_INIT)
    add_posix_example(pthreads SOURCES posix/pthreads.cpp)
    add_posix_example(pthreads-factorial SOURCES posix/pthreads-factorial.cpp)
    add_posix_example(pthreads-cancel-factorial SOURCES posix/pthreads-cancel-factorial.cpp)
endif()

add_windows_example(beginthreadex SOURCES windows/beginthreadex.cpp)
