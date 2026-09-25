add_posix_example(socket
    SOURCES posix/socket.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
add_posix_example(socket-control SOURCES posix/socket-control.cpp)

add_windows_example(socket
    SOURCES windows/socket.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver-windows.cpp
    LIBRARIES ws2_32)
add_windows_example(socket-control SOURCES windows/socket-control.cpp LIBRARIES ws2_32)
