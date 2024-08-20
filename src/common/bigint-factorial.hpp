// bigint_factorial.cpp - Funciones comunes a los ejemplos del factorial con BigInt.
//

#include <iostream>
#include <print>

#include <BigInt/BigInt.hpp>

BigInt get_user_input(std::string_view output_label = "INPUT")
{
    if (! output_label.empty())
    {
        std::print( "[{}] ", output_label);
    }

    std::print( "Introduzca un número: " );
    std::cout.flush();
    
    BigInt number;
    std::cin >> number;
    return number;
}

BigInt calculate_factorial(BigInt number, BigInt lower_bound, std::string_view output_label = "FACTORIAL")
{
    if (! output_label.empty())
    {
        std::print( "[{}] ", output_label);
    }

    std::println( "Calculando..." );

    lower_bound = lower_bound < 2 ? 2 : lower_bound;
    BigInt factorial = 1;
    for ( BigInt i = lower_bound; i <= number; i++ )
    {
        factorial = factorial * i;
    }

    return factorial;
}

BigInt calculate_factorial(BigInt number, std::string_view output_label = "FACTORIAL")
{
    return calculate_factorial(number, 2, output_label);
}
