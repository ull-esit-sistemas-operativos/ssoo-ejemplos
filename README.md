## Ejemplos de las clases de Sistemas Operativos

[![CC-0 license](https://img.shields.io/badge/License-CC--0-blue.svg)](https://creativecommons.org/publicdomain/zero/1.0)

### Descripción

En los apuntes de la asignatura y durante las clases de Sistemas Operativos del Grado en Ingeniería Informática, solemos hacer referencia a algunos programas de ejemplo que usamos para ilustrar lo explicado.
Este proyecto es una recopilación de dichos programas.

Concretamente los ejemplos incluidos son:

 * `src/cap04/led_test.cpp` — Acceso a la E/S serial en UNIX. Ejemplo del control de un LED tricolor conectado a un Arduino que a su vez está conectado al puerto serie del ordenador.
 * `src/cap06/syscall.s` — Llamadas al sistema en el simulador en Linux para x86-64.
 * `src/cap06/softstack.c` — Ejemplo básico en C para mostrar la pila de software con `strace` o `ltrace`.
 * `src/cap06/softstack.cpp` — Ejemplo básico en C++ para mostrar la pila de software con `strace` o `ltrace`.
 * `src/cap09/fork.cpp` — Creación de procesos mediante `fork()`.
 * `src/cap09/fork-exec.cpp` — Ejecutar otros programas mediante `fork()` y `exec()`.
 * `src/cap10/fifo-client.c` — Cliente en C de comunicación entre procesos mediante tuberías con nombre.
 * `src/cap10/fifo-client.cpp` — Cliente en C++ de comunicación entre procesos mediante tuberías con nommbre.
 * `src/cap10/fifo-server.c` — Servidor de comunicación entre procesos mediante tuberías con nombre.
 * `src/cap10/fork-pipe.cpp` — Comunicación entre procesos padre e hijo mediante tuberías anónimas.
 * `src/cap10/fork-redir.cpp` — Redirección de E/S estándar usando tuberías anónimas.
 * `src/cap10/mqueue-client.cpp` — Cliente de comunicación entre procesos mediante colas de mensajes POSIX.
 * `src/cap10/mqueue-server.cpp` — Servidor de comunicación entre procesos mediante colas de mensajes POSIX.
 * `src/cap10/signals.cpp` — Ejemplo básico de manejo de señales POSIX.
 * `src/cap10/socket-client.cpp` — Cliente de comunicación entre procesos mediante colas de mensajes POSIX.
 * `src/cap10/socket-server.cpp` — Servidor de comunicación entre procesos mediante colas de mensajes POSIX.
 * `src/cap11/anom-shared-memory.cpp` — Comunicación entre procesos padre e hijo mediante memoria compartida anónima.
 * `src/cap11/shared-memory.cpp` — Ejemplo de comunicación entre procesos mediante memoria compartida.
 * `src/cap11/shared-memory-control.cpp` — Programa de control del ejemplo de comunicación entre procesos mediante memoria compartida.
 * `src/cap12/jthreads-factorial.cpp` — Uso de hilos con `std::jthread` y cancelación coordinada en C++: Cálculo del factorial de un número.
 * `src/cap12/pthreads.cpp` — Creación de hilos mediante POSIX Threads.
 * `src/cap12/pthreads-factorial.cpp` — Uso de hilos mediante POSIX Threads: Cálculo del factorial de un número.
 * `src/cap12/threads.cpp` — Creación de hilos con `std::thread` en C++.
 * `src/cap12/threads-factorial.cpp` — Uso de hilos con `std::thread` en C++: Cálculo del factorial de un número.
 * `src/cap13/pthreads-sync-counter.cpp` — Sincronización de hilos mediante mutex de POSIX Threads: Incrementar un contador.
 * `src/cap13/pthreads-sync-factorial.cpp` — Sincronización de hilos mediante mutex de POSIX Threads: Cálculo del factorial de un número.
 * `src/cap13/threads-sync-counter.cpp` — Sincronización de hilos mediante mutex en C++: Incrementar un contador.
 * `src/cap13/threads-sync-factorial.cpp` — Sincronización de hilos mediante mutex en C++: Cálculo del factorial de un número.
 * `src/cap13/threads-sync-semaphore.cpp` — Sincronización de hilos mediante semáforos en C++.
 * `src/cap17/mapped-files.c` — Archivos mapeados en memoria con `mmap()`.
 * `src/cap17/mapped-files.cpp` — Archivos mapeados en memoria con `mmap()` en C++.
 * `src/cap19/file-copy.cpp` — Copia de archivos con `read()` y `write()`.
 * `src/cap19/file-attribs.cpp` — Leer y mostrar los atributos de archivo.
 * `src/cap19/filelock-server.c` — Ejemplo del uso de bloqueo de archivos.
 * `src/cap19/filelock-stop.cpp` — Programa de control del ejemplo del uso de bloqueo de archivos.
 * `src/cap19/dir-list.cpp` — Listar el contenido de un directorio.
 * `src/otros/yash.cpp` — Ejemplo muy básico del funcionamiento interno de una shell.

### Requisitos

Para la compilación de estos ejemplos en Linux es necesario tener instalado un compilador de C++ que soporte C++23 —como **GCC 14**— y los siguientes paquetes:

 * `build-essential`
 * `cmake`

Mientras que en Microsoft Windows se necesita almenos **Visual Studio Build Tools 2022**, que incluye una versión de **CMake**.

### Compilación

Los ejemplos se compilan de manera sencilla siguiendo estos pasos:

 1. Ir al directorio del proyecto.
 2. Ejecutar `cmake -B build`
 3. Ejecutar `cmake --build build`

En Microsoft Windows estos comandos deben ejecutarse desde la consola de **Developer Command Prompt**.

En cada sistema solo se compilarán los ejemplos compatibles.
Los ejecutables resultantes de la compilación son guardados en el directorio `build/bin/`.

### Autor y contacto

Si tienes cualquier cuestión no dudes en ponerte en contacto con Jesús Torres <[jmtorres@ull.es](mailto:jmtorres@ull.es)>.
