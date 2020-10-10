// factorial.cpp - Funciones comunes a los ejemplos del factorial.
//

#include <iostream>

#include "factorial.hpp"

int get_user_input()
{
    std::cout << "[PADRE] Introduzca un número: ";
    std::cout.flush();
    
    int number;
    std::cin >> number;
    return number;
}

int calculate_factorial(int number)
{
    std::cout << "[HIJO] Calculando...";
    std::cout.flush();

    int factorial = 1;
    for ( int i = 2; i < number; i++ )
    {
        factorial = factorial * number;

        std::cout << '.';
        std::cout.flush();
    }

    std::cout << '\n';
    std::cout.flush();

    return factorial;
}
