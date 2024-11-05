if(UNIX)
    add_executable(fifo fifo.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
    add_executable(fifo-control fifo-control.cpp)

    add_executable(fork-pipe fork-pipe.cpp)

    add_executable(fork-redir fork-redir.cpp)
endif()