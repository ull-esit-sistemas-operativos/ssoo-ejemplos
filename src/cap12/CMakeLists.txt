# add_executable(threads threads.cpp)
# target_link_libraries(threads fmt::fmt)
# target_include_directories(threads PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

add_executable(threads-factorial threads-factorial.cpp)
target_link_libraries(threads-factorial fmt::fmt)
target_include_directories(threads-factorial PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_USE_PTHREADS_INIT)
    # add_executable(pthreads pthreads.cpp)
    # target_link_libraries(pthreads fmt::fmt)
    # target_include_directories(pthreads PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)

    add_executable(pthreads-factorial pthreads-factorial.cpp)
    target_link_libraries(pthreads-factorial fmt::fmt)
    target_include_directories(pthreads-factorial PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib)
endif()
