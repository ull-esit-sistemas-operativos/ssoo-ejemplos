add_posix_example(fifo
    SOURCES posix/fifo.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
add_posix_example(fifo-control SOURCES posix/fifo-control.cpp)

add_posix_example(fork-pipe SOURCES posix/fork-pipe.cpp)
add_posix_example(fork-redir SOURCES posix/fork-redir.cpp)
