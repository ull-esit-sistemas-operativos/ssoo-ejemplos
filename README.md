# Ejemplos de las clases de Sistemas Operativos

En los [apuntes de la asignatura](https://ull-esit-sistemas-operativos.github.io/ssoo-apuntes/so2425/) y durante las clases de Sistemas Operativos del Grado en Ingeniería Informática, solemos hacer referencia a algunos programas de ejemplo que usamos para ilustrar lo explicado.
Este proyecto es una recopilación de dichos programas.

**Tabla de contenidos**
- [Ejemplos incluidos](#ejemplos-incluidos)
- [Requisitos de compilación](#requisitos-de-compilación)
- [Compilación](#compilación)
- [Autor y contacto](#autor-y-contacto)

## Ejemplos incluidos

Concretamente los ejemplos incluidos actualmente son:

 * `src/cap04/` — **4. Componentes del sistema**
   * `led-test.cpp` — Acceso a la E/S serial en UNIX. Ejemplo del control de un LED tricolor conectado a un Arduino que a su vez está conectado al puerto serie del ordenador.
 * `src/cap06/` — **6. Interfaz de programación de aplicaciones**
   * `syscall.s` — Llamadas al sistema en el simulador en Linux para x86-64.
   * `softstack.c` — Ejemplo básico en C para mostrar la pila de software con `strace` o `ltrace`.
   * `softstack.cpp` — Ejemplo básico en C++ para mostrar la pila de software con `strace` o `ltrace`.
 * `src/cap09/` — **9. Procesos**
   * `fork.cpp` — Creación de procesos mediante `fork()`.
   * `fork-exec.cpp` — Ejecutar otros programas mediante `fork()` y `exec()`.
 * `src/cap11/` — **11. Comunicación mediante paso de mensajes**
   * `mqueue/` — Ejemplo de comunicación entre procesos mediante colas de mensajes POSIX.
   * `signals/` — Ejemplo básico de manejo de señales POSIX.
   * `sockets/` — Ejemplo de comunicación entre procesos mediante _sockets_.
   * `tuberías/` — Ejemplos de comunicación entre procesos mediante tuberías anónimas y con nombre, y de redirección de la E/S estándar.
 * `src/cap12/` — **12. Memoria compartida**
   * `anom-shared-memory.cpp` — Comunicación entre procesos padre e hijo mediante memoria compartida anónima.
   * `shared-memory.cpp` — Ejemplo de comunicación entre procesos mediante memoria compartida.
   * `shared-memory-control.cpp` — Programa de control del ejemplo de comunicación entre procesos mediante memoria compartida.
 * `src/cap13/` — **13. Hilos**
   * `pthreads.cpp` — Creación de hilos mediante POSIX Threads.
   * `pthreads-cancel-factorial.cpp` — Cancelación de hilos mediante POSIX Threads: Cálculo del factorial de un número.
   * `pthreads-factorial.cpp` — Uso de hilos mediante POSIX Threads: Cálculo del factorial de un número.
   * `threads.cpp` — Creación de hilos con `std::jthread` en C++.
   * `threads-cancel-factorial.cpp` — Cancelación cooperativa de hilos con `std::jthread` en C++: Cálculo del factorial de un número.
   * `threads-factorial.cpp` — Uso de hilos con `std::jthread` en C++: Cálculo del factorial de un número.
 * `src/cap14/` — **14. Sincronización**
   * `mutex/` — Ejemplos de sincronización de hilos mediante mutex, en POSIX Threads y en C++.
   * `semáforos/` — Ejemplo de la implementación de un semáforo en C++ a partir de un mutex y una variable de condición.
 * `src/cap17/` — **17. Memoria virtual**
   * `mapped-files.cpp` — Archivos mapeados en memoria con `mmap()`.
 * `src/cap19/` — **19. Sistemas de archivos**
   * `file-copy.cpp` — Copia de archivos con `read()` y `write()`.
   * `file-attribs.cpp` — Leer y mostrar los atributos de archivo.
   * `filelock.cpp` — Ejemplo del uso de bloqueo de archivos.
   * `filelock-control.cpp` — Programa de control del ejemplo del uso de bloqueo de archivos.
   * `dir-list.cpp` — Listar el contenido de un directorio.
 * `src/otros/` — **Otros ejemplos**
   * `yash.cpp` — Ejemplo muy básico del funcionamiento interno de una shell.

## Requisitos de compilación

Para la compilación de estos ejemplos en Linux es necesario tener instalado un compilador de C++ que soporte C++23 —como **GCC 14**— y los siguientes paquetes:

 * `build-essential`
 * `cmake`

Mientras que en Microsoft Windows se necesita almenos **Visual Studio Build Tools 2022**, que incluye una versión de **CMake**.

## Compilación

Los ejemplos se compilan de manera sencilla siguiendo estos pasos:

 1. Ir al directorio del proyecto.
 2. Ejecutar `cmake -B build`
 3. Ejecutar `cmake --build build`

En Microsoft Windows estos comandos deben ejecutarse desde la consola de **Developer Command Prompt**.

En cada sistema solo se compilarán los ejemplos compatibles.
Los ejecutables resultantes de la compilación son guardados en el directorio `build/bin/`.

## Autor y contacto

Si tienes cualquier cuestión no dudes en ponerte en contacto con Jesús Torres <[jmtorres@ull.es](mailto:jmtorres@ull.es)>.
