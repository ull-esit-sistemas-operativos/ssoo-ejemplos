add_posix_example(fifo
    SOURCES posix/fifo.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
add_posix_example(fifo-control SOURCES posix/fifo-control.cpp)

add_win32_example(namedpipe
    SOURCES win32/namedpipe.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver-win32.cpp)
add_win32_example(namedpipe-control SOURCES win32/namedpipe-control.cpp)

add_posix_example(fork-pipe SOURCES posix/fork-pipe.cpp)
add_win32_example(createprocess-pipe SOURCES win32/createprocess-pipe.cpp)

add_posix_example(fork-redir SOURCES posix/fork-redir.cpp)
add_win32_example(createprocess-redir SOURCES win32/createprocess-redir.cpp)
