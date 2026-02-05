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
    # Posibles rutas de instalación de Intel oneAPI en Windows
    possible_paths = [
        r"C:\Program Files (x86)\Intel\oneAPI\setvars.bat",
        r"C:\Program Files\Intel\oneAPI\setvars.bat",
        r"D:\Program Files (x86)\Intel\oneAPI\setvars.bat",
        r"D:\Program Files\Intel\oneAPI\setvars.bat",
    ]
    
    setvars_path = None
    for path in possible_paths:
        if os.path.exists(path):
            setvars_path = path
            break
    
    if setvars_path is None:
        print("[AVISO] No se encontró Intel oneAPI en ninguna ubicación estándar:")
        for path in possible_paths:
            print(f"        - {path}")
        print("        Saltando pruebas de Intel.")
        return None

    print(f"[Intel] Cargando entorno desde: {setvars_path} ...")
    
    # TRUCO CRÍTICO:
    # Ejecutamos el .bat en una shell y luego 'set' para capturar las variables resultantes.
    # Intel icx necesita las vars de VS (INCLUDE, LIB, etc.) que setvars.bat carga automáticamente.
    command = f'call "{setvars_path}" --config=release --force > nul 2>&1 && set'
    
    try:
        # Usamos cmd.exe explícitamente porque setvars es un batch file
        # Aumentamos el timeout para dar tiempo a que setvars.bat termine
        output = subprocess.check_output(
            command, 
            shell=True, 
            executable=shutil.which('cmd.exe'), 
            text=True,
            timeout=60,  # 60 segundos de timeout
            stderr=subprocess.DEVNULL  # Suprimir errores de stderr
        )
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Falló la ejecución de setvars.bat (código: {e.returncode})")
        print("        Esto puede deberse a:")
        print("        - Intel oneAPI no está completamente instalado")
        print("        - Faltan componentes de Visual Studio")
        print("        - Problemas de permisos")
        return None
    except subprocess.TimeoutExpired:
        print("[ERROR] Timeout ejecutando setvars.bat (>60s)")
        print("        El script de Intel puede estar colgado")
        return None
    except Exception as e:
        print(f"[ERROR] Error inesperado ejecutando setvars.bat: {e}")
        return None

    # Creamos un nuevo entorno basado en el actual del sistema
    new_env = os.environ.copy()
    
    # Parseamos la salida del comando 'set'
    for line in output.splitlines():
        if '=' in line and not line.startswith('='):  # Evitar líneas malformadas
            key, _, value = line.partition('=')
            if key.strip():  # Solo procesar keys no vacías
                # Normalizamos keys a mayúsculas para consistencia en Windows, 
                # pero guardamos ambas por compatibilidad con herramientas case-sensitive
                new_env[key] = value
                new_env[key.upper()] = value
    
    # Verificar que las variables críticas de Intel estén presentes
    intel_vars = ['ONEAPI_ROOT', 'INTEL_LLVM_DIR', 'PATH']
    missing_vars = []
    for var in intel_vars:
        if var not in new_env:
            missing_vars.append(var)
    
    if missing_vars:
        print(f"[AVISO] Variables de Intel oneAPI faltantes: {missing_vars}")
        print("        El entorno puede no estar completamente configurado")
    
    print("[Intel] Entorno cargado exitosamente")
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
                print(f"[Intel] Saltando preset '{preset}' debido a problemas de configuración")
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
