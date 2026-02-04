#!/bin/bash

# ==============================================================================
# SCRIPT DE VALIDACIÓN CRUZADA (Bash Version)
# ==============================================================================
# Requiere: scripts/intel_launch.sh para el preset "intel"

# Lista de presets definidos en CMakePresets.json
PRESETS=("gcc" "clang" "msvc" "intel")

# Detectar ubicación del script y raíz del proyecto
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
INTEL_LAUNCHER="$SCRIPT_DIR/intel_launch.sh"

# Moverse a la raíz del proyecto
cd "$PROJECT_ROOT" || { echo "Error: No se pudo acceder a $PROJECT_ROOT"; exit 1; }

if [ ! -f "CMakePresets.json" ]; then
    echo "Error crítico: CMakePresets.json no encontrado en la raíz."
    exit 1
fi

# Asegurar permisos de ejecución para el launcher auxiliar
if [ -f "$INTEL_LAUNCHER" ]; then
    chmod +x "$INTEL_LAUNCHER"
fi

echo "======================================================================"
echo "   VALIDACIÓN CRUZADA (Bash)"
echo "======================================================================"
echo "Directorio de trabajo: $(pwd)"

for PRESET in "${PRESETS[@]}"; do
    echo ""
    echo "--------------------------------------------------"
    echo " PROCESANDO PRESET: ${PRESET^^}"
    echo "--------------------------------------------------"

    # Definir el comando prefijo (wrapper)
    # Por defecto está vacío (ejecución normal)
    CMD_PREFIX=""
    
    # Si el preset es "intel", usamos el script lanzador auxiliar
    if [ "$PRESET" == "intel" ]; then
        if [ -f "$INTEL_LAUNCHER" ]; then
            CMD_PREFIX="$INTEL_LAUNCHER"
            echo "[Intel] Usando wrapper: intel_launch.sh"
        else
            echo "[AVISO] No se encontró $INTEL_LAUNCHER. Saltando Intel."
            continue
        fi
    fi

    # 1. Configurar
    echo -e "\n[Ejecutando] Configuración $PRESET"
    # Ejecuta: [wrapper opcionall] cmake --preset ...
    $CMD_PREFIX cmake --preset "$PRESET" || exit 1

    # 2. Compilar
    echo -e "\n[Ejecutando] Compilación $PRESET"
    $CMD_PREFIX cmake --build --preset "$PRESET" || exit 1

    # 3. Tests
    # Preparamos argumentos para CTest
    CTEST_ARGS="--preset $PRESET --output-on-failure"
    
    # Corrección para MSVC: especificar configuración Debug explícita
    if [ "$PRESET" == "msvc" ]; then
        CTEST_ARGS="$CTEST_ARGS -C Debug"
    fi
    
    echo -e "\n[Ejecutando] Tests $PRESET"
    $CMD_PREFIX ctest $CTEST_ARGS || exit 1

done

echo ""
echo "======================================================================"
echo " ¡ÉXITO! Validación completada en todos los entornos."
echo "======================================================================"