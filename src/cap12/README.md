# Memoria compartida

La **memoria compartida** es un mecanismo de comunicación entre procesos que consiste en que dos o más de ellos mapeen en su espacio de direcciones la misma región de memoria física.

A diferencia del paso de mensajes, aquí el sistema operativo solo interviene al crear la región y al mapearla.
A partir de ese momento los procesos leen y escriben en ella como en cualquier otra variable, sin llamadas al sistema por cada dato intercambiado, lo que la convierte en el mecanismo de comunicación más rápido.

Ese es también su inconveniente: como el sistema operativo ya no se entera de los accesos, no puede ordenarlos.
Nada impide que un proceso lea la región mientras otro la está escribiendo, así que la sincronización corre por cuenta de los programas, que aquí la resuelven con [semáforos](https://man7.org/linux/man-pages/man7/sem_overview.7.html).

Hay dos formas de crear la región, y en este directorio hay un ejemplo de cada una.

## Memoria compartida anónima

Una región **anónima** no tiene nombre en el sistema, así que solo pueden acceder a ella los procesos que la hereden de quien la creó.
En la práctica, eso significa reservarla antes de [`fork()`](https://man7.org/linux/man-pages/man2/fork.2.html), para que el proceso hijo la reciba junto con el resto del espacio de direcciones del padre.

En [`anom-shared-memory.cpp`](anom-shared-memory.cpp) el programa pide un número al usuario, lanza un proceso hijo para que calcule su factorial y lee el resultado de la región compartida.
La región se reserva con [`mmap()`](https://man7.org/linux/man-pages/man2/mmap.2.html), pasando `MAP_ANONYMOUS | MAP_SHARED` y `-1` como descriptor de archivo, porque no hay ningún archivo detrás:

```cpp
void* shared_mem = mmap(
    nullptr,
    sizeof(memory_content),
    PROT_READ | PROT_WRITE,
    MAP_ANONYMOUS | MAP_SHARED,
    -1,
    0 );
```

En la región se coloca una estructura con el hueco para el resultado y un semáforo para sincronizarse:

```cpp
struct memory_content
{
    sem_t ready;        // Semáforo para indicar al padre cuándo está listo el resultado
    int factorial;      // Para guardar el resultado de calcular el factorial
};
```

El padre no puede leer `factorial` en cuanto crea al hijo, porque el cálculo tarda y leería un valor sin escribir.
Por eso espera en [`sem_wait()`](https://man7.org/linux/man-pages/man3/sem_wait.3.html) hasta que el hijo, tras dejar el resultado en la región, llama a [`sem_post()`](https://man7.org/linux/man-pages/man3/sem_post.3.html).
El semáforo se inicializa con [`sem_init()`](https://man7.org/linux/man-pages/man3/sem_init.3.html) pasando un valor distinto de 0 en el argumento `pshared`, que es lo que indica que va a usarse entre procesos distintos y no entre hilos del mismo proceso.

Conviene fijarse en que el número que introduce el usuario no hace falta meterlo en la región compartida.
El hijo recibe una copia de toda la memoria del padre al hacer `fork()`, así que ya lo tiene; lo que hay que compartir es únicamente lo que se escribe **después** de crear el proceso.

## Memoria compartida con nombre

Una región **con nombre** se identifica con una ruta y es pública para el resto del sistema, así que dos procesos sin ningún parentesco entre ellos pueden abrirla y comunicarse a través de ella.

En [`shared-memory.cpp`](shared-memory.cpp) hay un programa que muestra la hora del sistema periódicamente y que puede ser controlado a distancia por [`shared-memory-control.cpp`](shared-memory-control.cpp), que se une a la misma región para darle órdenes.
Es el mismo ejemplo que en los directorios de tuberías, colas de mensajes y _sockets_ del capítulo anterior, resuelto ahora con memoria compartida.
El código que muestra la hora está en [`../common/timeserver.cpp`](../common/timeserver.cpp) y se comparte entre todos ellos.

Lo que ambos programas tienen que ponerse de acuerdo en conocer —el nombre de la región y la estructura de su contenido— está en [`shared-memory-common.hpp`](shared-memory-common.hpp):

```cpp
struct memory_content
{
    sem_t empty;
    sem_t ready;
    std::array<char, 100> command_buffer;
    size_t command_length;
};
```

Crear la región lleva tres pasos, porque a diferencia del caso anónimo aquí sí hay un objeto con nombre en el sistema:

1. [`shm_open()`](https://man7.org/linux/man-pages/man3/shm_open.3.html) la crea con `O_CREAT | O_EXCL` y devuelve un descriptor de archivo.
2. [`ftruncate()`](https://man7.org/linux/man-pages/man2/ftruncate.2.html) le da el tamaño necesario, porque recién creada tiene tamaño 0.
3. [`mmap()`](https://man7.org/linux/man-pages/man2/mmap.2.html) la mapea en el espacio de direcciones del proceso, esta vez con `MAP_SHARED` y el descriptor devuelto por `shm_open()`.

El programa de control lo tiene más fácil: como la región ya existe y ya tiene su tamaño, le basta con abrirla llamando a `shm_open()` con `O_RDWR` y mapearla con `mmap()`.

Para enviar las órdenes, en `command_buffer` el programa de control copia el comando que quiere mandar, y los dos semáforos ordenan el acceso al búfer:

- `empty` indica que `command_buffer` está libre, así que se inicializa a 1.
  El programa de control espera en él antes de escribir un comando nuevo, y el programa controlado lo incrementa después de leer el que había.
- `ready` indica que hay un comando pendiente de leer, así que se inicializa a 0.
  El programa controlado espera en él antes de leer `command_buffer`, y el programa de control lo incrementa después de escribir.

De momento la única orden que entiende [`shared-memory.cpp`](shared-memory.cpp) es `QUIT`, que le pide que termine, pero no costaría nada añadir otras.

Al terminar, el programa que creó la región la borra del sistema con [`shm_unlink()`](https://man7.org/linux/man-pages/man3/shm_unlink.3.html).
Si no lo hiciera, la región seguiría existiendo después de que el proceso muera —en Linux se puede comprobar listando `/dev/shm`— y el programa no podría volver a arrancar, porque `shm_open()` se llama con `O_EXCL` y fallaría al encontrarla ya creada.
