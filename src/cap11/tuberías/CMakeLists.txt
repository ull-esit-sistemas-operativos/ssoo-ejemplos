add_posix_example(fifo
    SOURCES posix/fifo.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
add_posix_example(fifo-control SOURCES posix/fifo-control.cpp)

add_windows_example(namedpipe
    SOURCES windows/namedpipe.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver-windows.cpp)
add_windows_example(namedpipe-control SOURCES windows/namedpipe-control.cpp)

add_posix_example(fork-pipe SOURCES posix/fork-pipe.cpp)
add_windows_example(createprocess-pipe SOURCES windows/createprocess-pipe.cpp)

add_posix_example(fork-redir SOURCES posix/fork-redir.cpp)
add_windows_example(createprocess-redir SOURCES windows/createprocess-redir.cpp)
