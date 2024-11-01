if(UNIX)
    add_executable(socket-control socket-control.cpp)
    add_executable(socket socket.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp)
endif()