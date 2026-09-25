# Ejemplos de las clases de Sistemas Operativos

En los [apuntes de la asignatura](https://ull-esit-sistemas-operativos.github.io/ssoo-apuntes/so2425/) y durante las clases de Sistemas Operativos del Grado en Ingeniería Informática, solemos hacer referencia a algunos programas de ejemplo que usamos para ilustrar lo explicado.
Este proyecto es una recopilación de dichos programas.

**Tabla de contenidos**
- [Ejemplos de las clases de Sistemas Operativos](#ejemplos-de-las-clases-de-sistemas-operativos)
  - [Organización del proyecto](#organización-del-proyecto)
  - [Ejemplos incluidos](#ejemplos-incluidos)
  - [Requisitos de compilación](#requisitos-de-compilación)
  - [Compilación](#compilación)
  - [Autor y contacto](#autor-y-contacto)

## Organización del proyecto

Los ejemplos están agrupados por capítulo de los apuntes, en `src/capNN/`.
Dentro de cada capítulo, el subdirectorio en el que está cada ejemplo indica para qué API del sistema operativo está escrito:

| Ubicación | API | Se compila en |
| --- | --- | --- |
| La raíz del capítulo | La librería estándar de C++ | Todos los sistemas |
| `posix/` | La [API POSIX](https://man7.org/linux/man-pages/man7/standards.7.html) | Sistemas UNIX |
| `windows/` | La [API de Windows](https://learn.microsoft.com/en-us/windows/win32/api/), antes llamada API Win32 | Microsoft Windows |

De esta manera, un mismo ejemplo resuelto con las dos APIs aparece con el mismo nombre en `posix/` y en `windows/`, y se pueden comparar uno al lado del otro.

La excepción son los ejemplos que llevan el nombre de la función que ilustran, como `fork.cpp` o `fork-pipe.cpp`: su versión de Windows lleva el nombre de la función equivalente de la API de Windows, porque es lo que cambia de una a otra.
Así, `posix/fork-exec.cpp` se corresponde con `windows/createprocess.cpp`.

Además, en `src/common/` está el código que comparten varios ejemplos y en `lib/` las librerías de terceros que utilizan.

## Ejemplos incluidos

Concretamente los ejemplos incluidos actualmente son:

 * `src/cap04/` — **4. Componentes del sistema**
   * `posix/led-test.cpp` — Acceso a la E/S serial en UNIX. Ejemplo del control de un LED tricolor conectado a un Arduino que a su vez está conectado al puerto serie del ordenador.
 * `src/cap06/` — **6. Interfaz de programación de aplicaciones**
   * `posix/syscall.s` — Llamadas al sistema en el simulador en Linux para x86-64.
   * `posix/softstack.c` — Ejemplo básico en C para mostrar la pila de software con `strace` o `ltrace`.
   * `softstack.cpp` — Ejemplo básico en C++ para mostrar la pila de software con `strace` o `ltrace`.
 * `src/cap09/` — **9. Procesos**
   * `posix/fork.cpp` — Creación de procesos mediante `fork()`.
   * `posix/fork-exec.cpp` — Ejecutar otros programas mediante `fork()` y `exec()`.
   * `windows/createprocess.cpp` — Ejecutar otros programas en Windows mediante `CreateProcess()`.
 * `src/cap11/` — **11. Comunicación mediante paso de mensajes**
   * `mqueue/` — Ejemplo de comunicación entre procesos mediante colas de mensajes POSIX.
   * `signals/` — Ejemplo básico de manejo de señales POSIX.
   * `sockets/` — Ejemplo de comunicación entre procesos mediante _sockets_.
   * `tuberías/` — Ejemplos de comunicación entre procesos mediante tuberías anónimas y con nombre, y de redirección de la E/S estándar. También las versiones con la API de Windows, incluida la de tuberías con nombre.
 * `src/cap12/` — **12. Memoria compartida**
   * `posix/anom-shared-memory.cpp` — Comunicación entre procesos padre e hijo mediante memoria compartida anónima.
   * `posix/shared-memory.cpp` — Ejemplo de comunicación entre procesos mediante memoria compartida.
   * `posix/shared-memory-control.cpp` — Programa de control del ejemplo de comunicación entre procesos mediante memoria compartida.
   * `windows/shared-memory.cpp` y `windows/shared-memory-control.cpp` — Los mismos ejemplos en Windows con `CreateFileMapping()` y semáforos con nombre.
 * `src/cap13/` — **13. Hilos**
   * `posix/pthreads.cpp` — Creación de hilos mediante POSIX Threads.
   * `posix/pthreads-cancel-factorial.cpp` — Cancelación de hilos mediante POSIX Threads: Cálculo del factorial de un número.
   * `posix/pthreads-factorial.cpp` — Uso de hilos mediante POSIX Threads: Cálculo del factorial de un número.
   * `windows/beginthreadex.cpp` — Creación de hilos en Windows mediante `_beginthreadex()`.
   * `threads.cpp` — Creación de hilos con `std::jthread` en C++.
   * `threads-cancel-factorial.cpp` — Cancelación cooperativa de hilos con `std::jthread` en C++: Cálculo del factorial de un número.
   * `threads-factorial.cpp` — Uso de hilos con `std::jthread` en C++: Cálculo del factorial de un número.
 * `src/cap14/` — **14. Sincronización**
   * `mutex/` — Ejemplos de sincronización de hilos mediante mutex, en POSIX Threads y en C++.
   * `semáforos/` — Ejemplo de la implementación de un semáforo en C++ a partir de un mutex y una variable de condición.
 * `src/cap17/` — **17. Memoria virtual**
   * `posix/mapped-files.cpp` — Archivos mapeados en memoria con `mmap()`.
   * `windows/mapped-files.cpp` — Archivos mapeados en memoria en Windows con `CreateFileMapping()` y `MapViewOfFile()`.
 * `src/cap19/` — **19. Sistemas de archivos**
   * `posix/file-copy.cpp` — Copia de archivos con `read()` y `write()`.
   * `windows/file-copy.cpp` — Copia de archivos en Windows con `ReadFile()` y `WriteFile()`.
   * `posix/file-attribs.cpp` — Leer y mostrar los atributos de archivo.
   * `windows/file-attribs.cpp` — Leer y mostrar los atributos de archivo en Windows con `GetFileInformationByHandle()`.
   * `posix/filelock.cpp` — Ejemplo del uso de bloqueo de archivos.
   * `posix/filelock-control.cpp` — Programa de control del ejemplo del uso de bloqueo de archivos.
   * `windows/filelock.cpp` y `windows/filelock-control.cpp` — El mismo ejemplo en Windows con `LockFileEx()`.
   * `posix/dir-list.cpp` — Listar el contenido de un directorio.
   * `windows/dir-list.cpp` — Listar el contenido de un directorio en Windows con `FindFirstFile()` y `FindNextFile()`.
 * `src/otros/` — **Otros ejemplos**
   * `posix/yash.cpp` — Ejemplo muy básico del funcionamiento interno de una shell.

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

Cada ejemplo documenta además, en el comentario `Compilar:` de su cabecera, el comando con el que compilarlo a mano desde su propio directorio, sin usar CMake.

## Autor y contacto

Si tienes cualquier cuestión no dudes en ponerte en contacto con Jesús Torres <[jmtorres@ull.es](mailto:jmtorres@ull.es)>.
