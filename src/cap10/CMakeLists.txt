if(MSVC)
else()
    add_executable(fifo-client-c fifo-client.c)

    add_executable(fifo-client-cpp fifo-client.cpp)
    target_link_libraries(fifo-client-cpp fmt::fmt)

    add_executable(fifo-server fifo-server.c common.c)
    
    add_executable(pipe pipe.cpp)
    target_link_libraries(pipe fmt::fmt)
endif()