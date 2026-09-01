// filelock.cpp - Ejemplo del uso de bloqueos de archivos
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
//  A diferencia de los otros ejemplos de este capítulo, el programa de control no se comunica con el servidor a
//  través de un canal propio, sino enviándole la señal SIGTERM. Por eso aquí no se usan las funciones de
//  'common/timeserver.hpp': como necesitamos esperar tanto la señal SIGALRM como las de terminación (SIGTERM,
//  SIGINT), usamos sigwait() en el hilo principal en lugar de un manejador de señales asíncrono. Así podemos usar
//  con total libertad el resto de la librería estándar de C++ --como std::println() o excepciones-- para tratar
//  cada señal, algo que no sería seguro hacer dentro de un manejador de señales.
//
//  Compilar:
//
//      g++ -o filelock filelock.cpp
//

#include <chrono>
#include <print>
#include <string>
#include <system_error>

#include <signal.h>     // Funciones relacionadas con señales.

#include "filelock-common.hpp"
#include "pid_file.hpp"

const int ALARM_DEFAULT_TIME = 3 /* seg. */;

int protected_main()
{
    // Crear el archivo con el PID del proceso. Si ya hay otro servidor en ejecución, esto lanzará una excepción.
    examples::pid_file pidfile{ PID_FILENAME };

    // Iniciar la alarma.
    alarm( ALARM_DEFAULT_TIME );

    // Configurar el conjunto de señales por las que debe esperar sigwait().
    // sigwait() bloqueará el hilo hasta que llegue una de estas señales.
    sigset_t sigwaitset;
    sigemptyset( &sigwaitset );
    sigaddset( &sigwaitset, SIGALRM );
    sigaddset( &sigwaitset, SIGTERM );
    sigaddset( &sigwaitset, SIGINT );

    // Bloquear las señales por las que esperaremos en sigwait(). Así se evita que el sistema las entregue mediante
    // un manejador de señales asíncrono, permitiéndonos tratarlas de forma síncrona en el hilo principal.
    sigprocmask( SIG_BLOCK, &sigwaitset, nullptr );

    // Esperar hasta que una señal indique que el proceso debe terminar.
    bool quit_app = false;
    while (!quit_app)
    {
        int signum;
        sigwait( &sigwaitset, &signum );

        if (signum == SIGALRM)
        {
            auto now = std::chrono::floor<std::chrono::seconds>( std::chrono::system_clock::now() );
            std::println( "{:%c}", now );

            // Programar la siguiente alarma.
            alarm( ALARM_DEFAULT_TIME );
        }
        else if (signum == SIGTERM || signum == SIGINT)
        {
            // Terminar el proceso.
            quit_app = true;
        }
    }

    // Detener la alarma.
    alarm(0);

    // Vamos a salir del programa...
    std::println( "Ha llegado orden de terminar ¡Adiós!" );

    return EXIT_SUCCESS;
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
