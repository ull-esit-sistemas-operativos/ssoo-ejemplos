# Semáforos

Un **semáforo** es un objeto con un contador interno y dos operaciones, _acquire_ y _release_.
_Acquire_ decrementa el contador si es mayor que 0 y, si vale 0, bloquea al hilo que la invoca hasta que otro hilo llame a _release_, que incrementa el contador y despierta a uno de los hilos que esperaban.
Así, un semáforo inicializado a _N_ solo deja que _N_ hilos ejecuten a la vez el código que protege.

Un semáforo inicializado a 1 proporciona **exclusión mutua**, igual que un _mutex_.
Pero, a diferencia del _mutex_, no tiene que liberarlo el mismo hilo que lo adquirió, así que también sirve para que un hilo avise a otro de que ha ocurrido algo, cubriendo el caso de la **sincronización condicional**.

Tanto POSIX —con [`sem_init()`](https://man7.org/linux/man-pages/man3/sem_init.3.html), [`sem_wait()`](https://man7.org/linux/man-pages/man3/sem_wait.3.html) y [`sem_post()`](https://man7.org/linux/man-pages/man3/sem_post.3.html)— como la librería estándar de C++ desde C++20 —con [`std::counting_semaphore`](https://en.cppreference.com/w/cpp/thread/counting_semaphore)— ofrecen semáforos.
Pero, si un sistema o un lenguaje no los tuviera, implementarlos es sencillo cuando se dispone de un _mutex_ y una variable de condición.
Eso es justo lo que hacemos en este ejemplo.

## Semáforo hecho con un _mutex_ y una variable de condición

En [`semaphore.hpp`](semaphore.hpp) está la clase `examples::semaphore`, que guarda el contador del semáforo en un miembro `count_` protegido por el _mutex_ `mutex_`, y usa la variable de condición `cv_` para dormir a los hilos que no pueden entrar:

```cpp
class semaphore {
public:

    semaphore(int count = 0) : count_(count) {}

    void release() {
        std::unique_lock<std::mutex> lock(mutex_);
        count_++;
        cv_.notify_one();
    }

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        while(count_ == 0) {
            cv_.wait(lock);
        }
        count_--;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};
```

En `acquire()`, si el contador está a 0 no pueden entrar más hilos, así que el hilo se pone a esperar en `cv_`.
Mientras espera, el _mutex_ queda liberado, porque de lo contrario ningún otro hilo podría entrar a incrementar el contador y todos quedarían bloqueados indefinidamente.
La espera está dentro de un `while` y no de un `if` porque el estándar de C++ permite que `wait()` retorne sin que haya habido notificación —lo que se conoce como **despertar espurio**—, así que hay que volver a comprobar la condición antes de continuar.

`release()` incrementa el contador y despierta a uno de los hilos que estuvieran esperando, que saldrá de su `wait()` tras readquirir el _mutex_.

## Limitar los hilos que entran en una sección crítica

En [`threads-sync-semaphore.cpp`](threads-sync-semaphore.cpp) se usa la clase para permitir que solo tres de los nueve hilos que crea el programa estén dentro de la sección crítica al mismo tiempo:

```cpp
void thread_function(examples::semaphore& sem, int thread_id)
{
    sem.acquire();
    std::println( "Hilo {} creado", thread_id );

    // Dormir el hilo para simular trabajo
    std::this_thread::sleep_for( std::chrono::seconds(2) );

    std::println( "Hilo {} terminado", thread_id );
    sem.release();
}
```

Al ejecutarlo se ve el efecto con claridad: los mensajes aparecen en tandas de tres, separadas por dos segundos, porque hasta que uno de los tres hilos que están dentro no llama a `release()`, ninguno de los que esperan en `acquire()` puede continuar.
