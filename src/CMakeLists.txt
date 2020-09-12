add_executable(fifo-client fifo-client.cpp)
add_executable(fifo-server fifo-server.cpp)
add_executable(fork fork.cpp)
add_executable(mmap mmap.cpp)
add_executable(pipe pipe.cpp)
add_executable(shared-memory shared-memory.cpp)

add_executable(shared-memory-sync shared-memory-sync.cpp)
target_link_libraries(shared-memory-sync Threads::Threads)

add_executable(softstack-c softstack.c)
add_executable(softstack-cpp softstack.cpp)

add_executable(threads threads.cpp)
target_link_libraries(threads Threads::Threads)

add_executable(threads-mutex threads-mutex.cpp)
target_link_libraries(threads-mutex Threads::Threads)

add_executable(threads-mutex-pthread threads-mutex-pthread.cpp)
target_link_libraries(threads-mutex-pthread Threads::Threads)

add_executable(threads-pthread threads-pthread.cpp)
target_link_libraries(threads-pthread Threads::Threads)

add_executable(yash yash.cpp)