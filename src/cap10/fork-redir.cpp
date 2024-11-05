// fork-redir.cpp - Ejemplo del uso de tuberías para redirigir la E/S estándar
//
//  Compilar:
//
//      gcc -o fork-redir fork-redir.cpp
//

#include <array>        // Recomendada para crear arrays de tamaño fijo compatibles con C
#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en
                        // C++ pues mete las funciones en el espacio de nombres 'std', como el resto de la librería
                        // estándar de C++.
#include <print>
#include <string>
#include <system_error>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Función para leer todo el contenido de un descriptor de archivo de lectura.
std::string read_all( int fd )
{
    std::string stdout_buffer;
    std::array<char, 1024> read_buffer;

    // Para leer todo el contenido de la tubería, se lee en un bucle hasta que read() devuelva 0.
    // read() devuelve el número de bytes leídos o -1 si hay un error. Si no hay más datos que leer, devuelve 0,
    // lo que indica que todos los descriptores de escritura de la tubería se han cerrado, por lo que no van a llegar
    // más datos.
    ssize_t bytes_read = 1;
    while (bytes_read > 0)
    {
        // Se lee en bloques de `read_buffer.size()` bytes cada vez y se añade al buffer de salida `stdout_buffer`.
        bytes_read = read( fd, read_buffer.data(), read_buffer.size() );
        if (bytes_read < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en read()" );
        }
        else if (bytes_read > 0)
        {
            stdout_buffer.append( read_buffer.data(), bytes_read );
        }
    }

    return stdout_buffer;
}

int protected_main()
{
    // Crear una tubería
    std::array<int, 2> fds;     // Equivalente a 'int fds[2]' pero más seguro

    // Crear una tubería para conectar el proceso hijo con el padre
    int return_code = pipe( fds.data() );
    if (return_code < 0) {
        throw std::system_error( errno, std::system_category(), "Fallo en pipe()" );
    }

    // Crear el proceso hijo
    pid_t child = fork();
    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo

        // El hijo hereda el acceso a la tubería, pero de su copia de los descriptores de los dos extremos solo
        // necesita el de escritura, para escribir la salida estándar del proceso.
        // Cerramos el de lectura.
        close( fds[0] );

        // Un programa, como el que vamos a ejecutar, que quiere mostrar algo por pantalla, generalmente usa la salida
        // estándar. La salida estándar está disponible en el descriptor de archivo STDOUT_FILENO (1), así que las
        // funciones printf() o puts() que usan los programas lo que realmente hacen es write(STDOUT_FILENO, texto, ...)
        //
        // Si queremos que lo que los programas escriban en su descriptor STDOUT_FILENO realmente vaya a la entrada
        // de la tubería, lo que se puede hacer es copiar las propiedades del descriptor en fds[1] (que es el de la
        // entrada de la tubería) en el descriptor STDOUT_FILENO. Así STDOUT_FILENO ya no sirve para mandar cosas a
        // donde las mandaba en el padre (la terminal, por ejemplo) sino a donde también las manda fds[1].
        //
        // Eso es lo que hace la siguiente función.
        dup2( fds[1], STDOUT_FILENO);

        // ...y una vez hecha la copia, ya no necesitamos el descriptor en fds[1]
        close( fds[1] );
        
        // Con todo configurado, sustituimos el programa actual por el que nos interesa.
        execl( "/bin/ls", "ls", "-l", NULL );
        
        std::println( stderr, "[HIJO] Error ({}) al ejecutar el programa: {}", errno, std::strerror(errno) );
        return EXIT_FAILURE;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre 
        
        // El padre tiene acceso total a la tubería pero solo necesita el descriptor de lectura para obtener el
        // resultado del hijo. Además, el descriptor de escritura del hijo debe ser el único descriptor de escritura
        // abierto, para recibir un fin de archivo cuando el hijo muera.
        //
        // Cerramos el descriptor de escritura.
        close(fds[1]);
        
        // Leemos toda la salida del proceso hijo y cerrar el descriptor de lectura.
        std::string stdout_buffer = read_all( fds[0] );
        close( fds[0] );

        // Sabemos que el hijo ha terminado porque el otro extremo de la tubería se cerró.
        // Aun así hay que llamar a wait() para:
        //
        //  a) Evitar que el proceso hijo se quede como proceso zombi.
        //  b) Obtener el estado de salida para saber si terminó con éxito (salió con 0) y, por tanto, que el
        //     contenido de buffer es válido.
        int status;
        wait( &status );

        if (! (WIFEXITED(status) && WEXITSTATUS(status) == 0) )
        {
            std::println( "El comando 'ls' terminó inesperadamente." );
            return EXIT_FAILURE;
        }

        // Contar el número de líneas de la salida del comando
        int num_of_lines = std::count_if( stdout_buffer.begin(), stdout_buffer.end(),
            [](char c) { return c == '\n'; } );

        std::println( "La salida de 'ls' tiene {} líneas", num_of_lines );

        return EXIT_SUCCESS;
    }
    else {
        // Aquí solo entra el padre si no pudo crear el hijo
        throw std::system_error( errno, std::system_category(), "Fallo en fork()" );

        close( fds[0] );
        close( fds[1] );
        return EXIT_FAILURE;
    }
}

int main()
{
    try
    {
        return protected_main();
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