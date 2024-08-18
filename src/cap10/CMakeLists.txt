if(UNIX)
    add_executable(fifo-client-c fifo-client.c)
    add_executable(fifo-client-cpp fifo-client.cpp)
    add_executable(fifo-server fifo-server.c ../common/timeserver.c)

    add_executable(mqueue-client mqueue-client.cpp)
    target_link_libraries(mqueue-client rt)
    add_executable(mqueue-server mqueue-server.cpp ../common/timeserver.c)
    target_link_libraries(mqueue-server rt)

    add_executable(socket-client socket-client.cpp)
    add_executable(socket-server socket-server.cpp ../common/timeserver.c)

    add_executable(fork-pipe fork-pipe.cpp)

    add_executable(fork-redir fork-redir.c)
    add_executable(signals signals.c)
endif()