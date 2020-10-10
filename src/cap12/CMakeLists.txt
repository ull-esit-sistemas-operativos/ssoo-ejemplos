add_executable(threads-cpp threads.cpp)
target_link_libraries(threads-cpp fmt::fmt Threads::Threads)
target_include_directories(threads-cpp PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_USE_PTHREADS_INIT)
    add_executable(pthread pthread.cpp)
    target_link_libraries(pthread fmt::fmt Threads::Threads)
    target_include_directories(pthread PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)
endif()
