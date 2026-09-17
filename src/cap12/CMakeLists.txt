add_posix_example(anom-shared-memory SOURCES posix/anom-shared-memory.cpp)

add_posix_example(shared-memory
    SOURCES posix/shared-memory.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp
    LIBRARIES rt)
add_win32_example(shared-memory
    SOURCES win32/shared-memory.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver-win32.cpp)

add_posix_example(shared-memory-control SOURCES posix/shared-memory-control.cpp LIBRARIES rt)
add_win32_example(shared-memory-control SOURCES win32/shared-memory-control.cpp)
