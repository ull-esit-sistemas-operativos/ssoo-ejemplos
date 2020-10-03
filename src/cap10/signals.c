// signals.c - Ejemplo del manejo de señales POSIX
//
//  Compilar:
//
//      gcc -o signals signals.c
//
//  Uso: Ejecutar el programa y enviarle una señal con el comando kill. 
//  
//      kill -TERM <PID>    
//

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <signal.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

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

    write( STDOUT_FILENO, message, strlen(message));
}

void segv_signal_handler(int signum)
{
    const char* message = "¡Algo ha ido mal! Señal SIGSEGV recibida.\n";
    write( STDOUT_FILENO, message, strlen(message));

    // No podemos ignorar el problema porque si salimos de aquí sin hacer nada se reintentará la instrucción que causo
    // el error, devolviéndonos aquí de nuevo, indefinidamente.
    //
    // Lo interesante es que esta señal nos da la oportunidad de hacer cosas críticas antes de terminar.

    exit(1);
}

int main()
{
    struct sigaction term_action = {0};
    term_action.sa_handler = &term_signal_handler;
    term_action.sa_flags = SA_RESTART;

    sigaction( SIGTERM, &term_action, NULL );
    sigaction( SIGINT, &term_action, NULL );
    sigaction( SIGHUP, &term_action, NULL );

    struct sigaction segv_action = {0};
    segv_action.sa_handler = &segv_signal_handler;

    sigaction( SIGSEGV, &segv_action, NULL );

    puts("Pulse ENTER para provocar una violación de segmento...");
    getc(stdin);

    char *bad_ptr = NULL;
    *bad_ptr = 'a';    

    return 0;
}
