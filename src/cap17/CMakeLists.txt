if(UNIX)
    add_executable(mapped-files mapped-files.cpp)
    target_link_libraries(mapped-files fmt::fmt)
endif()