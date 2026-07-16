if(UNIX)
    add_executable(mqueue-client mqueue-client.cpp)
    target_link_libraries(mqueue-client rt)
    add_executable(mqueue-server mqueue-server.cpp ../common/timeserver.cpp)
    target_link_libraries(mqueue-server rt)

    add_executable(signals signals.cpp)
endif()