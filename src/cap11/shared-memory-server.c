// shared-memory-server.c - Servidor del ejemplo del uso de memoria compartida con nombre para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  crea una región de memoria compartida a la que puede unirse el programa cliente para darle órdenes.
//
//  Los problemas de sincronización no son sencillos. Para simplificar, vamos a suponer que solo
//  hay un cliente conectado al mismo tiempo. Así solo necesitaremos dos semáforos.
//
//  Compilar:
//
//      gcc -I../ -lrt -pthread -o shared-memory-server shared-memory-server.c ../common/timeserver.c
//

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>
#include <sys/mman.h>   // Cabecera para mmap() y shm_open()    
#include <sys/types.h>
#include <sys/stat.h>

#include "common/timeserver.h"
#include "shared-memory-server.h"

int create_control_shm(int* controlfd);
void cleanup_control_shm();

int init_control_memory(int controlfd, struct memory_content** memory_region);
void free_control_memory(struct memory_content* memory_region);

int main()
{
    int exit_code = 0;
    bool shm_created = false;
    bool memory_allocated = false;

    int controlfd = -1;
    struct memory_content* memory_region;

    // Las variables anteriores y las siguientes estructuras de if son para el manejo de errores en C. Así liberamos
    // recursos y dejamos todo en su sitio al terminar, incluso si es por un error. Por ejemplo, si al salir por un
    // error olvidamos eliminar el objeto de memoria compartida, no podríamos volver a ejecutar el servidor hasta
    // borrarla a mano. En C++ es mejor encapsular los recursos en clases y usar su destructor para liberarlos (RAII).

    if (! exit_code)
    {
        exit_code = create_control_shm(&controlfd);
        if (exit_code == 0)
        {
            shm_created = true;
        }
    }

    if (! exit_code)
    {
        exit_code = init_control_memory(controlfd, &memory_region);
        if (exit_code == 0)
        {
            memory_allocated = true;
        }
    }

    if (! exit_code)
    {
        setup_signals();
        start_alarm();

        printf( "Escuchando en el canal de control '%s'...\n", CONTROL_SHM_NAME);

        // Leer de la memoria compartida los comandos e interpretarlos.
        while (!quit_app)
        {
             // Poner el proceso a la espera de que esté el comando
            sem_wait( &memory_region->ready );

            if (strcmp( memory_region->command_buffer, QUIT_COMMAND ) == 0)
            {
                quit_app = true;
            }

            // Aquí va código para detectar e interpretar más comandos...
            //                

            // Indicar al cliente que ya se leyó el comando, así que puede poner otro.
            sem_post( &memory_region->empty );
        }

        stop_alarm();
    }

    // Vamos a salir del programa...
    puts( "Ha llegado orden de terminar ¡!" );

    if (shm_created)
    {
        cleanup_control_shm(controlfd);
    }

    if (memory_allocated)
    {
        free_control_memory(memory_region);
    }

    return exit_code;
}

int create_control_shm(int* controlfd)
{
    // Como no hay función en el estándar de C ni en el de C++ para crear objetos de memoria compartida, usamos
    // directamente shm_open() de la librería del sistema.
    *controlfd = shm_open( CONTROL_SHM_NAME, O_CREAT | O_RDWR, 0777 );
    if (*controlfd < 0)
    {
        if (errno == EEXIST)
        {
            // Si ya existe el objeto de memoria compartida, no deberíamos usarlo porque habrían varios servidores
            // usando el mismo canal de control y no hemos pensado en eso al diseñarlo.
            fputs( "Error: Hay otro servidor en ejecución.\n", stderr );
        }
        else {
            fprintf( stderr, "Error (%d) al crear el objeto de memoria compartida: %s\n", errno,
                strerror(errno) );
        }

        return EXIT_FAILURE;
    }

    // El objeto de memoria compartida creado tiene tamaño 0. Tenemos que extenderlo para que quepa 'memory_content'.
    int return_code = ftruncate( *controlfd, sizeof(struct memory_content) );
    if (return_code < 0) {
        fprintf( stderr, "Error (%d) al extender el objeto de memoria compartida: %s\n", errno,
            strerror(errno) );
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void cleanup_control_shm(int controlfd)
{
    // Cerrar el descriptor de archivo del objeto de memoria compartida abierto.
    close(controlfd);

    // Eliminar el objeto de memoria compartida. Nadie más podrá conectarse.
    int return_code = shm_unlink( CONTROL_SHM_NAME );
    if (return_code < 0) {
        fprintf( stderr, "Error (%d) al borrar el objeto de memoria compartida: %s\n", errno,
            strerror(errno) );
    }
}

int init_control_memory(int controlfd, struct memory_content** memory_region)
{
    // Reservar una región de la memoria virtual del proceso y mapear en ella el objeto de memoria compartida
    // recientemente creado.
    *memory_region = mmap( NULL, sizeof(struct memory_content), PROT_READ | PROT_WRITE, MAP_SHARED, controlfd, 0 );
    if (*memory_region == MAP_FAILED)
    {
        fprintf( stderr, "Error (%d) al reservar la memoria compartida: %s\n", errno,
            strerror(errno) );
        return EXIT_FAILURE;
    }

    // Necesitamos un mecanismo para sincronizar cliente y servidor. El cliente necesita esperar a que el servidor
    // lea el último comando antes de escribir el siguiente. El servidor debe esperar a que el cliente ponga un
    // comando antes de intentar leer. Para esto el sistema ofrece mecanismos de sincronización, como los semáforos.
    //
    // Inicializar los semáforo.
    sem_init( &(*memory_region)->empty, 1, 1);
    sem_init( &(*memory_region)->ready, 1, 0);

    return EXIT_SUCCESS;
}

void free_control_memory(struct memory_content* memory_region)
{
    // Liberar la región de memoria reservada para mapear el objeto de memoria compartida.
    munmap( memory_region, sizeof(struct memory_content) );
}
