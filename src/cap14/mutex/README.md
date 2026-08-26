# Mutex

Un **_mutex_** —de _mutual exclusion_— es un objeto que garantiza que solo un hilo a la vez ejecute la porción de código que manipula un recurso compartido, es decir, su **sección crítica**.

El hilo que quiere entrar en la sección crítica adquiere el _mutex_ y lo libera al salir de ella.
Si otro hilo intenta adquirirlo mientras tanto, queda bloqueado hasta que el primero lo libere.
Todos los hilos que acceden al recurso deben usar el mismo _mutex_: la protección no está en el dato, sino en el acuerdo entre los hilos que lo comparten.

Aquí hay dos ejemplos, cada uno en dos versiones: una con los _mutex_ de [POSIX Threads](https://man7.org/linux/man-pages/man7/pthreads.7.html) —[`pthread_mutex_lock()`](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) y [`pthread_mutex_unlock()`](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)— y otra con [`std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex) de la librería estándar de C++.

## Un contador compartido

En [`pthreads-sync-counter.cpp`](pthreads-sync-counter.cpp) y [`threads-sync-counter.cpp`](threads-sync-counter.cpp) dos hilos incrementan un millón de veces cada uno el mismo contador.

Sin protección, el resultado final casi nunca es el esperado, porque `counter++` no es una operación indivisible: se descompone en leer el valor, incrementarlo y volver a guardarlo, y ambos hilos pueden entrelazar esos pasos perdiendo incrementos por el camino.
El resultado es correcto siempre que cada hilo adquiera el mismo _mutex_ antes del incremento:

```cpp
// Bloquear el mutex antes de incrementar el contador.
pthread_mutex_lock( &args->mutex );
args->counter++;
// Desbloquear el mutex tras incrementar el contador.
pthread_mutex_unlock( &args->mutex );
```

En la versión en C++ el _mutex_ no se bloquea ni se desbloquea a mano, sino que se construye un [`std::lock_guard`](https://en.cppreference.com/w/cpp/thread/lock_guard) que lo adquiere al crearse y lo libera al destruirse, cuando la ejecución sale del ámbito.
Así es más difícil olvidarse de liberarlo.

Es interesante comentar las llamadas al _mutex_ y volver a ejecutar el programa, para comprobar que el valor final del contador deja de ser el correcto.

## Un vector de resultados parciales

En [`pthreads-sync-factorial.cpp`](pthreads-sync-factorial.cpp) y [`threads-sync-factorial.cpp`](threads-sync-factorial.cpp) se calcula el factorial del número que indique el usuario, repartiendo el trabajo entre dos hilos: uno multiplica desde _N_ hasta _N_/2 y el otro desde _N_/2 - 1 hasta 2.

A diferencia de los ejemplos del capítulo anterior, los hilos no devuelven su resultado, sino que lo guardan en un `std::vector` compartido:

```cpp
struct factorial_thread_results
{
    pthread_mutex_t mutex;
    std::vector<BigInt> partials;
};
```

Como el vector lo modifican ambos hilos, hay que protegerlo con el _mutex_ que lo acompaña antes de añadirle nada:

```cpp
// Bloquear el mutex y guardar el resultado
pthread_mutex_lock( &args->results->mutex );
args->results->partials.push_back( result );
pthread_mutex_unlock( &args->results->mutex );
```

No basta con que `push_back()` sea una sola llamada.
Para añadir un elemento, el vector tiene que leer y actualizar la dirección donde termina la parte ocupada y, si el elemento no cabe, reservar un bloque de memoria mayor, mover allí lo que ya tenía y liberar el bloque antiguo.
Si dos hilos hacen eso a la vez, pueden construir su elemento en el mismo hueco, liberar dos veces el mismo bloque o dejar posiciones sin construir.

Cuando ambos hilos terminan, el hilo principal recorre el vector multiplicando los resultados parciales para obtener el factorial final.
Ahí ya no hace falta sincronización, porque [`pthread_join()`](https://man7.org/linux/man-pages/man3/pthread_join.3p.html) —o `std::thread::join()`— garantiza que los hilos han terminado y nadie más está tocando el vector.
