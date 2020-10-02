// timeserver.h - Funciones comunes a los ejemplos del servidor de tiempo.
//

#pragma once

// Variable global para indicar que la aplicación debe terminar
#if defined(__cplusplus)
#include <atomic>
extern std::atomic_bool quit_app;
extern "C" {
#else
#include <stdatomic.h>
extern atomic_bool quit_app;
#endif

// Control de la alarma
void start_alarm();
void stop_alarm();

// Configuración del manejo de señales
void setup_signals();

#if defined(__cplusplus)
}
#endif