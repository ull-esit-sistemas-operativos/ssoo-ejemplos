add_portable_example(threads-sync-counter SOURCES threads-sync-counter.cpp)
add_portable_example(threads-sync-factorial SOURCES threads-sync-factorial.cpp)

if(CMAKE_USE_PTHREADS_INIT)
    add_posix_example(pthreads-sync-counter SOURCES posix/pthreads-sync-counter.cpp)
    add_posix_example(pthreads-sync-factorial SOURCES posix/pthreads-sync-factorial.cpp)
endif()
