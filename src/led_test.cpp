// led_test.cpp - Control de LED tricolor por el puerto serie
//
//      g++ -o led_test led_test.cpp

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

void setupSerial(int fd, termios &oldtio)
{
    termios newtio;

    // salvar la configuración actual del puerto
    tcgetattr(fd, &oldtio);

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // modo de entrada (no canónico, sin eco, etc.)
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;  // desactivar temporizado entre caracteres
    newtio.c_cc[VMIN]     = 5;  // bloquear hasta que se reciban 5 bytes
        
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
}

int main(int argc, char* argv[])
{
    int fd;
    termios oldtio;

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY );
    if (fd < 0) {
        std::cerr << "/dev/ttyUSB0" << std::endl;
        exit(-1);
    }

    setupSerial(fd, oldtio);
 
    std::string chars;
    std::cout << "\nIndica el color del LED (R, G o B): \n";
    while(std::cin.good()) {
        std::cin >> chars;

        int character = chars[0];
        if (! isalpha(character)) {
            continue;
        }

        character = toupper(character);
        if (character == 'R' || character == 'G' || character == 'B') {
            write(fd, &character, 1);
        }
 
        chars.clear();
    }

    tcsetattr(fd, TCSANOW, &oldtio);
}
