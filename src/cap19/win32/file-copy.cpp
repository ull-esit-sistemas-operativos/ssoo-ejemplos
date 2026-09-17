// file-copy.cpp - Copia un archivo a otro
//
//  Este programa copia un archivo a otro. Si el archivo de destino ya existe, se sobreescribe.
//  Si el archivo de origen no existe, se muestra un mensaje de error.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/file-copy.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 file-copy.cpp
//

#include <cstdio>       // Cabecera para BUFSIZ
#include <print>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

int protected_main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::println( stderr, "Uso: {} <origen> <destino>", argv[0] );
        return EXIT_FAILURE;
    }

    // Abrir el archivo de origen.
    // El manejador que devuelve CreateFile() hace el papel que en POSIX hace el descriptor de archivo de open().
    // A diferencia de POSIX, hay que indicar explícitamente qué pueden hacer con el archivo los demás procesos
    // mientras lo tengamos abierto: aquí, solo leerlo.
    HANDLE source_handle = CreateFileA( argv[1], GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, nullptr );
    if (source_handle == INVALID_HANDLE_VALUE)
    {
        // Las funciones de la API Win32 no dejan el motivo del error en 'errno', como las de POSIX, sino que hay
        // que pedírselo al sistema con GetLastError(). std::system_category() sabe traducir los códigos de error
        // de cada sistema, así que el resto del programa no cambia.
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateFile() del archivo de origen" );
    }

    // Comprobar que el archivo de origen es un archivo regular.
    // GetFileType() distingue los archivos del disco de las tuberías y los dispositivos de caracteres, como hace
    // la macro S_ISREG() con el modo del archivo en POSIX. Hay que comprobarlo antes de pedir los atributos,
    // porque GetFileInformationByHandle() solo funciona con los archivos del disco.
    //
    // La comprobación no llega a saltar con los directorios, porque CreateFile() se niega a abrirlos y el programa
    // ya ha terminado antes de llegar aquí. En POSIX, en cambio, open() los abre sin problema y es esta
    // comprobación la que los detecta.
    if (GetFileType( source_handle ) != FILE_TYPE_DISK)
    {
        throw std::runtime_error( "El archivo de origen no es un archivo regular" );
    }

    // Obtener los atributos del archivo de origen.
    BY_HANDLE_FILE_INFORMATION source_info;
    if (! GetFileInformationByHandle( source_handle, &source_info ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en GetFileInformationByHandle() del archivo de origen" );
    }

    // Crear el archivo de destino.
    //  - Si el archivo de destino ya existe, se sobreescribe.
    //  - Si el archivo de destino no existe, se crea con los mismos atributos que el archivo de origen.
    HANDLE dest_handle = CreateFileA( argv[2], GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                      source_info.dwFileAttributes, nullptr );
    if (dest_handle == INVALID_HANDLE_VALUE)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateFile() del archivo de destino" );
    }

    // Copiar el contenido del archivo de origen al de destino.
    char buffer[BUFSIZ];
    DWORD bytes_read;
    while (true)
    {
        // ReadFile() avisa de los errores con su valor de retorno y señala el final del archivo devolviendo TRUE
        // y 0 bytes leídos. En POSIX ambas cosas van en el valor de retorno de read(): -1 y 0 respectivamente.
        if (! ReadFile( source_handle, buffer, static_cast<DWORD>(sizeof(buffer)), &bytes_read, nullptr ))
        {
            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                     "Fallo en ReadFile() del archivo de origen" );
        }

        if (bytes_read == 0) break;

        DWORD bytes_written;
        if (! WriteFile( dest_handle, buffer, bytes_read, &bytes_written, nullptr ))
        {
            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                     "Fallo en WriteFile() del archivo de destino" );
        }
    }

    // Cerrar los archivos.
    CloseHandle( source_handle );
    CloseHandle( dest_handle );

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
