add_posix_example(mapped-files SOURCES posix/mapped-files.cpp)
add_win32_example(mapped-files SOURCES win32/mapped-files.cpp LIBRARIES shlwapi)
