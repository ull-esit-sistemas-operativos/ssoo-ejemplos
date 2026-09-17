// file-attribs.cpp - Ejemplo de acceso a los atributos de un archivo
//
//  El programa muestra los atributos de un archivo indicado por el usuario a través de la línea de comandos.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/file-attribs.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 file-attribs.cpp
//

#include <chrono>
#include <cstdint>
#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

// Varios campos de BY_HANDLE_FILE_INFORMATION son números de 64 bits partidos en dos mitades de 32 bits, porque la
// estructura viene de una época en la que Windows era de 16 y 32 bits. Hay que recomponerlos.
uint64_t join( DWORD high, DWORD low )
{
    return (static_cast<uint64_t>(high) << 32) | low;
}

// FILETIME cuenta los intervalos de 100 nanosegundos transcurridos desde el 1 de enero de 1601, mientras que el
// time_t de POSIX cuenta los segundos transcurridos desde el 1 de enero de 1970. Entre ambas épocas hay
// 11.644.473.600 segundos, que son los que hay que descontar para pasar de una a la otra.
std::chrono::system_clock::time_point to_time_point( FILETIME filetime )
{
    const uint64_t EPOCH_DIFFERENCE = 11644473600ULL;

    uint64_t ticks = join( filetime.dwHighDateTime, filetime.dwLowDateTime );
    uint64_t seconds = ticks / 10000000ULL - EPOCH_DIFFERENCE;

    return std::chrono::system_clock::from_time_t( static_cast<time_t>(seconds) );
}

int protected_main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::println( stderr, "Uso: {} <archivo>", argv[0] );
        return EXIT_FAILURE;
    }

    // Abrir el archivo para obtener sus atributos.
    //
    // A diferencia de stat(), la API Win32 necesita un manejador del archivo para dar toda esta información, así que
    // hay que abrirlo antes. Eso sí, basta con pedir FILE_READ_ATTRIBUTES: no hace falta permiso de lectura sobre el
    // contenido, igual que stat() tampoco lo necesita.
    //
    // FILE_FLAG_BACKUP_SEMANTICS es imprescindible para que CreateFile() acepte abrir un directorio; sin esa marca
    // se niega, y este programa debe funcionar también con directorios.
    //
    // Como stat(), CreateFile() sigue los enlaces e informa del archivo al que apuntan. Para que informase del
    // enlace en sí, como hace lstat(), habría que añadir además FILE_FLAG_OPEN_REPARSE_POINT.
    HANDLE file_handle = CreateFileA( argv[1], FILE_READ_ATTRIBUTES,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                      OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr );
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en CreateFile()" );
    }

    // Obtener los atributos del archivo indicado.
    BY_HANDLE_FILE_INFORMATION file_info;
    if (! GetFileInformationByHandle( file_handle, &file_info ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en GetFileInformationByHandle()" );
    }

    CloseHandle( file_handle );

    std::println( "Atributos del archivo '{}':", argv[1] );
    // El número de serie del volumen identifica la unidad en la que está el archivo, como st_dev en POSIX.
    std::println( "  Volumen: 0x{:08x}", file_info.dwVolumeSerialNumber );
    // El índice identifica al archivo dentro de su volumen, como el número de inodo en POSIX.
    std::println( "  Índice del archivo: {}", join( file_info.nFileIndexHigh, file_info.nFileIndexLow ) );
    std::println( "  Tamaño: {}", join( file_info.nFileSizeHigh, file_info.nFileSizeLow ) );
    std::println( "  Número de enlaces: {}", file_info.nNumberOfLinks );
    std::println( "  Atributos: 0x{:08x}", file_info.dwFileAttributes );
    std::print( "    Es un ");
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        std::println( "punto de reanálisis, como un enlace simbólico" );
    }
    else if (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        std::println( "directorio" );
    }
    else
    {
        std::println( "archivo normal" );
    }

    // En POSIX el modo del archivo lleva, además del tipo, los permisos de lectura, escritura y ejecución para el
    // propietario, el grupo y el resto. La API Win32 no tiene nada de eso: solo estas marcas, que dicen cómo hay que
    // tratar el archivo, no quién puede usarlo.
    std::string flags;
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_READONLY)   flags += "solo lectura, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     flags += "oculto, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     flags += "del sistema, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)    flags += "pendiente de copia de seguridad, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)  flags += "temporal, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) flags += "comprimido, ";
    if (file_info.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)  flags += "cifrado, ";

    if (flags.empty())
    {
        flags = "ninguna";
    }
    else
    {
        flags.resize( flags.size() - 2 );   // Quitar la coma y el espacio del final.
    }

    std::println( "    Marcas: {}", flags );

    // Quién puede acceder al archivo no se consulta aquí. Windows no guarda un propietario y un grupo con sus bits
    // de permisos, como st_uid, st_gid y st_mode en POSIX, sino una lista de control de acceso (ACL) con una entrada
    // por cada usuario o grupo, que se obtiene aparte con GetSecurityInfo().

    // Windows guarda la fecha de creación del archivo, que POSIX no tiene. A cambio, no guarda la fecha en la que se
    // modificaron por última vez los atributos, que en POSIX es st_ctime.
    std::println( "  Fecha de creación: {}", to_time_point( file_info.ftCreationTime ) );
    std::println( "  Fecha de acceso: {}", to_time_point( file_info.ftLastAccessTime ) );
    std::println( "  Fecha de modificación: {}", to_time_point( file_info.ftLastWriteTime ) );

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
