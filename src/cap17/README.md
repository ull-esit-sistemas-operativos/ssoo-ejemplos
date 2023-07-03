# Archivos mapeados en memoria

El mapeo de archivos en memoria es una técnica eficiente que permite a los programas acceder a los datos en un archivo como si fueran una matriz en memoria.
Esto se realiza con las llamadas al sistema `mmap()` y `munmap()`.

## Uso de mmap()

La función `mmap()` mapea un archivo en el espacio de direcciones de un proceso.
Aquí se muestra un ejemplo de cómo mapear un archivo en memoria:

```c
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Primero se abre el archivo, con los permisos adecuados.
int fd = open("archivo.txt", O_RDONLY);
if (fd < 0) {
    fprintf(stderr, "Fallo al abrir el archivo: %s\n", strerror(errno));
    return 1;
}

// Se usa fstat() para obtener el tamaño del archivo porque a mmap() hay que indicarle
// el tamaño de la porción del archivo que queremos mapear.
struct stat st;
if (fstat(fd, &st) < 0) {
    fprintf(stderr, "Fallo en fstat: %s\n", strerror(errno));
    return 1;
}

void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
if (map == MAP_FAILED) {
    fprintf(stderr, "Fallo en mmap: %s\n", strerror(errno));
    return 1;
}

// Ahora se puede acceder a los datos del archivo como si estuvieran en la memoria:
// Por ejemplo, imprimir los primeros 10 caracteres por la consola.
for (int i = 0; i < 10 && i < st.st_size; i++) {
    printf("%c", ((char*)map)[i]);
}
```

## Uso de munmap()

Una vez que hayas terminado de trabajar con el mapeo en memoria, se debe liberar con `munmap()`:

```c
if (munmap(map, st.st_size) < 0) {
    fprintf(stderr, "Fallo en munmap: %s\n", strerror(errno));
    return 1;
}
```

Y tampoco se debe olvidar cerrar el descriptor del archivo cuando se haya terminado:

```c
if (close(fd) < 0) {
    fprintf(stderr, "Fallo al cerrar el archivo: %s\n", strerror(errno));
    return 1;
}
```

## Ejemplos

El archivo [mmapped-files.c](mmapped-files.c) contiene un ejemplo del uso de `mmap()` y `munmap()` para hacer la copia de un archivo.
El archivo [mmapped-files.cpp](mmapped-files.cpp) contiene un ejemplo similar pero en C++, usando la clase `memory_map` definida en [memory_map.hpp](memory_map.hpp)