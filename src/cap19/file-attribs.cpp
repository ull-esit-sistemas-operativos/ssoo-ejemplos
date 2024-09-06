// file-attribs.cpp - Ejemplo de acceso a los atributos de un archivo
//
// El programa muestra los atributos de un archivo indicado por el usuario a través de la línea de comandos.
//
//  Compilar:
//
//      g++ -o file-attribs file-attribs.cpp
//

#include <chrono>
#include <print>
#include <system_error>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int protected_main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::println( stderr, "Uso: {} <archivo>", argv[0] );
        return EXIT_FAILURE;
    }

    // Obtener los atributos del archivo indicado.
    struct stat file_stat;
    if (stat( argv[1], &file_stat ) == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en stat()" );
    }

    std::println( "Atributos del archivo '{}':", argv[1] );
    std::println( "  Dispositivo: {}", file_stat.st_dev );
    std::println( "  Número de inodo: {}", file_stat.st_ino );
    std::println( "  Tamaño: {}", file_stat.st_size );
    std::println( "  Tamaño en bloques de 512 bytes: {}", file_stat.st_blocks );
    std::println( "  Tamaño de E/S recomendado: {}", file_stat.st_blksize );
    std::println( "  Número de enlaces: {}", file_stat.st_nlink );
    std::println( "  Tipo de archivo: 0{:o}", file_stat.st_mode );
    std::print( "    Es un ");
    switch (file_stat.st_mode & S_IFMT)
    {
        case S_IFREG: std::println( "archivo normal" ); break;
        case S_IFDIR: std::println( "directorio" ); break;
        case S_IFCHR: std::println( "dispositivo de caracteres" ); break;
        case S_IFBLK: std::println( "dispositivo de bloques" ); break;
        case S_IFIFO: std::println( "FIFO" ); break;
        case S_IFLNK: std::println( "enlace simbólico" ); break;
        case S_IFSOCK: std::println( "socket" ); break;
        default: std::println( "tipo desconocido" ); break;
    }
    std::println( "    Permisos: 0{:o}", file_stat.st_mode & ~S_IFMT );
    std::println( "  Propietario: {}", file_stat.st_uid );
    std::println( "  Grupo: {}", file_stat.st_gid );
    std::println( "  Fecha de acceso: {}", std::chrono::system_clock::from_time_t(file_stat.st_atime) );
    std::println( "  Fecha de modificación: {}", std::chrono::system_clock::from_time_t(file_stat.st_mtime) );
    std::println( "  Fecha de cambio: {}", std::chrono::system_clock::from_time_t(file_stat.st_ctime) );

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
