# add_executable(threads threads.cpp)
add_executable(threads-factorial threads-factorial.cpp)

if(CMAKE_USE_PTHREADS_INIT)
    # add_executable(pthreads pthreads.cpp)
    add_executable(pthreads-factorial pthreads-factorial.cpp)
endif()
