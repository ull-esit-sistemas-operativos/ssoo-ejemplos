add_executable(threads-sync-counter threads-sync-counter.cpp)
add_executable(threads-sync-factorial threads-sync-factorial.cpp)
add_executable(threads-sync-semaphore threads-sync-semaphore.cpp)

if(CMAKE_USE_PTHREADS_INIT)
    add_executable(pthreads-sync-counter pthreads-sync-counter.cpp)
    add_executable(pthreads-sync-factorial pthreads-sync-factorial.cpp)
endif()
