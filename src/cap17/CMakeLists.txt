if(UNIX)
    add_executable(mapped-files mapped-files.c)

    add_executable(mapped-files-cpp mapped-files.cpp)
    target_link_libraries(mapped-files-cpp fmt::fmt)
endif()