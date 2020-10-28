if(UNIX)
    add_executable(filelock-client filelock-client.cpp)
    target_link_libraries(filelock-client fmt::fmt)

    add_executable(filelock-server filelock-server.c)
    target_link_libraries(filelock-server)
endif()