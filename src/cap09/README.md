# Operaciones sobre los procesos

## Uso de fork()

La función `fork()` se utiliza para crear un nuevo proceso.
El proceso creado es una copia exacta del proceso que llamó a `fork()`, pero tiene su propio espacio de direcciones.

```c
#include <unistd.h>

pid_t pid = fork();

if (pid < 0) {
    // Error al crear el proceso.
} else if (pid == 0) {
    // Este bloque se ejecutará en el proceso hijo.
} else {
    // Este bloque se ejecutará en el proceso padre.
}
```

El archivo [fork.c](fork.c) contiene un ejemplo del uso de `fork()`. 

## Uso de exec()

La función `exec()` se utiliza para reemplazar la imagen del proceso actual con un nuevo programa.
La función `exec()` solo retorna de su invocación si hay un error.

```c
#include <unistd.h>

if (pid < 0) {
    // Error al crear el proceso.
} else if (pid == 0) {
    // En el proceso hijo, ejecutamos otro programa.
    execl("/path/to/program", "program", (char *)NULL);

    // Si llegamos aquí, hubo un error al ejecutar exec.
} else {
    // Este bloque se ejecutará en el proceso padre.
}
```

El archivo [fork-exec.c](fork-exec.c) contiene un ejemplo del uso de `fork()` y `exec()` para ejecutar otro proceso con otro programa. 

## Uso de wait() y waitpid()

Las funciones `wait()` y `waitpid()` se utilizan para hacer que el proceso padre espere a que termine uno de sus procesos hijos.

```c
#include <sys/wait.h>
#include <unistd.h>

int status;

if (pid < 0) {
    // Error al crear el proceso.
} else if (pid == 0) {
    // En el proceso hijo, ejecutamos otro programa.
    execl("/path/to/program", "program", (char *)NULL);

    // Si llegamos aquí, hubo un error al ejecutar exec.
} else {
    // En el proceso padre, esperamos a que termine el hijo.
    wait(&status);
}
```

La función `wait()` retorna cuando termina alguno de los procesos hijos del procesos que la llamó.
Para esperar a un proceso hijo específico, podemos usar `waitpid()`.

```c
#include <sys/wait.h>

pid_t pid = fork();

if (pid == 0) {
    // En el proceso hijo
} else {
    int status;
    // En el proceso padre, esperamos a que termine el hijo específico.
    waitpid(pid, &status, 0);
}
```

## Terminación del programa

Para terminar un programa, simplemente usamos la función `return` desde la función `main()`.
También podemos usar la función `exit()` desde cualquier parte del programa para terminarlo inmediatamente.

```c
#include <stdlib.h>

if (alguna_condicion_de_error) {
    exit(1);  // Termina el programa con un código de error.
}
```

El valor indicado en `exit()` o `return` lo puede leer el padre al volver de `wait()` o `waitpid()` usando la macro `WEXITSTATUS()` sobre el valor  de `status`

```c
#include <sys/wait.h>

pid_t pid = fork();

if (pid == 0) {
    // En el proceso hijo
} else {
    int status;
    // En el proceso padre, esperamos a que termine el hijo específico.
    waitpid(pid, &status, 0);
    int child_exit_status = WEXITSTATUS(status);
}
```

## Errores en las llamadas al sistema

Cualquiera de las anteriores llamadas al sistema puede fallar.
Para manejar errores en las llamadas al sistema, normalmente comprobamos el valor de retorno de la llamada al sistema.
Si es negativo, es que ocurrió un error y la variable `errno` contiene el código de error.

```c
#include <errno.h
#include <stdio.h>
#include <unistd.h>

pid_t pid = fork();

if (pid < 0) {
    fprintf(stderr, "Fallo en fork: %s\n", strerror(errno));
    return 1;  // Termina el programa con un código de error.
}
```

La función `strerror()` permite obtener un mensaje descriptivo del código de error indicado.