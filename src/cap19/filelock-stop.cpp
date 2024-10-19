// filelock-stop.cpp - Programa de control del ejemplo del uso de bloqueos de archivos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  crea un archivo con el PID del proceso. Este archivo es bloqueado durante su creación para que solo un servidor
//  pueda escribir su PID en él. Otros servidores detectarán la situación y terminarán inmediatamente.
//
//  El programa de control puede usar este archivo para conocer el PID para enviar una señal al servidor y hacer
//  que termine.
//
//  Esta técnica es muy usada por los servicios del sistema. Frecuentemente crean un subdirectorio con el nombre del
//  servicio dentro de /var/run. Allí colocan un archivo '.pid' con el PID del proceso, así como otros recursos
//  necesarios para la comunicación con el servicio, como sockets de dominio UNIX o FIFO.
//  El archivo '.pid' permite a los clientes saber si el servicio está en ejecución y mandarle señales para detenerlo
//  o reiniciarlo. 
//
//  Compilar:
//
//      g++ -o filelock-stop filelock-stop.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la
                        // librería estándar de C++.
#include <fstream>      
#include <print>
#include <string>
#include <system_error>

#include <unistd.h>
#include <signal.h>

using namespace std::literals;

const std::string PID_FILENAME = "filelock-server.pid"s;

int main()
{
    std::ifstream pidfile_stream{ PID_FILENAME };
    
    // Comprobamos si se pudo abrir el archivo con el PID.
    if ( ! pidfile_stream.is_open() )
    {
        // Con la API de la librería estándar de C++ no se pueden conocer más detalles sobre el motivo, como:
        // si el archivo no existe, no se tienen permisos suficientes, etc.
        std::println( stderr, "Error: No se puede abrir '{}'.\n"
                              "Quizás el servidor no se esté ejecutando o no se tengan permisos suficientes"
                              , PID_FILENAME );
        return EXIT_FAILURE;
    }

    // Usamos std::ifstream::read() para intentar leer el archivo en una sola llamada al sistema read().
    // Es importante hacerlo en una sola llamada porque, como el servidor también escribe en el archivo en una sola
    // operación, así estamos seguros de que la lectura es o antes o después de la escritura (pero nunca en medio)
    // gracias a la semántica de coherencia POSIX. Por tanto, incluso sin usar bloqueo de archivos, el PID se leerá
    // completo o no se leerá nada.

    std::array<char, 20> buffer;
    pidfile_stream.read( buffer.data(), sizeof(buffer) );
    
    // Convertir el PID lido como cadena en un número
    pid_t server_pid;
    std::from_chars(buffer.data(), buffer.data() + pidfile_stream.gcount(), server_pid);

    std::println( "Cerrando el servidor..." );
    kill( server_pid, SIGTERM );

    std::println( "¡Adiós!" );

    return EXIT_SUCCESS;
}
