// timeserver.c - Funciones comunes a los ejemplos del servidor de tiempo.
//

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "timeserver.h"

const int ALARM_DEFAULT_TIME = 3 /* seg. */; 

atomic_bool quit_app;

void alarm_signal_handler(int signum);
void term_signal_handler(int signum);

//////
// Control de la alarma
//

void start_alarm()
{
    struct sigaction alarm_action = {0};
    alarm_action.sa_handler = &alarm_signal_handler;
    sigaction( SIGALRM, &alarm_action, NULL );

    alarm( ALARM_DEFAULT_TIME );   
}

void stop_alarm()
{
    alarm(0);
}

void alarm_signal_handler(int signum)
{
    time_t current_time = time(NULL);
    
    // Las funciones XXXX_r() son reentrantes => es seguro usarlas en manejadores de señal e hilos
    char current_time_string[26];
    ctime_r( &current_time, current_time_string );
    
    // No se puede usar printf(). Mo está dentro de las funciones seguras en manejadores de señal
    write( STDOUT_FILENO, current_time_string, strlen(current_time_string) );
    
    // Programar la siguiente alarma
    alarm( ALARM_DEFAULT_TIME );
}

//////
// Configuración del manejo de señales
//

void setup_signals()
{
    struct sigaction term_action = {0};
    term_action.sa_handler = &term_signal_handler;

    sigaction( SIGTERM, &term_action, NULL );
    sigaction( SIGINT, &term_action, NULL );
}

void term_signal_handler(int signum)
{
    quit_app = true;
}
