# check-build-commands.ps1 - Comprueba los comandos de compilación documentados en los ejemplos de la API de Windows
#
#  Es el equivalente para Windows de check-build-commands.sh: ejecuta el comando documentado bajo el epígrafe
#  «Compilar:» en la cabecera de cada ejemplo, desde el directorio del ejemplo, y avisa de los que fallan.
#
#  Un aviso del compilador también se considera un fallo, porque significa que al comando documentado le falta
#  alguna opción que CMake sí le pasa al compilador.

$ErrorActionPreference = 'Stop'

Set-Location (Join-Path $PSScriptRoot '..\..')

# Localizar el entorno de desarrollo de Visual Studio, que es el que pone cl.exe en el PATH.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsPath = & $vswhere -latest -products * -property installationPath
$vsDevCmd = Join-Path $vsPath 'Common7\Tools\VsDevCmd.bat'

$comprobados = 0
$fallidos = 0

foreach ($linea in (Select-String -Path (Get-ChildItem -Recurse -Path src -Include *.cpp) -Pattern '^//      cl ')) {
    $archivo = Resolve-Path -Relative $linea.Path
    $comando = $linea.Line -replace '^//      ', ''
    $directorio = Split-Path -Parent $linea.Path

    Write-Output ''
    Write-Output "== $archivo"
    Write-Output "   $comando"

    $salida = cmd /c "`"$vsDevCmd`" -arch=x64 -host_arch=x64 >nul 2>&1 && cd /d `"$directorio`" && $comando 2>&1"
    $codigo = $LASTEXITCODE

    $avisos = $salida | Select-String -Pattern ': (warning|error) '

    if ($codigo -ne 0 -or $avisos) {
        $salida | ForEach-Object { Write-Output "   $_" }
        Write-Output '   ERROR: el comando documentado no compila el ejemplo sin avisos.'
        $fallidos++
    }
    $comprobados++

    # Borrar lo que haya generado la compilación, para no dejar el árbol de fuentes lleno de binarios.
    # Ojo: -Include solo filtra si la ruta termina en un comodín.
    Get-ChildItem -Path (Join-Path $directorio '*') -Include *.obj, *.exe, *.pdb, *.ilk -File |
        Remove-Item -Force -ErrorAction SilentlyContinue
}

Write-Output ''
Write-Output '----'
Write-Output "$comprobados comandos comprobados, $fallidos fallidos."

if ($fallidos -ne 0) { exit 1 }
