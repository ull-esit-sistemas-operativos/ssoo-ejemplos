add_executable(threads threads.cpp)
target_link_libraries(threads fmt::fmt)
target_include_directories(threads PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_USE_PTHREADS_INIT)
    # En sistemas POSIX, es necesario enlazar el ejemplo en C++ con POSIX Threads
    target_link_libraries(threads Threads::Threads)

    add_executable(pthreads pthreads.cpp)
    target_link_libraries(pthreads fmt::fmt Threads::Threads)
    target_include_directories(pthreads PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)
endif()
