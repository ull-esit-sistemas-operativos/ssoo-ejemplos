// file-copy.cpp - Copia un archivo a otro
//
//  Este programa copia un archivo a otro. Si el archivo de destino ya existe, se sobreescribe.
//  Si el archivo de origen no existe, se muestra un mensaje de error.
//
//  Compilar:
//
//      g++ -o file-copy file-copy.cpp
//

#include <print>
#include <system_error>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int protected_main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::println( stderr, "Uso: {} <origen> <destino>", argv[0] );
        return EXIT_FAILURE;
    }

    // Abrir el archivo de origen.
    int source_fd = open( argv[1], O_RDONLY );
    if (source_fd == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en open(): del archivo de origen" );
    }

    // Obtener los atributos del archivo de origen.
    struct stat source_stat;
    if (fstat( source_fd, &source_stat ) == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en fstat() del archivo de origen" );
    }

    // Comprobar que el archivo de origen es un archivo regular.
    if (!S_ISREG( source_stat.st_mode ))
    {
        throw std::runtime_error( "El archivo de origen no es un archivo regular" );
    }

    // Crear el archivo de destino.
    //  - Si el archivo de destino ya existe, se sobreescribe.
    //  - Si el archivo de destino no existe, se crea con los mismos permisos que el archivo de origen.
    int dest_fd = open( argv[2], O_WRONLY | O_CREAT | O_TRUNC, source_stat.st_mode );
    if (dest_fd == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en open() del archivo de destino" );
    }

    // Copiar el contenido del archivo de origen al de destino.
    char buffer[BUFSIZ];
    ssize_t bytes_read;
    while ((bytes_read = read( source_fd, buffer, sizeof(buffer) )) > 0)
    {
        if (write( dest_fd, buffer, bytes_read ) == -1)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en write() del archivo de destino" );
        }
    }

    if (bytes_read == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en read() del archivo de origen" );
    }

    // Cerrar los archivos.
    close( source_fd );
    close( dest_fd );

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