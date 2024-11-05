# Operaciones sobre los procesos

## Uso de fork()

La función [`fork()`](https://manpages.debian.org/stretch/manpages-es/fork.2.es.html) se utiliza para crear un nuevo proceso.
El proceso creado es una copia exacta del proceso que llamó a `fork()`, pero tiene su propio espacio de direcciones.

```cpp
pid_t pid = fork();

if (pid < 0)
{
    // Error al crear el proceso.
}
else if (pid == 0)
{
    // Este bloque se ejecutará en el proceso hijo.
}
else
{
    // Este bloque se ejecutará en el proceso padre.
}
```

El archivo [fork.c](fork.c) contiene un ejemplo del uso de `fork()`. 

## Uso de exec()

La función [`exec()`](https://manpages.debian.org/stretch/manpages-es/exec.3.es.html) se utiliza para reemplazar la imagen del proceso actual con un nuevo programa.
Esta función solo retorna de su invocación si ha ocurrido un error.

```cpp
if (pid < 0)
{
    // Error al crear el proceso.
}
else if (pid == 0)
{
    // En el proceso hijo, ejecutamos otro programa.
    execl("/bin/ls", "ls", "-l", "/etc", nullptr);

    // Si llegamos aquí, hubo un error al ejecutar exec.
}
else
{
    // Este bloque se ejecutará en el proceso padre.
}
```

El archivo [fork-exec.c](fork-exec.c) contiene un ejemplo del uso de `fork()` y `exec()` para ejecutar otro proceso con otro programa.

### Variantes de exec()

Existen distintas variantes de la función `exec()` que permiten pasar argumentos al programa que se va a ejecutar de distintas formas.

- `execl()`: Permite pasar los argumentos al programa como una lista de argumentos separados por coma.
    Esta es la versión que utilizamos en el ejemplo anterior.

    ```cpp
    execl("/bin/ls", "ls", "-l", "/etc", nullptr);
    ```

- `execv()`: Permite pasar los argumentos al programa como un array de cadenas de caracteres.
    Esta versión es útil cuando el número de argumentos no se conoce en el momento de compilar.

    ```cpp
    char *args[] = { "ls", "-l", "/etc", nullptr };
    execv("/bin/ls", args);
    ```

- `execle()`: Permite pasar los argumentos al programa como una lista de argumentos separados por coma y también permite pasar variables de entorno.

    ```cpp
    char *env[] = { "LANG=en_US.UTF-8", nullptr };
    execle("/bin/ls", "ls", "-l", "/etc", nullptr, env);
    ```

- `execve()`: Permite pasar los argumentos al programa como un array de cadenas de caracteres y también permite pasar variables de entorno.

    ```cpp
    char *args[] = { "ls", "-l", "/etc", nullptr };
    char *env[] = { "LANG=en_US.UTF-8", nullptr };
    execve("/bin/ls", args, env);
    ```

- `execlp()`: Si no se especifica la ruta completa del programa en el primer argumento, solo el nombre del programa, el sistema buscará en los directorios del `PATH` el programa a ejecutar.

    ```cpp
    execlp("ls", "ls", "-l", "/etc", nullptr);
    ```

- `execvp()`: Permite buscar el programa en el `PATH` del sistema y pasar los argumentos al programa como un array de cadenas de caracteres.

    ```cpp
    char *args[] = { "ls", "-l", "/etc", nullptr };
    execvp("ls", args);
    ```

En el mismo sentido, también existen las funciones `execvpe()` y `execlpe()` que permiten pasar variables de entorno, al tiempo que permiten buscar el programa en el `PATH` del sistema.

En todos los casos, el último argumento debe ser `nullptr`, que indica el final de la lista de argumentos.

## Terminación del programa

Para terminar un programa, simplemente usamos la función `return` desde la función `main()`.
También podemos usar la función `exit()` desde cualquier parte del programa para terminarlo inmediatamente.

```cpp
if (alguna_condicion_de_error) {
    std::exit(1);   // Termina el programa con un código de error 1.
}
```

Como veremos a continuación, el valor pasado a `exit()` o `return` se puede leer en el proceso padre al esperar a que termine el proceso hijo.

## Uso de wait() y waitpid()

Las funciones [`wait()`](https://manpages.debian.org/stretch/manpages-es/wait.2.es.html) y [`waitpid()`](https://manpages.debian.org/stretch/manpages-es/waitpid.2.es.html) se utilizan para hacer que el proceso padre espere a que termine uno de sus procesos hijos.

```cpp
pid_t pid = fork();
if (pid < 0)
{
    // Error al crear el proceso.
}
else if (pid == 0)
{
    // En el proceso hijo, ejecutamos otro programa.
    execl("/bin/ls", "ls", "-l", "/etc", nullptr);

    // Si llegamos aquí, hubo un error al ejecutar exec.
} else {
    int status;

    // En el proceso padre, esperamos a que termine el hijo.
    wait(&status);
}
```

La función `wait()` pone al proceso en espera y solo retorna cuando termina alguno de los procesos hijos del proceso que la llamó.

Para esperar a un proceso hijo específico, podemos usar `waitpid()`.

```cpp
#include <sys/wait.h>

pid_t pid = fork();
if (pid == 0)
{
    // En el proceso hijo
} else {
    int status;
    // En el proceso padre, esperamos a que termine el hijo con PID `pid`.
    waitpid(pid, &status, 0);
}
```

Al volver de `wait()` o `waitpid()`, el valor de `status` contiene información sobre la terminación del proceso hijo.

Por ejemplo, podemos usar la macro `WIFEXITED()` para saber si el proceso hijo terminó normalmente.
Y la macro `WEXITSTATUS()` para obtener el valor de retorno del proceso hijo.

```cpp
#include <sys/wait.h>

pid_t pid = fork();
if (pid == 0)
{
    // Hacemos terminar el proceso hijo con el código de salida 42.
    std::exit(42);
}
else
{
    int status;
    // En el proceso padre, esperamos a que termine el hijo con PID `pid`.
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))  // true si el hijo terminó normalmente.
    {
        // Obtenemos el valor de retorno del proceso hijo.
        int child_exit_status = WEXITSTATUS(status);
        std::println("El proceso hijo terminó con el código de salida: {}", child_exit_status);
    }
}
```

Por ejemplo, se puede ejecutar el comando `ls`, esperar a que termine y obtener su código de salida:

```cpp
pid_t pid = fork();
if (pid == 0)
{
    execl("/bin/ls", "ls", "-l", "/etc", nullptr);

    // Si llegamos aquí, hubo un error al ejecutar exec.
    std::exit(128);
}
else
{
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
    {
        int child_exit_status = WEXITSTATUS(status);
        std::println("'ls' terminó con el código de salida: {}", child_exit_status);
    }
}
```

En el ejemplo anterior, si el comando `ls` se puede ejecutar, el valor de `child_exit_status` será el código de salida del comando `ls`.
Por lo general, el código de salida será 0 si el comando se ejecutó correctamente y otro valor si hubo algún error.

Pero si `execl()` falla, el valor de `child_exit_status` será 128, que es el valor que se pasa a `exit()` en caso de error.