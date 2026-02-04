# Roadmap de LogicLanguage

## ✅ Fase 2: Fundamentos Lógicos y CI (Completada)

- **Sintaxis Core**: Implementación de *Expression Templates* para representar operaciones lógicas como `P(x)`, `&&`, `||`, `forall`, etc.
- **Motor de Sustitución**: Creación de `Substitute_t` para el reemplazo de variables a nivel de tipos.
- **Inferencia Básica**:
    - `Theorem<F>`: Representa la veracidad de una fórmula.
    - Reglas implementadas: Axioma de Identidad, Modus Ponens, Generalización y Particularización.
- **CI/CD Local**: Scripts de validación cruzada para `GCC`, `Clang`, `MSVC` e `Intel oneAPI`.

---

## 🚀 Fase 3: Deducción Natural y Contextos (Próximo Gran Reto)

El objetivo es evolucionar de `Theorem<Formula>` (representando $\vdash A$, "A es verdad absoluta") a `Theorem<Context, Formula>` (representando $\Gamma \vdash \phi$, "Bajo las suposiciones $\Gamma$, se demuestra $\phi$").

### Nuevas Reglas de Inferencia Requeridas:

1.  **Suposición (Assumption)**:
    - Permite introducir una hipótesis en el contexto.
    - `Assume<A>()` $\to$ `Theorem<[A], A>`

2.  **Introducción de la Implicación (Teorema de la Deducción)**:
    - Si a partir de `Theorem<[A, ...], B>` podemos "descargar" la hipótesis `A`, obtendremos `Theorem<[...], A -> B>`.

3.  **Gestión de Contextos en Modus Ponens**:
    - A partir de $\Gamma_1 \vdash A$ y $\Gamma_2 \vdash A \to B$, el resultado debe ser $\Gamma_1 \cup \Gamma_2 \vdash B$.

---

## 📦 Fase 4: Lógica Proposicional Completa

Implementar las reglas de introducción y eliminación para los conectores restantes:

-   **Conjunción ($\land$)**:
    -   **Introducción**: $A, B \vdash A \land B$
    -   **Eliminación**: $A \land B \vdash A$
-   **Disyunción ($\lor$)**:
    -   **Introducción**: $A \vdash A \lor B$
    -   **Eliminación (Prueba por casos)**: $A \lor B, (A \to C), (B \to C) \vdash C$
-   **Negación ($\neg$) y Contradicción ($\bot$)**:
    -   Implementar *Reductio ad absurdum*.

---

## ♾️ Fase 5: Teoría de Conjuntos (ZFC)

Una vez que el sistema lógico sea robusto y ergonómico, se definirá el lenguaje de la teoría de conjuntos.

-   **Predicados Primitivos**:
    -   `In(a, b)`: Representa la pertenencia ($a \in b$).
    -   `Equal(a, b)`: Representa la igualdad.
-   **Axiomas ZFC**: Implementar los axiomas de Zermelo-Fraenkel (Extensionalidad, Par, Unión, Potencia, Infinito, etc.).

---

## 🛠️ Fase 6: Ergonomía y DSL (Domain-Specific Language)

-   **Mejora de la Sintaxis**: Crear macros y funciones de ayuda para que la escritura de demostraciones se asemeje más a las matemáticas formales y menos a la metaprogramación en C++.
-   **Mejora de Errores**: Utilizar `conceptos` de C++20 y `static_assert` con mensajes descriptivos para facilitar la depuración de pruebas fallidas.
