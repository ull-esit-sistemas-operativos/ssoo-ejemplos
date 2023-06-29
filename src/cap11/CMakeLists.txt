if(UNIX)
    add_executable(anom-shared-memory anom-shared-memory.cpp ../common/factorial.cpp)
    target_include_directories(anom-shared-memory PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(shared-memory-client shared-memory-client.cpp)
    target_link_libraries(shared-memory-client rt)

    add_executable(shared-memory-server shared-memory-server.c ../common/timeserver.c)
    target_link_libraries(shared-memory-server rt)
    target_include_directories(shared-memory-server PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()