if(UNIX)
    add_executable(led-test led-test.cpp)
    target_link_libraries(led-test fmt::fmt rt)
endif()