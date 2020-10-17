add_executable(threads-sync-cpp threads-sync.cpp)
target_link_libraries(threads-sync-cpp fmt::fmt Threads::Threads)
target_include_directories(threads-sync-cpp PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_USE_PTHREADS_INIT)
    add_executable(pthreads-sync pthreads-sync.cpp)
    target_link_libraries(pthreads-sync fmt::fmt Threads::Threads)
    target_include_directories(pthreads-sync PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)
endif()
