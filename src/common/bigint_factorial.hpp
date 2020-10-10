// bigint_factorial.cpp - Funciones comunes a los ejemplos del factorial con BigInt.
//

#include <iostream>

#include <BigInt/BigInt.hpp>

BigInt get_user_input()
{
    std::cout << "Introduzca un número: ";
    std::cout.flush();
    
    BigInt number;
    std::cin >> number;
    return number;
}

BigInt calculate_factorial(BigInt number, BigInt lower_bound = 2)
{
    lower_bound = lower_bound < 2 ? 2 : lower_bound;
    BigInt factorial = 1;
    for ( BigInt i = lower_bound; i <= number; i++ )
    {
        factorial = factorial * i;
    }

    return factorial;
}
