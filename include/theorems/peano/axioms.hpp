#pragma once

#include "../../logic_language/logic_language.hpp"

namespace logic::peano {
    
    // =========================================================
    // === PEANO AXIOMS IN C++ eDSL ===
    // =========================================================
    
    // Variables para números naturales
    constexpr auto n = "n"_var;
    constexpr auto m = "m"_var;
    constexpr auto k = "k"_var;
    
    // Constante cero
    constexpr auto Zero = Natural<0>{};
    
    // Función sucesor
    template<typename N>
    constexpr auto S(N) { return Succ<N>{}; }
    
    // Predicados aritméticos
    template<typename X>
    constexpr auto IsNat(X) { return Predicate<"Natural", X>{}; }
    
    template<typename X, typename Y>
    constexpr auto Eq(X, Y) { return Equal<X, Y>{}; }
    
    template<typename X, typename Y, typename Z>
    constexpr auto Plus(X, Y, Z) { return Predicate<"Plus", X, Y, Z>{}; } // X + Y = Z
    
    template<typename X, typename Y, typename Z>
    constexpr auto Times(X, Y, Z) { return Predicate<"Times", X, Y, Z>{}; } // X * Y = Z
    
    // =========================================================
    // === AXIOMAS DE PEANO ===
    // =========================================================
    
    // PA1: 0 es un número natural
    constexpr auto PA1() {
        return BY_AXIOM(IsNat(Zero));
    }
    
    // PA2: Si n es natural, entonces S(n) es natural
    constexpr auto PA2() {
        return BY_AXIOM(forall(n, IsNat(n) >> IsNat(S(n))));
    }
    
    // PA3: Para todo n natural, S(n) ≠ 0
    constexpr auto PA3() {
        return BY_AXIOM(forall(n, IsNat(n) >> !Eq(S(n), Zero)));
    }
    
    // PA4: S es inyectiva: S(n) = S(m) → n = m
    constexpr auto PA4() {
        return BY_AXIOM(forall(n, forall(m, 
            (IsNat(n) && IsNat(m) && Eq(S(n), S(m))) >> Eq(n, m))));
    }
    
    // PA5: Esquema de Inducción
    // Para cualquier fórmula φ(x): φ(0) ∧ ∀n(φ(n) → φ(S(n))) → ∀n φ(n)
    template<typename Formula>
    constexpr auto PA5_induction(Formula phi) {
        auto base_case = phi; // φ(0) - asumiendo que phi ya está instanciada con 0
        auto inductive_step = forall(n, phi >> Substitute_t<Formula, decltype(n), decltype(S(n))>);
        auto conclusion = forall(n, phi);
        return BY_AXIOM((base_case && inductive_step) >> conclusion);
    }
    
    // =========================================================
    // === AXIOMAS PARA SUMA ===
    // =========================================================
    
    // Suma con 0: n + 0 = n
    constexpr auto plus_zero() {
        return BY_AXIOM(forall(n, IsNat(n) >> Plus(n, Zero, n)));
    }
    
    // Suma con sucesor: n + S(m) = S(n + m)
    constexpr auto plus_succ() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            (IsNat(n) && IsNat(m) && Plus(n, m, k)) >> 
            Plus(n, S(m), S(k))))));
    }
    
    // =========================================================
    // === AXIOMAS PARA MULTIPLICACIÓN ===
    // =========================================================
    
    // Multiplicación por 0: n * 0 = 0
    constexpr auto times_zero() {
        return BY_AXIOM(forall(n, IsNat(n) >> Times(n, Zero, Zero)));
    }
    
    // Multiplicación por sucesor: n * S(m) = (n * m) + n
    constexpr auto times_succ() {
        return BY_AXIOM(forall(n, forall(m, forall(k, forall("p"_var,
            (IsNat(n) && IsNat(m) && Times(n, m, k) && Plus(k, n, "p"_var)) >>
            Times(n, S(m), "p"_var))))));
    }
    
} // namespace logic::peano
