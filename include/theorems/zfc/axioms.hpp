#pragma once

#include "../../logic_language/logic_language.hpp"

namespace logic::zfc {
    
    // =========================================================
    // === ZFC AXIOMS IN C++ eDSL ===
    // =========================================================
    
    // Variables para conjuntos
    constexpr auto x = "x"_var;
    constexpr auto y = "y"_var;
    constexpr auto z = "z"_var;
    constexpr auto A = "A"_var;
    constexpr auto B = "B"_var;
    constexpr auto C = "C"_var;
    
    // Predicados básicos de teoría de conjuntos
    template<typename X, typename Y>
    constexpr auto In(X, Y) { return Predicate<"In", X, Y>{}; }
    
    template<typename X, typename Y>
    constexpr auto Subset(X, Y) { return Predicate<"Subset", X, Y>{}; }
    
    template<typename X>
    constexpr auto Set(X) { return Predicate<"Set", X>{}; }
    
    template<typename X, typename Y>
    constexpr auto Equal_Set(X, Y) { return Predicate<"Equal", X, Y>{}; }
    
    // Funciones/operaciones de conjuntos
    template<typename X, typename Y>
    constexpr auto Union(X, Y) { return Predicate<"Union", X, Y>{}; }
    
    template<typename X>
    constexpr auto PowerSet(X) { return Predicate<"PowerSet", X>{}; }
    
    constexpr auto EmptySet() { return Predicate<"EmptySet">{}; }
    
    // =========================================================
    // === AXIOMAS ZFC ===
    // =========================================================
    
    // Axioma de Extensionalidad
    // ∀A∀B(∀x(x ∈ A ↔ x ∈ B) → A = B)
    constexpr auto axiom_extensionality() {
        auto premise = forall(x, In(x, A) == In(x, B));
        auto conclusion = Equal_Set(A, B);
        return BY_AXIOM(forall(A, forall(B, premise >> conclusion)));
    }
    
    // Axioma del Conjunto Vacío
    // ∃A∀x(¬(x ∈ A))
    constexpr auto axiom_empty_set() {
        auto empty_property = forall(x, !In(x, A));
        return BY_AXIOM(exists(A, empty_property));
    }
    
    // Axioma de Parejas
    // ∀A∀B∃C∀x(x ∈ C ↔ (x = A ∨ x = B))
    constexpr auto axiom_pairing() {
        auto pair_property = forall(x, In(x, C) == (Equal_Set(x, A) || Equal_Set(x, B)));
        return BY_AXIOM(forall(A, forall(B, exists(C, pair_property))));
    }
    
    // Axioma de Unión
    // ∀A∃B∀x(x ∈ B ↔ ∃y(y ∈ A ∧ x ∈ y))
    constexpr auto axiom_union() {
        auto union_property = forall(x, In(x, B) == exists(y, In(y, A) && In(x, y)));
        return BY_AXIOM(forall(A, exists(B, union_property)));
    }
    
    // Axioma del Conjunto Potencia
    // ∀A∃B∀x(x ∈ B ↔ x ⊆ A)
    constexpr auto axiom_power_set() {
        auto power_property = forall(x, In(x, B) == Subset(x, A));
        return BY_AXIOM(forall(A, exists(B, power_property)));
    }
    
    // Esquema de Axiomas de Separación (Comprehension)
    // Para cada fórmula φ(x): ∀A∃B∀x(x ∈ B ↔ (x ∈ A ∧ φ(x)))
    template<typename Formula>
    constexpr auto axiom_separation(Formula phi) {
        auto separation_property = forall(x, In(x, B) == (In(x, A) && phi));
        return BY_AXIOM(forall(A, exists(B, separation_property)));
    }
    
    // Axioma del Infinito
    // ∃A(∅ ∈ A ∧ ∀x(x ∈ A → x ∪ {x} ∈ A))
    constexpr auto axiom_infinity() {
        auto empty_in_A = In(EmptySet(), A);
        auto successor_property = forall(x, In(x, A) >> In(Union(x, Predicate<"Singleton", x>{}), A));
        return BY_AXIOM(exists(A, empty_in_A && successor_property));
    }
    
    // Axioma de Elección (forma simple)
    // ∀A(∀x∈A(x≠∅) → ∃f∀x∈A(f(x)∈x))
    constexpr auto axiom_choice() {
        auto non_empty = forall(x, In(x, A) >> !Equal_Set(x, EmptySet()));
        auto choice_function = exists(Predicate<"Function", "f">{}, 
            forall(x, In(x, A) >> In(Predicate<"Apply", "f", x>{}, x)));
        return BY_AXIOM(forall(A, non_empty >> choice_function));
    }
    
} // namespace logic::zfc
