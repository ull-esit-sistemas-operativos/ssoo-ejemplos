if(UNIX)
    add_executable(file-copy file-copy.cpp)
    add_executable(file-attribs file-attribs.cpp)
    add_executable(filelock-client filelock-client.cpp)
    add_executable(filelock-server filelock-server.c)
    add_executable(dir-list dir-list.cpp)
endif()