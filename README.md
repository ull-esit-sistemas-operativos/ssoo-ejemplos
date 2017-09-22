## Ejemplos utilizados durante las clases teóricas de Sistemas Operativos

<a rel="license" href="http://creativecommons.org/publicdomain/zero/1.0/deed.es"><img alt="Licencia Creative Commons" style="border-width:0" src="http://i.creativecommons.org/l/zero/1.0/88x31.png" /></a><br />Este obra de <a xmlns:cc="http://creativecommons.org/ns#" href="http://jmtorres.webs.ull.es/" property="cc:attributionName" rel="cc:attributionURL">Jesús Torres</a> está bajo una <a rel="license" href="http://creativecommons.org/publicdomain/zero/1.0/deed.es">Licencia Creative Commons Public Domain Dedication 1.0</a>.

<a href="http://flattr.com/thing/1003364/Clases-de-Sistemas-Operativos-by-aplatanado" target="_blank">
<img src="http://api.flattr.com/button/flattr-badge-large.png" alt="Flattr this" title="Flattr this" border="0" /></a>


### Descripción

Durante mis clases de Sistemas Operativos en 2º de Grado de Ingeniería
Informática suelo hacer uso de algunos programas de ejemplo con el objetivo de
ilustrar lo explicado en la teoría. Este proyecto no es más que una
recopilación de dichos programas.

Concretamente los ejemplos incluidos son:

 * `mips/misp-syscall.s` - Llamadas al sistema en el simulador de MIPS SPIMi.
 * `src/fifo-client.cpp` - Cliente de comunicación entre procesos mediante tuberías con nombre.
 * `src/fifo-server.cpp` - Servidor de comunicación entre procesos mediante tuberías con nombre.
 * `src/fork.cpp` - Creación de procesos mediante `fork()` y ejecución de programas con `exec()`.
 * `src/led_test.cpp` - Acceso a la E/S serial en UNIX. Ejemplo con el control de un LED tricolor conectado a un Arduino.
 * `src/mmap.cpp` - Ficheros mapeados en memoria.
 * `src/pipe.cpp` - Comunicación entre procesos mediante tuberías.
 * `src/shared-memory-sync.cpp` - Sincronización mediante semáforos con memoria compartida entre procesos.
 * `src/shared-memory.cpp` - Comunicación mediante memoria compartida
 * `src/softstack.c` - Ejemplo básico en C para mostrar la pila de software con `strace`.
 * `src/softstack.cpp` - Ejemplo básico en C++ para mostrar la pila de software con `strace`.
 * `src/threads.cpp` - Creación de hilos en C++.
 * `src/threads-mutex.cpp` - Sincronización de hilos mediante mutex.
 * `src/threads-pthread.cpp` - Creación de hilos mediante POSIX pthread.
 * `src/threads-mutex-pthread.cpp` - Sincronización de hilos mediante mutex de POSIX pthread.
 * `src/yash.cpp` - Ejemplo muy básico del funcionamiento interno de una shell.


### Requisitos

Para la compilación de estos ejemplos es necesario disponer de los siguientes paquetes:

 * `g++`
 * `libc-dev`, `libstdc++-dev`
 * `cmake`, `make`


### Compilación

Los ejemplos se compilan de manera sencilla siguiendo estos pasos:

 1. Ir al subdirectorio `build/` dentro del directorio del proyecto.
 2. Ejecutar `cmake ..`
 3. Ejecutar `make`

Los ejecutables resultantes de la compilación son almacenados en la carpeta
`bin/`.


### Autor y contacto

Si tiene cualquier cuestión no dude en ponerse en contacto con el [autor](http://jmtorres.webs.ull.es/).
