#!/bin/bash

# ==============================================================================
# INTEL oneAPI LAUNCHER
# ==============================================================================
# Delega la carga del entorno a scripts de Python robustos.

# Directorio donde están los scripts
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Detectar Python
if command -v python3 &> /dev/null; then
    PY_CMD=python3
elif command -v python &> /dev/null; then
    PY_CMD=python
else
    echo "[ERROR] Python no encontrado."
    exit 1
fi

# Ejecutar el generador de entorno
# Esto imprimirá una serie de "export VAR=VAL" si tiene éxito
ENV_OUTPUT=$("$PY_CMD" "$SCRIPT_DIR/setvarsall_intel.py")
RET_CODE=$?

if [ $RET_CODE -ne 0 ]; then
    echo "[ERROR] Falló la configuración de Intel oneAPI."
    exit 1
fi

# Cargar las variables en el shell actual
eval "$ENV_OUTPUT"

# Verificar que icx está accesible
if ! command -v icx &> /dev/null; then
    echo "[ERROR] 'icx' no encontrado en el PATH."
    exit 1
fi

# Ejecutar el comando solicitado
exec "$@"