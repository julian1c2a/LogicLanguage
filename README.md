# LogicLanguage

**LogicLanguage** es un proyecto experimental que tiene como objetivo implementar un lenguaje de dominio específico (DSL) para lógica formal y un demostrador de teoremas en tiempo de compilación utilizando las capacidades avanzadas de C++23.

## 🎯 Objetivo Principal

El propósito de este proyecto es permitir la **escritura y verificación formal de demostraciones matemáticas**, abarcando desde la Lógica de Primer Orden hasta la Teoría de Conjuntos (ZFC), directamente dentro del sistema de tipos de C++. La premisa es simple: **si el código compila, la demostración es matemáticamente válida.**

## 🏗️ Estado del Proyecto

### ✅ Implementado

-   **Sintaxis DSL**: Se ha creado una interfaz intuitiva mediante *Expression Templates* que permite escribir fórmulas de manera natural, como `P(x) && Q(y)` o `forall(x, ...)`.
-   **Sistema de Tipos Lógicos**: Las fórmulas y expresiones lógicas se representan de forma estática como tipos de C++.
-   **Motor de Inferencia (Kernel)**:
    -   `Theorem<T>`: Un tipo robusto que encapsula una fórmula demostrada, garantizando que solo se puedan crear instancias a través de derivaciones válidas.
    -   **Axiomas**: Implementado el Axioma de Identidad (`A -> A`).
    -   **Reglas de Inferencia**: Soporte para *Modus Ponens*, *Generalización* e *Instanciación Universal*.
-   **Motor de Sustitución**: `Substitute_t`, un componente de metaprogramación capaz de reemplazar variables dentro de árboles de tipos complejos que representan fórmulas.
-   **Infraestructura de CI/CD**: Se ha configurado un sistema de construcción robusto utilizando `CMakePresets` y scripts de validación para asegurar la compatibilidad con los principales compiladores: **GCC**, **Clang**, **MSVC** e **Intel oneAPI**.

### 🚧 En Desarrollo

-   **Deducción Natural**: Evolución del sistema para soportar contextos y suposiciones ($\Gamma \vdash \phi$).
-   **Reglas Lógicas Completas**: Implementación de las reglas de introducción y eliminación para todos los conectores lógicos (`And`, `Or`, `Not`).
-   **Teoría de Conjuntos (ZFC)**: Definición de los axiomas y predicados necesarios para formalizar ZFC.

## 🚀 Uso Rápido

### Ejemplo: Demostración de Sócrates

```cpp
#include <logic_language/logic_language.hpp>
using namespace logic;

int main() {
    constexpr auto socrates = "socrates"_var;
    constexpr auto x = "x"_var;
    
    PROOF_BEGIN socrates_is_mortal = []() {
        // Premisa: Todos los humanos son mortales
        constexpr auto all_humans_mortal = ASSUME(forall(x, Human(x) >> Mortal(x)));
        
        // Premisa: Sócrates es humano  
        constexpr auto socrates_human = ASSUME(Human(socrates));
        
        // Instanciar: Human(socrates) → Mortal(socrates)
        constexpr auto implication = FORALL_ELIM(all_humans_mortal, socrates);
        
        // Modus Ponens: Mortal(socrates)
        constexpr auto conclusion = APPLY_MP(socrates_human, implication);
        
        // Descargar hipótesis
        constexpr auto step1 = DISCHARGE(Human(socrates), conclusion);
        constexpr auto final = DISCHARGE(forall(x, Human(x) >> Mortal(x)), step1);
        
        QED final;
    }();
    PROOF_END
    
    // Verificación automática: si compila, la demostración es válida
    ASSERT_TAUTOLOGY(socrates_is_mortal);
    return 0;
}
```

## 🚀 Uso Rápido

### Requisitos

-   **CMake**: Versión 3.22 o superior.
-   **Compilador C++23**:
    -   GCC 13+
    -   Clang 16+
    -   MSVC 19.3+
    -   Intel oneAPI (ICX) 2024+
-   **Python 3**: Necesario para los scripts de validación.

### Validación

Para compilar y ejecutar las pruebas en todos los compiladores detectados, puedes usar el script de Python:

```bash
python scripts/run_validation.py
```

Si estás en un entorno Bash (como MSYS2, Cygwin o Linux), también puedes usar el script de shell:

```bash
./scripts/run_validation.sh
```

## 📂 Estructura del Proyecto

```
LogicLanguage/
├── include/logic_language/   # Código fuente principal de la librería
├── tests/                      # Pruebas de concepto y validación de la lógica
├── scripts/                    # Herramientas de CI/CD local y configuración de entorno
├── CMakeLists.txt              # Script principal de CMake
└── README.md                   # Este archivo
```
