#!/usr/bin/env bash
#
# check-build-commands.sh - Comprueba los comandos de compilación documentados en los ejemplos
#
#  La cabecera de cada ejemplo documenta, bajo el epígrafe «Compilar:», el comando con el que compilarlo a mano,
#  sin CMake. Este script ejecuta cada uno de esos comandos desde el directorio del ejemplo, que es desde donde
#  están escritos para ejecutarse, y avisa de los que fallan.
#
#  Solo se comprueban los comandos de GCC, porque el resto de ejemplos son de la API de Windows.
#

set -uo pipefail

cd "$( dirname "$0" )/../.."

comprobados=0
fallidos=0

# Las líneas que devuelve grep tienen la forma 'ruta/al/ejemplo.cpp:5://      g++ -o ejemplo ejemplo.cpp'.
while IFS= read -r linea
do
    archivo="${linea%%:*}"
    comando="${linea#*://      }"
    directorio="$( dirname "$archivo" )"

    printf '\n== %s\n   %s\n' "$archivo" "$comando"

    if ( cd "$directorio" && eval "$comando" )
    then
        comprobados=$(( comprobados + 1 ))
    else
        echo "   ERROR: el comando documentado no compila el ejemplo."
        fallidos=$(( fallidos + 1 ))
    fi

    # Borrar el ejecutable generado, para no dejar el árbol de fuentes lleno de binarios.
    ejecutable="$( echo "$comando" | grep -oE '\-o [^ ]+' | sed 's/^-o //' )"
    [ -n "$ejecutable" ] && rm -f "$directorio/$ejecutable"
done < <( grep -rn '^//      g++ \|^//      gcc ' src/ )

printf '\n----\n%d comandos comprobados, %d fallidos.\n' "$(( comprobados + fallidos ))" "$fallidos"

[ "$fallidos" -eq 0 ]
