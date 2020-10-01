    add_executable(threads threads.cpp)
    target_link_libraries(threads Threads::Threads)

    add_executable(threads-mutex threads-mutex.cpp)
    target_link_libraries(threads-mutex Threads::Threads)

if(CMAKE_USE_PTHREADS_INIT)
    add_executable(threads-mutex-pthread threads-mutex-pthread.cpp)
    target_link_libraries(threads-mutex-pthread Threads::Threads)

    add_executable(threads-pthread threads-pthread.cpp)
    target_link_libraries(threads-pthread Threads::Threads)
endif()
