add_posix_example(anom-shared-memory SOURCES posix/anom-shared-memory.cpp)

add_posix_example(shared-memory
    SOURCES posix/shared-memory.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp
    LIBRARIES rt)
add_windows_example(shared-memory
    SOURCES windows/shared-memory.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver-windows.cpp)

add_posix_example(shared-memory-control SOURCES posix/shared-memory-control.cpp LIBRARIES rt)
add_windows_example(shared-memory-control SOURCES windows/shared-memory-control.cpp)
