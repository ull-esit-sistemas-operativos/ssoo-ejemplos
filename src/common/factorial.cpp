// factorial.cpp - Funciones comunes a los ejemplos del factorial.
//

#include <iostream>
#include <print>

#include "factorial.hpp"

int get_user_input()
{
    std::print( "[PADRE] Introduzca un número: " );
    std::cout.flush();
    
    int number;
    std::cin >> number;
    return number;
}

int calculate_factorial(int number)
{
    std::print( "[HIJO] Calculando..." );
    std::cout.flush();

    int factorial = 1;
    for ( int i = 2; i <= number; i++ )
    {
        factorial = factorial * i;

        std::print(".");
        std::cout.flush();
    }

    std::cout << std::endl;

    return factorial;
}
