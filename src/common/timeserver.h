// timeserver.h - Funciones comunes a los ejemplos del servidor de tiempo.
//

#pragma once

#if defined(__cplusplus)
#include <atomic>
extern std::atomic_bool quit_app;
extern "C" {
#endif

// Control de la alarma
void start_alarm();
void stop_alarm();

#if defined(__cplusplus)
}
#endif