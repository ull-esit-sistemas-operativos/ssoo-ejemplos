# Sanitizadores en tiempo de ejecución.
#
# GCC y Clang soportan AddressSanitizer, UndefinedBehaviorSanitizer y LeakSanitizer a través de -fsanitize.
# MSVC solo soporta un equivalente de AddressSanitizer (/fsanitize=address, desde VS 2019 16.9);
# no existe equivalente de UndefinedBehaviorSanitizer ni de LeakSanitizer para MSVC.
if(MSVC)
    add_compile_options(/fsanitize=address)

    # /RTC1 (comprobaciones de tiempo de ejecución, activadas por defecto en configuración Debug)
    # es incompatible con /fsanitize=address.
    string(REGEX REPLACE "/RTC(su|[1csu])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE "/RTC(su|[1csu])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    add_compile_options(-fsanitize=address,undefined,leak)
    add_link_options(-fsanitize=address,undefined,leak)
endif()
