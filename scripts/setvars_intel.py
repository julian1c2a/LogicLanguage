#!/usr/bin/env python3
"""
Script para detectar la ubicación de setvars.bat de Intel oneAPI.
Busca en rutas estándar (C: y D:) y variables de entorno.
"""

import os
import sys

# Rutas estándar donde suele estar Intel oneAPI
INTEL_ONEAPI_PATHS = [
    r"C:\Program Files (x86)\Intel\oneAPI",
    r"C:\Program Files\Intel\oneAPI",
    r"D:\Program Files (x86)\Intel\oneAPI",
    r"D:\Program Files\Intel\oneAPI",
]

def find_intel_oneapi_root():
    """
    Busca el directorio raíz de Intel oneAPI.
    Retorna la ruta si existe, None en caso contrario.
    """
    # 1. Intentar variable de entorno explícita
    if "ONEAPI_ROOT" in os.environ:
        root = os.environ["ONEAPI_ROOT"]
        if os.path.exists(root):
            return root
    
    # 2. Buscar en las rutas estándar
    for path in INTEL_ONEAPI_PATHS:
        if os.path.exists(path):
            return path
    
    return None

def find_setvars_bat():
    """
    Encuentra la ruta absoluta a setvars.bat.
    """
    root = find_intel_oneapi_root()
    if not root:
        return None
    
    bat_path = os.path.join(root, "setvars.bat")
    if os.path.exists(bat_path):
        return bat_path
        
    return None

if __name__ == "__main__":
    bat = find_setvars_bat()
    if bat:
        print(bat)
    else:
        sys.exit(1)