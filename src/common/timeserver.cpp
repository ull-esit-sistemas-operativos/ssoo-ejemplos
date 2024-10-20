// timeserver.cpp - Funciones comunes a los ejemplos del servidor de tiempo.
//

#include <cstring>

#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "timeserver.hpp"

const int ALARM_DEFAULT_TIME = 5 /* seg. */; 

void alarm_signal_handler(int signum)
{
    time_t current_time = time( NULL );
    
    // Las funciones XXXX_r() son reentrantes => es seguro usarlas en manejadores de señal e hilos
    char current_time_string[26];
    ctime_r( &current_time, current_time_string );
    
    // No se puede usar printf() o std::cout. No está dentro de las funciones seguras en manejadores de señal
    write( STDOUT_FILENO, current_time_string, std::strlen(current_time_string) );
    
    // Programar la siguiente alarma
    alarm( ALARM_DEFAULT_TIME );
}

void start_alarm()
{
    struct sigaction alarm_action = {};
    alarm_action.sa_handler = &alarm_signal_handler;
    alarm_action.sa_flags = SA_RESTART;
    sigaction( SIGALRM, &alarm_action, NULL );

    alarm( ALARM_DEFAULT_TIME );   
}

void stop_alarm()
{
    alarm(0);
}
