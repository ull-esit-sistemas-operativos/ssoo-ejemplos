if(UNIX)
    add_executable(fifo fifo.cpp ../common/timeserver.cpp)
    add_executable(fifo-control fifo-control.cpp)

    add_executable(mqueue-client mqueue-client.cpp)
    target_link_libraries(mqueue-client rt)
    add_executable(mqueue-server mqueue-server.cpp ../common/timeserver.cpp)
    target_link_libraries(mqueue-server rt)

    add_executable(fork-pipe fork-pipe.cpp)

    add_executable(fork-redir fork-redir.cpp)
    add_executable(signals signals.cpp)
endif()