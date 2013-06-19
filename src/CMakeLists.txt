# CMakeLists.txt - Archivo de proyecto CMake

# Construir los programas de ejemplo
ADD_EXECUTABLE(fork fork.cpp)
ADD_EXECUTABLE(mmap mmap.cpp)
ADD_EXECUTABLE(pipe pipe.cpp)
ADD_EXECUTABLE(shared-memory shared-memory.cpp)
ADD_EXECUTABLE(shared-memory-sync shared-memory-sync.cpp)
ADD_EXECUTABLE(softstack-c softstack.cpp)
ADD_EXECUTABLE(softstack-cpp softstack.c)
ADD_EXECUTABLE(threads threads.cpp)
ADD_EXECUTABLE(threads-mutex threads-mutex.cpp)
ADD_EXECUTABLE(threads-mutex-pthread threads-mutex-pthread.cpp)
ADD_EXECUTABLE(threads-pthread threads-pthread.cpp)

TARGET_LINK_LIBRARIES(shared-memory-sync pthread)
TARGET_LINK_LIBRARIES(threads pthread)
TARGET_LINK_LIBRARIES(threads-mutex pthread)
TARGET_LINK_LIBRARIES(threads-mutex-pthread pthread)
TARGET_LINK_LIBRARIES(threads-pthread pthread)
