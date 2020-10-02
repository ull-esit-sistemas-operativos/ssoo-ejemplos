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

// Para indicar si las funciones que instalan manejadores de señal deben activar el flag
// SA_RESTART (por defecto) o no. Es un hack para 'fifo-server.c' que necesita que no se active
// el flag para que cuando llegue una señal la llamada al sistema en curso se interrumpa.
void set_signal_restart(bool restart);

// Control de la alarma
void start_alarm();
void stop_alarm();

// Configuración del manejo de señales
void setup_signals();

#if defined(__cplusplus)
}
#endif