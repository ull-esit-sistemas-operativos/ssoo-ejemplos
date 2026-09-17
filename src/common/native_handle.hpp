// native_handle.hpp - Utilidad para mostrar el manejador nativo de un hilo.
//

#pragma once

#include <bit>
#include <cstdint>

// El tipo que devuelve native_handle() depende del sistema operativo: en Linux es 'pthread_t' mientras  que en
// Windows es 'HANDLE'.
//
// Los ejemplos utilizan el especificador de formato "{:x}" para mostrar el manejador en hexadecimal, pero solo
// funciona para enteros, no para punteros --como `HANDLE` en Windows. Por eso reinterpretamos los bits del manejador
// como un entero sin signo del tamaño de un puntero, que sí se puede mostrar en hexadecimal en cualquier sistema.
template <typename Thread>
std::uintptr_t native_handle_value(Thread& thread)
{
    return std::bit_cast<std::uintptr_t>( thread.native_handle() );
}
