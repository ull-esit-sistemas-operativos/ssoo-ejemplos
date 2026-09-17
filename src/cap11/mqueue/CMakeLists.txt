add_posix_example(mqueue
    SOURCES posix/mqueue.cpp ${PROJECT_SOURCE_DIR}/src/common/timeserver.cpp
    LIBRARIES rt)
add_posix_example(mqueue-control SOURCES posix/mqueue-control.cpp LIBRARIES rt)
