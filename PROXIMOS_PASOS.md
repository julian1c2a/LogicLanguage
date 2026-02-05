# Próximos Pasos: Hacia la Deducción Natural

Este documento detalla la hoja de ruta técnica para evolucionar **LogicLanguage** desde un sistema axiomático básico (estilo Hilbert) hacia un demostrador de teoremas ergonómico basado en el cálculo de **Deducción Natural**.

## 🎯 Objetivo General

El fin es permitir la construcción de **demostraciones matemáticas naturales** que soporten suposiciones, contextos y la "descarga" de hipótesis. Esto es clave para formalizar razonamientos del tipo: *"Supongamos A, derivamos B, por lo tanto, hemos demostrado A implica B"*.

---

## 🚀 Fase 3: Contextos y Deducción Natural (Prioridad Alta)

Actualmente, `Theorem<phi>` representa una verdad absoluta ($\vdash \phi$). El siguiente gran paso es representar verdades relativas a un conjunto de suposiciones ($\Gamma \vdash \phi$).

### 3.1. Estructura de Tipos para Contextos

-   **Implementar `TypeList`**: Una lista de tipos en tiempo de compilación para almacenar las hipótesis del contexto.
-   **Redefinir `Theorem`**: Actualizar la estructura para incluir el contexto:
    ```cpp
    template<typename Context, typename Formula>
    struct Theorem { ... };
    ```
    Donde `Context` será una instancia de `TypeList<H1, H2, ...>`.

### 3.2. Reglas Estructurales

-   **Assumption (Suposición)**: Permite introducir una nueva hipótesis en el contexto.
    -   **Firma**: `assume<A>()` $\to$ `Theorem<TypeList<A>, A>`
-   **Weaken (Debilitamiento)**: Permite añadir hipótesis irrelevantes a un teorema ya demostrado.
    -   **Firma**: Si $\Gamma \vdash A$, entonces $\Gamma, B \vdash A$.

### 3.3. Reglas de Implicación (Teorema de la Deducción)

-   **Implication Intro ($\to_I$)**: La regla central de la deducción natural. Permite "descargar" una hipótesis.
    -   **Firma**: Si se tiene `Theorem<TypeList<A, Rest...>, B>`, se puede derivar `Theorem<TypeList<Rest...>, Implies<A, B>>`.
-   **Implication Elim ($\to_E$ / Modus Ponens)**: Adaptar la regla existente para que gestione y fusione los contextos de las premisas.
    -   **Firma**: $(\Gamma_1 \vdash A) + (\Gamma_2 \vdash A \to B) \Rightarrow (\Gamma_1 \cup \Gamma_2 \vdash B)$.

---

## 📦 Fase 4: Lógica Proposicional Completa

Implementar las reglas de introducción y eliminación para el resto de los conectores lógicos.

### 4.1. Conjunción ($\\land$)

-   **Intro**: $A, B \vdash A \land B$
-   **Elim 1**: $A \land B \vdash A$
-   **Elim 2**: $A \land B \vdash B$

### 4.2. Disyunción ($\\lor$)

-   **Intro 1**: $A \vdash A \lor B$
-   **Intro 2**: $B \vdash A \lor B$
-   **Elim (Prueba por Casos)**: Si $\Gamma \vdash A \lor B$, $\Gamma, A \vdash C$ y $\Gamma, B \vdash C$, entonces se puede concluir $\Gamma \vdash C$.

### 4.3. Negación ($\\neg$) y Contradicción ($\\bot$)

-   **Definir `False` / `Bottom`**: Un tipo para representar la contradicción ($\\bot$).
-   **Negation Intro**: Si $\Gamma, A \vdash \\bot$, entonces $\Gamma \vdash \\neg A$.
-   **Negation Elim**: $A, \\neg A \vdash \\bot$.
-   **Double Negation Elim**: $\\neg \\neg A \vdash A$ (para lógica clásica).

---

## ♾️ Fase 5A: Sistema Aritmético (Alternativa Práctica)

Para evitar la complejidad de construir toda la teoría de conjuntos antes de poder hacer demostraciones por inducción, implementamos un **sistema aritmético primitivo** basado en los axiomas de Peano.

### 5A.1. Números Naturales Primitivos

-   `Natural<N>`: Tipo para representar números naturales como términos del lenguaje.
-   `Succ<N>`: Función sucesor para construir números inductivamente.
-   `zero()`: Constante para el número 0.

### 5A.2. Axiomas de Peano

-   **Axioma del Cero**: 0 es un número natural.
-   **Axioma del Sucesor**: Si n es natural, entonces succ(n) es natural.
-   **Axioma de Inyectividad**: succ es una función inyectiva.
-   **Axioma de No-Circularidad**: 0 no es sucesor de ningún natural.

### 5A.3. Principio de Inducción

Implementación directa del esquema de inducción matemática:
```cpp
// Si P(0) y ∀n(P(n) → P(succ(n))), entonces ∀n P(n)
template<template<typename> class P>
constexpr auto induction_principle(
    Theorem<TypeList<>, P<Natural<0>>>,  // Caso base
    Theorem<TypeList<>, Forall<Var<"n">, Implies<P<Var<"n">>, P<Succ<Var<"n">>>>>>  // Paso inductivo
) -> Theorem<TypeList<>, Forall<Var<"n">, P<Var<"n">>>>;
```

### 5A.4. Aritmética Básica

-   **Suma**: Definida recursivamente con `axiom_add_zero()` y `axiom_add_succ()`.
-   **Multiplicación**: Definida recursivamente con `axiom_mult_zero()` y `axiom_mult_succ()`.
-   **Igualdad**: Predicado primitivo para comparar términos aritméticos.

## ♾️ Fase 5B: Teoría de Conjuntos (ZFC) - Opcional

Para demostraciones más avanzadas que requieran teoría de conjuntos completa.

### 5B.1. Predicados Primitivos

-   `In(a, b)`: Representa la relación de pertenencia ($a \in b$).
-   `Equal(a, b)`: Representa la igualdad de conjuntos (definida a partir de la extensionalidad).

### 5B.2. Axiomas de ZFC

Implementar los axiomas como funciones que devuelven `Theorem<...>` sin premisas (axiomas globales).

-   **Extensionalidad**: $\\forall x \\forall y (\\forall z (z \\in x \\iff z \\in y) \\implies x = y)$.
-   **Par (Pairing)**: $\\forall a \\forall b \\exists x (a \\in x \\land b \\in x)$.
-   ... y así sucesivamente con **Unión**, **Partes**, **Infinito**, etc.

### 5B.3. Conexión con Aritmética

Los números naturales del sistema aritmético pueden ser **interpretados** como conjuntos en ZFC:
-   0 ≡ ∅ (conjunto vacío)
-   succ(n) ≡ n ∪ {n} (construcción de von Neumann)

Esto permite **migrar** demostraciones del sistema aritmético a ZFC cuando sea necesario.

---

## 🛠️ Fase 6: Ergonomía y DSL

El objetivo final es hacer que escribir demostraciones sea lo más cercano posible a escribirlas en papel.

-   **Azúcar Sintáctico**: Desarrollar macros o alias (`PROOF(...)`, `QED`) para ocultar la verbosidad de la metaprogramación de plantillas.
-   **Mensajes de Error Claros**: Mejorar los `static_assert` para que, cuando una derivación falle, el compilador proporcione un mensaje claro sobre por qué la regla no se pudo aplicar.
