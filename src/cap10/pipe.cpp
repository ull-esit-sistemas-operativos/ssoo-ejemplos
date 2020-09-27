// pipe.cpp - Ejemplo del uso de tuberías para comunicar procesos
//
//  El programa solicita al usuario un número por la entrada estándar, lanza un proceso hijo para
//  que calcule el factorial y lee de la tubería que los onecta el resultado para utilizarlo.
//
//  Compilar:
//
//      g++ -o pipe pipe.cpp
//

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <sys/types.h>
#include <sys/wait.h>

#include <array>        // Recomendada para crear arrays de tamaño fijo compatibles con C
#include <iostream>

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo 
#include <cstring>      // <stdlib.h> como <cstdlib>. La primera es para usar con C mientras que
                        // la segunda es la recomendada en C++ pues mete las funciones en el
                        // espacio de nombres 'std', como el resto de la librería estándar de C++.

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

int get_user_input()
{
    std::cout << "[PADRE] Introduzca un número: ";
    std::cout.flush();
    
    int number;
    std::cin >> number;
    return number;
}

int calculate_factorial( int number )
{
    std::cout << "[HIJO] Calculando...";
    std::cout.flush();

    int factorial = 1;
    for ( int i = 2; i < number; i++ )
    {
        factorial = factorial * number;

        std::cout << '.';
        std::cout.flush();
    }

    std::cout << '\n';
    std::cout.flush();

    return factorial;
}

int main()
{
    int number = get_user_input();

    // Crear una tubería
    std::array<int, 2> fds;     // Equivalente a 'int fds[2]' pero más seguro

    int return_code = pipe( fds.data() );
    if (return_code < 0) {
        std::cerr << fmt::format( "Error {} al crear la tubería: {}\n", errno, std::strerror(errno) );
        return 3;
    }
 
    // Crear el proceso hijo para el cálculo del factorial
    pid_t child = fork();
    if (child == 0)
    {   
        // Aquí solo entra el proceso hijo

        // El hijo hereda el acceso a la tubería pero de su copia de los descriptores de los dos
        // extremos solo necesita el de escritura para devolver el resultado.
        // Cerramos el de lectura.
        close( fds[0] );

        int factorial = calculate_factorial( number );
        auto factorial_string = std::to_string( factorial );

        // Escribir en la tubería el resultado convertido a cadena sin el '\0' del final.
        ssize_t bytes_written = write( fds[1], factorial_string.c_str(), factorial_string.length() );
        if ( static_cast<size_t>(bytes_written) < factorial_string.length() )
        {
            std::cerr << fmt::format( "Error {} al escribir en la tubería: {}\n", errno, strerror(errno) );
            return 1;
        }

        // Al terminar el proceso todos los recursos se liberan y la entrada de tubería del hijo
        // se cierra. Si ese es el último descriptor abierto de la entrada a la tubería, el padre
        // recibirá un fin de archivo (EOF)) cuando no quede nada más por leer.
        return 0;
    }
    else if (child > 0)
    {
        // Aquí solo entra el proceso padre 
        
        // El padre tiene acceso total a la tubería pero solo necesita el descriptor de lectura para
        // obtener el resultado del hijo. Además, el descriptor de escritura del hijo debe ser el
        // único descriptor de escritura abierto, para recibir un fin de archivo cuando el hijo muera.
        // Cerramos el descriptor de escritura.
        close(fds[1]);

        std::array<char, 255> read_buffer;
        char* read_buffer_begin = read_buffer.begin();
        ssize_t bytes_read = 1;

        while (bytes_read > 0)
        {
            bytes_read = read( fds[0], read_buffer_begin, read_buffer.end() - read_buffer_begin - 1 );
            if (bytes_read > 0)
            {
                read_buffer_begin += bytes_read;
            }
        }

        // Aquí solo se llega si read() devuelve 0, que indica fin de archivo, o -1, que es un error.  
        if (bytes_read < 0)
        {
            std::cerr << fmt::format( "Error {} al leer la tubería: {}\n", errno, strerror(errno) );
            return 5;
        }

        // Sabemos que el hijo ha terminado. Aun así hay que llamar a wait() para:
        //  a) Evitar que el proceso hijo se quede como proceso zombi.
        //  b) Obtener el estado de salida para saber si terminó con éxito (salió con 0) y, por tanto,
        //     que el contenido de buffer es válido.
        int status;
        wait(&status);
        
        if (! (WIFEXITED(status) && WEXITSTATUS(status) == 0) )
        {
            std::cerr << "Error: La tarea terminó inesperadamente.\n";
            return 6;
        }
        
        std::string factorial(read_buffer.begin(), read_buffer_begin);
        std::cout << fmt::format( "[PADRE] El factorial de {} es {}\n", number, factorial );
    }
    else
    {
        // Aquí solo entra el padre si no pudo crear el hijo
        std::cerr << fmt::format( "Error {} al crear el procesos: {}\n", errno, strerror(errno) );
        return 4;
    }

    return 0;
}