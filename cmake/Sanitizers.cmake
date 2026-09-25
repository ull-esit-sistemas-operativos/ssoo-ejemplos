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

    # El enlazado incremental (/INCREMENTAL, activado por defecto en las configuraciones Debug y RelWithDebInfo)
    # también es incompatible con /fsanitize=address: el enlazador lo ignora y avisa con LNK4300. No basta con quitar
    # la opción, porque /DEBUG activa el enlazado incremental si no se dice lo contrario.
    foreach(config DEBUG RELWITHDEBINFO)
        foreach(type EXE SHARED MODULE)
            string(REGEX REPLACE "/INCREMENTAL(:YES)?( |$)" "/INCREMENTAL:NO\\2"
                CMAKE_${type}_LINKER_FLAGS_${config} "${CMAKE_${type}_LINKER_FLAGS_${config}}")
        endforeach()
    endforeach()
else()
    add_compile_options(-fsanitize=address,undefined,leak)
    add_link_options(-fsanitize=address,undefined,leak)
endif()
