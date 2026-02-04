import subprocess
import sys
import os
import shutil

# --- CONFIGURACIÓN ---
# Lista de presets a validar
# El orden importa: GCC y Clang suelen ser más rápidos para fallos de sintaxis.
PRESETS = ["gcc", "clang", "msvc", "intel"]

def get_project_root():
    """Calcula la raíz del proyecto basándose en la ubicación de este script."""
    # Este script está en <root>/scripts/run_validation.py
    script_path = os.path.abspath(__file__)
    script_dir = os.path.dirname(script_path)
    # Subimos un nivel para llegar a la raíz
    return os.path.dirname(script_dir)

def get_intel_env():
    """
    Busca y carga las variables de entorno de Intel oneAPI.
    Devuelve un diccionario con el entorno modificado o None si falla.
    """
    # Ruta estándar de instalación de Intel oneAPI en Windows
    setvars_path = r"C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
    
    if not os.path.exists(setvars_path):
        print(f"[AVISO] No se encontró Intel oneAPI en: {setvars_path}")
        print("        Saltando pruebas de Intel.")
        return None

    print(f"[Intel] Cargando entorno desde: {setvars_path} ...")
    
    # TRUCO CRÍTICO:
    # Ejecutamos el .bat en una shell y luego 'set' para capturar las variables resultantes.
    # Intel icx necesita las vars de VS (INCLUDE, LIB, etc.) que setvars.bat carga automáticamente.
    command = f'call "{setvars_path}" > nul && set'
    
    try:
        # Usamos cmd.exe explícitamente porque setvars es un batch file
        output = subprocess.check_output(command, shell=True, executable=shutil.which('cmd.exe'), text=True)
    except subprocess.CalledProcessError:
        print("[ERROR] Falló la ejecución de setvars.bat")
        return None

    # Creamos un nuevo entorno basado en el actual del sistema
    new_env = os.environ.copy()
    
    # Parseamos la salida del comando 'set'
    for line in output.splitlines():
        if '=' in line:
            key, _, value = line.partition('=')
            # Normalizamos keys a mayúsculas para consistencia en Windows, 
            # pero guardamos ambas por compatibilidad con herramientas case-sensitive
            new_env[key] = value
            new_env[key.upper()] = value
            
    return new_env

def run_command(command, description, env=None):
    """Ejecuta un comando y verifica si tuvo éxito."""
    print(f"\n[Ejecutando] {description}")
    print(f"$ {' '.join(command)}")
    
    # Usamos el entorno modificado (para Intel) o el del sistema
    run_env = env if env is not None else os.environ.copy()

    # Ejecutamos el proceso
    result = subprocess.run(command, text=True, env=run_env)
    
    if result.returncode != 0:
        print(f"\n[ERROR] Falló: {description}")
        print("Deteniendo ejecución para evitar errores en cascada.")
        sys.exit(result.returncode)

def main():
    print("="*70)
    print("   VALIDACIÓN CRUZADA: GCC, CLANG, MSVC e INTEL oneAPI")
    print("="*70)
    
    # 1. Nos movemos a la raíz del proyecto para que CMake encuentre CMakeLists.txt y Presets
    root_dir = get_project_root()
    print(f"Directorio del proyecto: {root_dir}")
    os.chdir(root_dir)

    if not os.path.exists("CMakePresets.json"):
        print("Error crítico: No se encuentra CMakePresets.json en la raíz detectada.")
        sys.exit(1)

    for preset in PRESETS:
        print(f"\n" + "-"*50)
        print(f" PROCESANDO PRESET: {preset.upper()}")
        print("-"*50)

        # Manejo especial para el entorno de Intel
        current_env = None
        if preset == "intel":
            current_env = get_intel_env()
            if current_env is None:
                continue # Saltamos Intel si no está instalado o falla la carga

        # A. Configurar (CMake Generate)
        run_command(["cmake", "--preset", preset], f"Configuración {preset}", env=current_env)

        # B. Compilar (Build)
        run_command(["cmake", "--build", "--preset", preset], f"Compilación {preset}", env=current_env)

        # C. Tests (CTest)
        test_cmd = ["ctest", "--preset", preset]
        
        # Corrección para MSVC: CTest necesita saber explícitamente la configuración
        # ya que VS es un generador multi-configuración.
        if preset == "msvc":
            test_cmd.extend(["-C", "Debug"])
        
        # Output-on-failure: Muestra logs solo si falla
        test_cmd.append("--output-on-failure")
            
        run_command(test_cmd, f"Tests {preset}", env=current_env)

    print("\n" + "="*70)
    print(" ¡ÉXITO! El DSL compila y pasa validaciones en todos los entornos.")
    print("="*70)

if __name__ == "__main__":
    main()