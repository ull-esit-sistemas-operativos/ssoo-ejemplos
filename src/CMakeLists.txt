# CMakeLists.txt - Archivo de proyecto CMake

# Construir los programas de ejemplo
ADD_EXECUTABLE(fifo-client fifo-client.cpp)
SET_TARGET_PROPERTIES(fifo-client PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(fifo-server fifo-server.cpp)
SET_TARGET_PROPERTIES(fifo-server PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(fork fork.cpp)
SET_TARGET_PROPERTIES(fork PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(mmap mmap.cpp)
SET_TARGET_PROPERTIES(mmap PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(pipe pipe.cpp)
SET_TARGET_PROPERTIES(pipe PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(shared-memory shared-memory.cpp)
SET_TARGET_PROPERTIES(shared-memory PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(shared-memory-sync shared-memory-sync.cpp)
SET_TARGET_PROPERTIES(shared-memory-sync PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")
TARGET_LINK_LIBRARIES(shared-memory-sync pthread)

ADD_EXECUTABLE(softstack-c softstack.c)

ADD_EXECUTABLE(softstack-cpp softstack.cpp)
SET_TARGET_PROPERTIES(softstack-cpp PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")

ADD_EXECUTABLE(threads threads.cpp)
SET_TARGET_PROPERTIES(threads PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")
TARGET_LINK_LIBRARIES(threads pthread)

ADD_EXECUTABLE(threads-mutex threads-mutex.cpp)
SET_TARGET_PROPERTIES(threads-mutex PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")
TARGET_LINK_LIBRARIES(threads-mutex pthread)

ADD_EXECUTABLE(threads-mutex-pthread threads-mutex-pthread.cpp)
SET_TARGET_PROPERTIES(threads-mutex-pthread PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")
TARGET_LINK_LIBRARIES(threads-mutex-pthread pthread)

ADD_EXECUTABLE(threads-pthread threads-pthread.cpp)
SET_TARGET_PROPERTIES(threads-pthread PROPERTIES COMPILE_FLAGS "-pthread ${CXX11_FLAGS}")
TARGET_LINK_LIBRARIES(threads-pthread pthread)
