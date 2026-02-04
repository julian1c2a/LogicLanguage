#!/usr/bin/env python3
"""
Script para ejecutar setvars.bat de Intel oneAPI y generar comandos de exportación para Bash.
Maneja la conversión de PATH de Windows a Unix (MSYS2).
"""

import os
import subprocess
import sys
from setvars_intel import find_setvars_bat

def win_to_unix_path(win_path):
    """Convierte C:\\Foo a /c/Foo para MSYS2."""
    p = win_path.replace("\\", "/")
    if ":" in p:
        drive, rest = p.split(":", 1)
        return f"/{drive.lower()}{rest}"
    return p

def get_intel_environment(args=None):
    setvars_bat = find_setvars_bat()
    if not setvars_bat:
        sys.stderr.write("ERROR: No se encontró setvars.bat de Intel oneAPI\n")
        sys.exit(1)

    # Argumentos por defecto
    args_str = " ".join(args) if args else "intel64"

    # COMANDO CRÍTICO:
    # 1. set "VSCMD_START_DIR=%CD%" -> Evita el error "STOP 1" de VS
    # 2. call setvars.bat -> Carga el entorno
    # 3. set -> Vuelca las variables
    cmd = f'set "VSCMD_START_DIR=%CD%" && call "{setvars_bat}" {args_str} > nul 2>&1 && set'

    try:
        # shell=True es necesario para comandos internos como 'set'
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    except Exception as e:
        sys.stderr.write(f"Excepción: {e}\n")
        sys.exit(1)

    if result.returncode != 0:
        # Reintento con logs visibles para depuración
        sys.stderr.write("Advertencia: setvars.bat falló. Reintentando con salida visible...\n")
        subprocess.run(f'set "VSCMD_START_DIR=%CD%" && call "{setvars_bat}" {args_str} && set', shell=True)
        sys.exit(1)

    env_vars = {}
    for line in result.stdout.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            key = key.strip().upper()
            value = value.strip()
            
            # Filtramos solo las variables relevantes para el compilador
            valid_keys = ["INCLUDE", "LIB", "LIBPATH", "ONEAPI_ROOT", 
                          "IFORT_COMPILER", "ICPP_COMPILER", "CMAKE_PREFIX_PATH", 
                          "PKG_CONFIG_PATH", "PATH"]
            
            if any(vk in key for vk in valid_keys):
                env_vars[key] = value

    return env_vars

def print_bash_exports(env_vars):
    for key, value in env_vars.items():
        if key == "PATH":
            # Conversión especial para el PATH: Windows (;) -> Unix (:)
            win_paths = value.split(";")
            unix_paths = [win_to_unix_path(p) for p in win_paths if p]
            new_path = ":".join(unix_paths)
            # Añadimos al principio del PATH existente ($PATH)
            print(f'export PATH="{new_path}:$PATH"')
        else:
            # Escapar backslashes y comillas para el resto
            safe_val = value.replace("\\", "\\\\").replace('"', '\\"')
            print(f'export {key}="{safe_val}"')

if __name__ == "__main__":
    args = sys.argv[1:]
    env = get_intel_environment(args)
    
    # Verificación de seguridad
    if "INCLUDE" not in env:
        sys.stderr.write("ERROR: No se detectó INCLUDE. Visual Studio no se cargó correctamente.\n")
        sys.exit(1)
        
    print_bash_exports(env)