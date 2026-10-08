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

El archivo [fork.cpp](posix/fork.cpp) contiene un ejemplo del uso de `fork()`. 

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

El archivo [fork-exec.cpp](posix/fork-exec.cpp) contiene un ejemplo del uso de `fork()` y `exec()` para ejecutar otro proceso con otro programa.

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

### Terminación de un proceso hijo

Lo anterior es cierto para terminar un programa, pero no para el proceso hijo creado con `fork()`, que no está terminando un programa sino descartando una copia del programa del proceso padre.
Ese hijo debe salir siempre con [`_exit()`](https://manpages.debian.org/stretch/manpages-es/_exit.2.es.html) —o con `std::_Exit()`, su equivalente en C++—, nunca con `return` ni con `exit()`.

Esto solo es un problema mientras el hijo ejecuta el mismo programa que el padre.
Si carga otro programa con `exec()`, en cuanto la llamada tiene éxito la imagen del proceso se sustituye entera y el problema desaparece.
Pero sigue siendo importante tenerlo en cuenta en el código que va desde el `fork()` hasta el `exec()`, y también si esta última falla.

El motivo es que `exit()` ejecuta antes las operaciones de cierre del programa: las funciones registradas con `atexit()`, los destructores de los objetos globales y el vaciado de los búferes de entrada y salida.
El hijo tiene una copia de todo ese estado, pero los efectos de esas operaciones salen del proceso, porque actúan sobre recursos que comparte con el padre.
Por ejemplo, el hijo hereda una copia de lo que el padre tuviera pendiente de escribir en la salida estándar y, al vaciarla, esos mensajes aparecen por duplicado.

```cpp
pid_t pid = fork();
if (pid == 0)
{
    // En el proceso hijo
    _exit(42);      // Nunca return ni exit()
}
```

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
    _exit(42);
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
    _exit(127);
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

Pero si `execl()` falla, el valor de `child_exit_status` será 127, que es el valor que se pasa a `_exit()` en caso de error.
Se usa 127 por seguir la convención de la _shell_, que usa este valor para informar de que no encontró el comando, y así no se confunde con los códigos de salida que puede devolver el programa ejecutado.

## Redirección de la E/S estándar

Los descriptores de archivo abiertos de un proceso se heredan al crear un proceso hijo con `fork()` y se conservan al cambiar de programa con `exec()`.
Gracias a eso, el código que se ejecuta en el hijo entre `fork()` y `exec()` puede cambiar a dónde apuntan la entrada, la salida y la salida de error estándar, y el programa ejecutado las usará sin saberlo.
Así es como la _shell_ implementa redirecciones como `ls -l > salida.txt`.

### Duplicar descriptores de archivo

Los descriptores de archivo se pueden copiar, haciendo que varios descriptores de archivo apunten al mismo archivo o recurso.

Para hacerlo se utiliza la función [`dup2()`](https://manpages.debian.org/stretch/manpages-es/dup.2.es.html), que copia el descriptor de archivo `oldfd` en el descriptor de archivo `newfd`.

```c
int dup2(int oldfd, int newfd);
```

Si `newfd` ya estaba abierto, se cierra antes de copiar el descriptor de archivo.
Y si `dup2()` tiene éxito, devuelve `newfd`.

Por ejemplo, si se abre un archivo y se copia el descriptor de archivo en el 42, ahora el archivo tiene dos descriptores de archivo abiertos, uno en el valor de `fd` y otro en 42.

```cpp
int fd = open("archivo.txt", O_RDONLY);
if (fd < 0)
{
    // Error al abrir el archivo...
}

// Copiar el descriptor de archivo en el 42
int fd2 = dup2(fd, 42);

// Leer datos del archivo con el descriptor de archivo 42
// Hubiera sido lo mismo usar fd2, ya que es igual a 42 al volver de dup2()
std::array<char, 1024> buffer;
ssize_t bytes_read = read(42, buffer.data(), buffer.size());
if (bytes_read < 0)
{
    // Error al leer del archivo...
}

close(fd);
close(42);
```

Ambos descriptores comparten la misma posición en el archivo, por lo que leer o escribir con uno de ellos avanza también la del otro.

### Redirección de la E/S estándar a un archivo

Los tres primeros descriptores de archivo de un proceso tienen un significado especial:

| Descriptor | Constante | Uso |
| --- | --- | --- |
| 0 | `STDIN_FILENO` | Entrada estándar |
| 1 | `STDOUT_FILENO` | Salida estándar |
| 2 | `STDERR_FILENO` | Salida de error estándar |

Por eso, si con `dup2()` se copia el descriptor de un archivo sobre uno de ellos, se redirige la E/S estándar del proceso a ese archivo.
Por ejemplo, para redirigir la salida estándar de un proceso a un archivo:

```cpp
int fd = open("salida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
if (fd < 0)
{
    // Error al abrir el archivo...
}

// Redirigir la salida estándar al archivo
dup2(fd, STDOUT_FILENO);

// Ya no necesitamos el descriptor original. El archivo sigue abierto a través de STDOUT_FILENO.
close(fd);

std::println("Este mensaje se escribirá en el archivo 'salida.txt'");
```

Se puede hacer lo mismo para redirigir la entrada estándar del proceso desde un archivo, duplicando el descriptor de archivo sobre `STDIN_FILENO`, o la salida de error estándar a un archivo, duplicándolo sobre `STDERR_FILENO`.

Es importante que el archivo se abra en el modo adecuado para la operación que se va a realizar.
Si se va a redirigir la salida estándar o de error a un archivo, el archivo debe abrirse en modo escritura, para poder escribir en él la salida del programa.
Si se va a redirigir la entrada estándar desde un archivo, el archivo debe abrirse en modo lectura, para poder leer de él la entrada del programa.

### Redirección junto con fork() y exec()

Lo habitual es hacer la redirección en el proceso hijo, justo antes de llamar a `exec()`.
Así solo se cambia la E/S estándar del hijo, mientras que la del padre queda intacta:

```cpp
pid_t pid = fork();
if (pid == 0)
{
    int fd = open("salida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        _exit(EXIT_FAILURE);
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);

    // 'ls' hereda la salida estándar redirigida y escribe en 'salida.txt'.
    execl("/bin/ls", "ls", "-l", nullptr);

    // Si llegamos aquí, hubo un error al ejecutar exec.
    _exit(127);
}
else
{
    int status;
    waitpid(pid, &status, 0);
}
```

Hay que tener cuidado con los búferes de la librería estándar.
Si el hijo escribe algo en la salida estándar antes de redirigirla y la salida no es una terminal, ese texto puede quedarse en el búfer.
Entonces, o se pierde al llamar a `exec()`, que sustituye la imagen del proceso con sus búferes incluidos, o acaba en el archivo si se vacía después de `dup2()`.
Por eso conviene vaciar el búfer con `std::fflush(stdout)` antes de redirigir la salida.

El archivo [fork-redir.cpp](posix/fork-redir.cpp) contiene un ejemplo completo que ejecuta `ls -l` con su salida estándar redirigida al archivo `salida.txt`.

El mismo mecanismo permite redirigir la E/S estándar de un proceso a una tubería, para que otro proceso lea su salida o le envíe su entrada.
Eso se ve en los ejemplos de tuberías del [capítulo 11](../cap11/tuberías/README.md#redirección-de-la-es-estándar-a-una-tubería).
