if(CMAKE_USE_PTHREADS_INIT)
    # Necesita POSIX Threads por llamar a pthread_self() en factorial_thread()
    add_executable(threads-sync-counter threads-sync-counter.cpp)
    target_link_libraries(threads-sync-counter fmt::fmt Threads::Threads)
    target_include_directories(threads-sync-counter PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

    add_executable(threads-sync-factorial threads-sync-factorial.cpp)
    target_link_libraries(threads-sync-factorial fmt::fmt Threads::Threads)
    target_include_directories(threads-sync-factorial PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

    add_executable(pthreads-sync-counter pthreads-sync-counter.cpp)
    target_link_libraries(pthreads-sync-counter fmt::fmt Threads::Threads)
    target_include_directories(pthreads-sync-counter PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

    add_executable(pthreads-sync-factorial pthreads-sync-factorial.cpp)
    target_link_libraries(pthreads-sync-factorial fmt::fmt Threads::Threads)
    target_include_directories(pthreads-sync-factorial PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)
endif()
