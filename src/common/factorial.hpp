// factorial.hpp - Funciones comunes a los ejemplos del factorial.
//

#pragma once

#include <iostream>
#include <print>

#include "factorial.hpp"

int get_user_input(std::string_view output_label)
{
    if (! output_label.empty())
    {
        std::print( "[{}] ", output_label);
    }

    std::print( "Introduzca un número: " );
    std::cout.flush();
    
    int number;
    std::cin >> number;
    return number;
}

int calculate_factorial(int number, std::string_view output_label)
{
    if (! output_label.empty())
    {
        std::print( "[{}] ", output_label);
    }

    std::println( "Calculando..." );

    int factorial = 1;
    for ( int i = 2; i <= number; i++ )
    {
        factorial = factorial * i;
    }

    return factorial;
}
