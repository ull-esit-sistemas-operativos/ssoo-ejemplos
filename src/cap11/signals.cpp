// signals.cpp - Ejemplo del manejo de señales POSIX en C++
//
//  Compilar:
//
//      g++ -o signals signals.cpp
//
//  Uso: Ejecutar el programa y enviarle una señal con el comando kill. 
//  
//      kill -TERM <PID>    
//

#include <cstring>
#include <csignal>
#include <cstdlib>
#include <print>

#include <unistd.h>

void term_signal_handler(int signum)
{
    const char* message;

    if (signum == SIGTERM)
    {
        message = "Señal SIGTERM recibida.\n";
    }
    else if (signum == SIGINT)
    {
        message = "Señal SIGINT recibida.\n";
    }
    else
    {
        message = "Señal desconocida recibida.\n";
    }

    write( STDOUT_FILENO, message, std::strlen(message) );
}

void segv_signal_handler(int signum)
{
    const char* message = "¡Algo ha ido mal! Señal SIGSEGV recibida.\n";
    write( STDOUT_FILENO, message, std::strlen(message) );

    // No podemos ignorar el problema, ya que si salimos de aquí sin hacer nada, se reintentará la instrucción
    // que causó el error, devolviéndonos aquí de nuevo indefinidamente.
    //
    // Esta señal nos da la oportunidad de realizar tareas críticas antes de terminar.

    // RECUERDA: No es buena idea terminar así un programa en C++, pero esto es solo un ejemplo.
    std::exit(1);
}

int main()
{
    struct sigaction term_action = {};
    term_action.sa_handler = &term_signal_handler;
    term_action.sa_flags = SA_RESTART;

    sigaction( SIGTERM, &term_action, nullptr );
    sigaction( SIGINT, &term_action, nullptr );
    sigaction( SIGHUP, &term_action, nullptr );

    struct sigaction segv_action = {};
    segv_action.sa_handler = &segv_signal_handler;

    sigaction( SIGSEGV, &segv_action, nullptr );

    std::println( "Pulse ENTER para provocar una violación de segmento..." );
    std::getc(stdin);

    char *bad_ptr = nullptr;
    *bad_ptr = 'a';    

    return EXIT_SUCCESS;
}
