if(UNIX)
    add_executable(filelock-client filelock-client.cpp)
    add_executable(filelock-server filelock-server.c)
endif()