if(MSVC)
else()
    add_executable(fifo-client-c fifo-client.c)

    add_executable(fifo-client-cpp fifo-client.cpp)
    target_link_libraries(fifo-client-cpp fmt::fmt)

    add_executable(fifo-server fifo-server.c ../common/timeserver.c)
    target_include_directories(fifo-server PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(pipe pipe.cpp ../common/factorial.cpp)
    target_link_libraries(pipe fmt::fmt)
    target_include_directories(pipe PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(redir redir.c)
    add_executable(signals signals.c)
endif()