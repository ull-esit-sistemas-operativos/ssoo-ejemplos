if(UNIX)
    add_executable(anom-shared-memory anom-shared-memory.cpp)
    target_include_directories(anom-shared-memory PRIVATE ${CMAKE_SOURCE_DIR}/src)

    add_executable(shared-memory shared-memory.cpp ../common/timeserver.cpp)
    target_include_directories(shared-memory PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(shared-memory rt)

    add_executable(shared-memory-control shared-memory-control.cpp)
    target_link_libraries(shared-memory-control rt)
endif()