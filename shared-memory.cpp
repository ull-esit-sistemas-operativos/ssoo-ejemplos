// shared-memory.cpp - Ejemplo del uso de mmap() para compartir memoria

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

//
// Estructura de un mensaje básico
//
struct Message
{
    int length;                 // longitud del mensaje
    char content[255];          // contenido del mensaje
};

int main()
{
    Message *shared;

    // pedir al sistema que asigne una zona de memoria compartida
    shared = (Message*)mmap(NULL, sizeof(Message), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    if (shared == MAP_FAILED) { // error
        perror("fallo en mmap()");
        exit(-2);
    }
 
    shared->length = 0;         // aun no hay nada en content

    // crear otro proceso
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        char message[255];

        std::cout << "HIJO: ¡Soy el proceso hijo!" << std::endl;
        std::cout << "HIJO: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "HIJO: Voy a esperar a recibir un mensaje..." << std::endl;
        while (shared->length == 0);    // cuando > 0, se ha preparado un mensaje
        strncpy(message, shared->content, shared->length);
        shared->length = 0;

        std::cout << "HIJO: Mensaje recibido. El mensaje es: '" << message << "'" << std::endl;
        exit(1);
    }
    else if (pid > 0) {         // proceso padre
        char message[] = "cadena del mensaje";
        int status;
        
        std::cout << "PADRE: ¡Soy el proceso padre!" << std::endl;
        std::cout << "PADRE: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "PADRE: Durmiendo 1 segundo..." << std::endl;
        sleep(1);
        
        std::cout << "PADRE: Voy a mandar el mensaje..." << std::endl;
        strncpy(shared->content, message, sizeof(message));
        shared->length = sizeof(message);

        std::cout << "PADRE: Voy a esperar a que mi hijo lo reciba..." << std::endl;
        while (shared->length > 0);     // cuando == 0, se ha recibido el mensaje
        std::cout << "PADRE: ¡Recibido!" << std::endl;

        std::cout << "PADRE: Voy a esperar a que mi hijo termine..." << std::endl;
        wait(&status);
        std::cout << "PARENT: El valor de salida de mi hijo fue: " <<
            WEXITSTATUS(status) << std::endl;
        std::cout << "PADRE: ¡Adios!" << std::endl;
        exit(2);
    }
    else {                      // error
        perror("fallo en fork()");
        exit(-1);
    }
}
