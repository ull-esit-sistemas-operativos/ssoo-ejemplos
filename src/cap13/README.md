# Hilos

Un **hilo** es una secuencia de instrucciones que se ejecuta en la CPU dentro de un proceso.
Todos los hilos de un proceso comparten su espacio de direcciones y sus recursos —memoria, archivos abiertos, señales— pero cada uno tiene su propia pila, su propio contador de programa y su propio juego de registros.

Los ejemplos de este directorio están en dos versiones: resueltos con la librería estándar de C++ y con [POSIX Threads](https://man7.org/linux/man-pages/man7/pthreads.7.html) (la librería de hilos de los sistemas POSIX).
Compararlos es la forma más rápida de ver qué trabajo hace por nosotros la librería estándar del lenguaje.

## Crear hilos y esperar a que terminen

En [`threads.cpp`](threads.cpp) y [`pthreads.cpp`](pthreads.cpp) el hilo principal crea tres hilos, les pasa un identificador a cada uno y espera a que terminen.

La diferencia está en cómo se le pasan los argumentos a la **función principal** del hilo.
En C++ se indican con su tipo, detrás del nombre de la función, al construir el objeto [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread):

```cpp
std::thread thread1( thread_function, 1 );
```

En POSIX Threads la función principal recibe y devuelve un `void*`, así que cuando hacen falta varios argumentos —o hace falta devolver algo— se agrupan en una estructura y se pasa su dirección:

```cpp
struct thread_args
{
    int id;
    int result;
};

thread_args thread1_args { .id = 1 };
pthread_create( &thread1, nullptr, thread_function, &thread1_args );
```

Dentro de la función principal hay que recuperar el puntero al tipo original con un `static_cast`.
Y hay que asegurarse de que la estructura siga existiendo mientras el hilo la use --en este ejemplo vive en `main()`--, que no retorna hasta haber esperado a los tres hilos.

Conviene fijarse en tres detalles de [`pthread_create()`](https://man7.org/linux/man-pages/man3/pthread_create.3.html):

- El primer argumento es la dirección de una variable `pthread_t`, donde la función deja el manejador del nuevo hilo.
  Ese manejador es lo que hay que pasar al resto de las funciones de la librería para indicar sobre qué hilo se quiere operar.
  No es el equivalente al PID de los procesos.
  Si el sistema implementa los hilos en el núcleo, cada hilo tendrá además un identificador propio para el sistema, pero POSIX Threads no ofrece ninguna forma portable de obtenerlo.
  Un hilo puede consultar su propio manejador con [`pthread_self()`](https://man7.org/linux/man-pages/man3/pthread_self.3.html).
- El segundo argumento —aquí `nullptr`— es opcional y sirve para indicar atributos del nuevo hilo, como el tamaño y la posición de su pila o su política de planificación.
- La función devuelve 0 si el hilo se pudo crear.
  No usa `errno`, sino que devuelve directamente el código de error, al contrario que la mayoría de las funciones de la librería del sistema.

Para esperar a que un hilo termine se usa [`pthread_join()`](https://man7.org/linux/man-pages/man3/pthread_join.3.html), que además recibe a través de su segundo argumento el puntero que devolvió la función principal del hilo:

```cpp
int* thread1_result;
pthread_join( thread1, reinterpret_cast<void**>(&thread1_result) );
```

Como lo que se devuelve es un puntero, **no se puede devolver la dirección de una variable local** de la función principal, pues se destruye cuando el hilo termina y el puntero deja de ser válido.
Por eso los ejemplos devuelven la dirección de un campo de la estructura de argumentos, que vive en `main()`.

Cuando el resultado del hilo no interesa, por lo que no se piensa esperar por él, se lo marca como separado con [`pthread_detach()`](https://man7.org/linux/man-pages/man3/pthread_detach.3.html) o [`std::thread::detach()`](https://en.cppreference.com/w/cpp/thread/thread/detach), y entonces es el sistema quien los libera automáticamente.

Es interesante comentar las llamadas a `join()` y volver a ejecutar los programas.
La versión con POSIX Threads termina sin esperar y mata a los hilos a media ejecución, mientras que la versión en C++ aborta con `std::terminate()`, porque destruir un objeto `std::thread` que no ha sido ni unido ni separado es un error.

## Repartir un cálculo entre varios hilos

En [`threads-factorial.cpp`](threads-factorial.cpp) y [`pthreads-factorial.cpp`](pthreads-factorial.cpp) se calcula el factorial del número que indique el usuario, repartiendo el trabajo entre dos hilos: uno multiplica desde _N_ hasta _N_/2 y el otro desde _N_/2 - 1 hasta 2.
Cuando ambos terminan, el hilo principal multiplica los dos resultados parciales.

El cálculo usa la clase `BigInt` de [`../../lib/BigInt`](../../lib/BigInt), para que el resultado no se desborde, y las funciones comunes de [`../common/bigint-factorial.hpp`](../common/bigint-factorial.hpp).

## Cancelar hilos

Cancelar un hilo es terminarlo antes de que acabe su trabajo.
En [`jthreads-factorial.cpp`](jthreads-factorial.cpp) y [`pthreads-cancel-factorial.cpp`](pthreads-cancel-factorial.cpp) se calcula otra vez el factorial con dos hilos, pero el hilo principal los cancela si el cálculo tarda más de cinco segundos.
Prueba a introducir un número pequeño y otro muy grande, para ver los dos casos.

Los dos ejemplos resuelven el mismo problema de formas muy distintas.

### Con POSIX Threads

Un hilo pide la cancelación de otro con [`pthread_cancel()`](https://man7.org/linux/man-pages/man3/pthread_cancel.3.html), indicando su manejador:

```cpp
pthread_cancel( thread1 );
```

Eso no termina el hilo inmediatamente.
Por defecto la cancelación es **en diferido**, lo que significa que el hilo solo muere al llegar a un **punto de cancelación**.
Casi todas las llamadas al sistema que pueden dejar al hilo esperando lo son —[`open()`](https://man7.org/linux/man-pages/man2/open.2.html), [`read()`](https://man7.org/linux/man-pages/man2/read.2.html), [`write()`](https://man7.org/linux/man-pages/man2/write.2.html), [`sleep()`](https://man7.org/linux/man-pages/man3/sleep.3.html)…— y la lista completa está en la sección _Cancellation points_ de [`pthreads(7)`](https://man7.org/linux/man-pages/man7/pthreads.7.html).

El bucle que calcula el factorial no llama a ninguna de ellas, así que no tiene ningún punto de cancelación propio. Es decir, por muy largo que fuera el cálculo, el hilo sería incancelable.
Por eso hay que introducir uno a mano con [`pthread_testcancel()`](https://man7.org/linux/man-pages/man3/pthread_testcancel.3.html):

```cpp
for ( BigInt i = lower_bound; i <= number; i++ )
{
    factorial = factorial * i;
    partial_result = factorial;

    pthread_testcancel();
}
```

Es interesante comentar esa llamada y volver a ejecutar el programa con un número grande.
El hilo principal pide la cancelación igual, pero los hilos la ignoran y `pthread_join()` no retorna hasta que terminan todo el cálculo.

La otra posibilidad es la **cancelación asíncrona**, que se activa con [`pthread_setcanceltype()`](https://man7.org/linux/man-pages/man3/pthread_setcanceltype.3.html) y hace que el hilo muera en cualquier instante.
Solo es segura en código que no reserva recursos ni toca datos compartidos, porque si el hilo muere en mitad de una reserva de memoria o de la modificación de una estructura de datos, la operación queda a medias.
Un hilo también puede desactivar temporalmente su propia cancelación con [`pthread_setcancelstate()`](https://man7.org/linux/man-pages/man3/pthread_setcancelstate.3.html), mientras hace algo que no se debe interrumpir.

Un hilo cancelado no ejecuta ni una instrucción más, así que **no llega a ejecutar la sentencia `return` de su función principal**.
Por eso `pthread_join()` no devuelve su resultado, sino el valor especial `PTHREAD_CANCELED`:

```cpp
void* thread1_retval;
pthread_join( thread1, &thread1_retval );

if (thread1_retval == PTHREAD_CANCELED) { /* ... */ }
```

Por la misma razón, un hilo cancelado tampoco cierra los archivos que tuviera abiertos, ni libera la memoria que hubiera reservado, ni —en C++— destruye sus objetos locales.
Para eso POSIX Threads ofrece una pila de manejadores de limpieza, que se manipula con [`pthread_cleanup_push()`](https://man7.org/linux/man-pages/man3/pthread_cleanup_push.3.html) y `pthread_cleanup_pop()`, y que la librería ejecuta antes de dar por terminado al hilo.
Estos ejemplos no reservan ningún recurso, así que no los necesitan.

### Con `std::jthread`

C++ incorporó un mecanismo de cancelación al estándar en C++20 que no sigue el modelo de cancelación de POSIX.
En lugar de matar al hilo, [`std::jthread`](https://en.cppreference.com/w/cpp/thread/jthread) le pasa a su función principal un **_token_ de cancelación** de tipo [`std::stop_token`](https://en.cppreference.com/w/cpp/thread/stop_token), que el hilo comprueba de vez en cuando:

```cpp
for ( BigInt i = lower_bound; i <= number; i++ )
{
    if(stoken.stop_requested())
    {
        return factorial;
    }
    factorial = factorial * i;
}
```

Es lo que se denomina **cancelación cooperativa**, pues el hilo no muere de repente, sino que **retorna normalmente**.
Así devuelve su resultado parcial como cualquier otro valor de retorno, se destruyen sus objetos locales y se libera lo que tuviera reservado, sin necesidad de manejadores de limpieza.

La otra diferencia está en el destructor.
El de `std::jthread` pide la cancelación del hilo con [`request_stop()`](https://en.cppreference.com/w/cpp/thread/jthread/request_stop) y luego espera a que termine antes de destruirse.
El de `std::thread`, en cambio, aborta el programa si no se ha llamado a `join()`, para esperar a que termine el hilo, o a `detach()`, para separarlo.
