// mapped-files.cpp - Ejemplo en C++ del uso de CreateFileMapping() para mapear archivos
//
//  El programa mapea el archivo indicado por la línea de comandos en la memoria y calcula el el número de líneas,
//  palabras y caracteres, como hace el comando wc.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/mapped-files.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 mapped-files.cpp shlwapi.lib
//

#include <cstdint>
#include <print>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo
#include <shlwapi.h>    // Cabecera para PathFindFileName()

int protected_main(int argc, char* argv[])
{
    const char* program_name = PathFindFileNameA( argv[0] );

    if (argc != 2)
    {
        std::println( stderr, "Uso: {} <archivo>", program_name );
        return EXIT_FAILURE;
    }

    // Abrir el archivo que se quiere mapear en memoria.
    HANDLE file_handle = CreateFileA( argv[1], GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL, nullptr );
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en CreateFile()" );
    }

    // Obtener el tamaño del archivo.
    // En POSIX hay que deducirlo moviendo el puntero de lectura/escritura al final del archivo con lseek(), pero
    // la API Win32 tiene una función que lo devuelve directamente.
    LARGE_INTEGER file_size;
    if (! GetFileSizeEx( file_handle, &file_size ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en GetFileSizeEx()" );
    }

    // Crear el objeto de mapeo del archivo.
    // Este paso no tiene equivalente en POSIX, donde mmap() mapea el archivo en la memoria directamente. En Windows
    // hace falta antes un objeto del sistema que represente al archivo como una región de memoria. Al indicar un
    // tamaño máximo de 0, el objeto abarca todo el archivo.
    HANDLE mapping_handle = CreateFileMappingA( file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr );
    if (mapping_handle == nullptr)
    {
        // Ojo: al fallar, CreateFileMapping() devuelve un manejador nulo, no INVALID_HANDLE_VALUE.
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateFileMapping()" );
    }

    // Mapear el objeto de mapeo en la memoria del proceso, solo para lectura.
    // Como el tamaño es 0, se mapea el objeto entero y, por tanto, todo el archivo.
    void* mapped_file = MapViewOfFile( mapping_handle, FILE_MAP_READ, 0, 0, 0 );
    if (mapped_file == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en MapViewOfFile()" );
    }

    // Cerrar los manejadores del objeto de mapeo y del archivo, ya que no se necesitan más. La región mapeada sigue
    // siendo válida, igual que en POSIX el mapeo sobrevive al cierre del descriptor de archivo.
    CloseHandle( mapping_handle );
    CloseHandle( file_handle );

    const uint8_t* memory_region_begin = static_cast<uint8_t*>(mapped_file);
    const uint8_t* memory_region_end = memory_region_begin + file_size.QuadPart;
    size_t lines = 0, words = 0, characters = 0;
    bool space_character = true;

    // Contar líneas, palabras y caracteres
    for (const uint8_t* p = memory_region_begin; p < memory_region_end; ++p)
    {
        if (*p == '\n') lines++;
        if (*p == '\n' || *p == '\t' || *p == ' ')
        {
            if (! space_character) words++;
            space_character = true;   
        }
        else
        {
            space_character = false;
        }
        characters++;
    }

    std::println( "{: >7} {: >7} {: >7}", lines, words, characters);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    try
    {
        return protected_main(argc, argv);
    }
    catch(std::system_error& e)
    {
        std::println( stderr, "Error ({}): {}", e.code().value(), e.what() );
    }
    catch(std::exception& e)
    {
        std::println( stderr, "Error: Excepción: {}", e.what() );
    }

    return EXIT_FAILURE;
}
