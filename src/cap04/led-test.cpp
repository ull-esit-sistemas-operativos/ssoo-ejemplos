// led_test.cpp - Control de LED tricolor por el puerto serie
//
//  El ejemplo necesita un arduino conectado en el puerto serie del ordenador, programado con el contenido de
//  'arduino/led_test.pde'.
//
//  Compilar:
//
//      g++ -o led_test led_test.cpp
//

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <print>
#include <string>

const char* DEVICE_PATH = "/dev/ttyUSB0";

void setupSerial(int fd, termios &oldtio)
{
    termios newtio = {};

    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Modo de entrada (no canónico, sin eco, etc.)
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0;     // Desactivar temporizado entre caracteres
    newtio.c_cc[VMIN]  = 5;     // Bloquear hasta que se reciban 5 bytes

    // Salvar la configuración actual del puerto
    tcgetattr( fd, &oldtio );
        
    tcflush( fd, TCIFLUSH );
    tcsetattr( fd, TCSANOW, &newtio );
}

int main(int argc, char* argv[])
{
    int fd = open( DEVICE_PATH, O_RDWR | O_NOCTTY );
    if (fd < 0) {
        std::println( stderr, "Error ({}) al abrir '{}': {}", errno, DEVICE_PATH, std::strerror(errno) );
        return EXIT_FAILURE;
    }

    termios oldtio;
    setupSerial( fd, oldtio );
 
    std::string chars;
    std::println( "\nIndica el color del LED (R, G o B): " );
    while(std::cin.good()) {
        std::cin >> chars;

        int character = chars[0];
        if (! isalpha( character )) {
            continue;
        }

        character = toupper( character );
        if (character == 'R' || character == 'G' || character == 'B') {
            write( fd, &character, 1 );
        }
 
        chars.clear();
    }

    tcsetattr( fd, TCSANOW, &oldtio );
    
    return EXIT_SUCCESS;
}
