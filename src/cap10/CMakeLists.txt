if(UNIX)
    add_executable(fifo-client-c fifo-client.c)
    add_executable(fifo-client-cpp fifo-client.cpp)

    add_executable(fifo-server fifo-server.c ../common/timeserver.c)
    target_include_directories(fifo-server PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(fork-pipe fork-pipe.cpp ../common/factorial.cpp)
    target_link_libraries(fork-pipe fmt::fmt)
    target_include_directories(fork-pipe PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(fork-redir fork-redir.c)
    add_executable(signals signals.c)
endif()