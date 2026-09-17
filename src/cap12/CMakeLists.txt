add_posix_example(anom-shared-memory SOURCES posix/anom-shared-memory.cpp)

add_posix_example(shared-memory
    SOURCES posix/shared-memory.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp
    LIBRARIES rt)
add_posix_example(shared-memory-control SOURCES posix/shared-memory-control.cpp LIBRARIES rt)
