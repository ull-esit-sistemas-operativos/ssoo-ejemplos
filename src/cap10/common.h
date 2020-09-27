// common.h - Funciones comunes a los ejemplos del capítulo 10.
//

#pragma once

#include <stdatomic.h>

// Variable global para indicar que la aplicación debe terminar
extern atomic_bool quit_app;

// Control de la alarma
void start_alarm();
void stop_alarm();

// Configuración del manejo de señales
void setup_signals();
