// shared-memory.cpp - Ejemplo del uso de mmap() para compartir memoria

#include <unistd.h>
#include <sys/wait.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <iostream>

#include "memory_map.h"

//
// Estructura de un mensaje básico
//
struct message
{
    size_t length;                  // longitud del mensaje
    std::array<char, 255> content;  // contenido del mensaje
    
    message() : length{0} {}
};

int main()
{
    // pedir al sistema que asigne una zona de memoria compartida
    memory_map shared_mmap {PROT_READ | PROT_WRITE, sizeof(message)};

    // usar el new de emplazamiento para construir un objeto message,
    // sin reservar memoria, sobre la memoria compartida
    auto shared_message = new(shared_mmap.data<void*>()) message;

    // crear otro proceso
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!\n";
        std::cout << "HIJO: Este es mi PID: " << getpid() << "\n";
        std::cout << "HIJO: Voy a esperar a recibir un mensaje...\n";
        while (shared_message->length == 0);    // cuando > 0, se ha preparado un mensaje

        std::string buffer(shared_message->content.data(), shared_message->length);
        shared_message->length = 0;

        std::cout << "HIJO: Mensaje recibido. El mensaje es: '" << buffer << "'\n";
        return 0;
    }
    else if (pid > 0) {         // proceso padre
        std::cout << "PADRE: ¡Soy el proceso padre!\n";
        std::cout << "PADRE: Este es mi PID: " << getpid()<< "\n";
        std::cout << "PADRE: Durmiendo 1 segundo...\n";
        sleep(1);
        
        std::cout << "PADRE: Voy a mandar el mensaje...\n";
        
        std::string text = "cadena del mensaje";
        text.copy(shared_message->content.data(), shared_message->content.size());
        shared_message->length = text.length();

        std::cout << "PADRE: Voy a esperar a que mi hijo lo reciba...\n";
        while (shared_message->length > 0);     // cuando == 0, se ha recibido el mensaje
        std::cout << "PADRE: ¡Recibido!\n";

        std::cout << "PADRE: Voy a esperar a que mi hijo termine...\n";
        int status;
        wait(&status);
        std::cout << "PARENT: El valor de salida de mi hijo fue: " <<
            WEXITSTATUS(status) << "\n";
        std::cout << "PADRE: ¡Adios!\n";
        return 0;
    }
    else {                      // error
        std::cerr << "fallo en fork(): " << std::strerror(errno) << "\n";
        return 4;
    }
}
