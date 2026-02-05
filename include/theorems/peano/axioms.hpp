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
    // === AXIOMAS DE PEANO (Traducidos desde Lean4) ===
    // =========================================================
    
    // Tipos básicos para ℕ₀
    template<size_t N>
    using PeanoNat = Natural<N>;
    
    using PeanoZero = PeanoNat<0>;
    constexpr auto peano_zero = PeanoZero{};
    
    template<typename N>
    using PeanoSucc = Succ<N>;
    
    // Predicados fundamentales (traducidos de PeanoNatAxioms.lean)
    template<typename X>
    constexpr auto is_zero(X) { return Predicate<"is_zero", X>{}; }
    
    template<typename X>
    constexpr auto is_succ(X) { return Predicate<"is_succ", X>{}; }
    
    // PA1: 0 es un número natural (AXIOM_zero_is_an_PeanoNat)
    constexpr auto PA1() {
        return BY_AXIOM(IsNat(Zero));
    }
    
    // PA2: Si n es natural, entonces S(n) es natural (AXIOM_succ_is_an_PeanoNat)
    constexpr auto PA2() {
        return BY_AXIOM(forall(n, IsNat(n) >> IsNat(S(n))));
    }
    
    // PA3: Para todo n natural, S(n) ≠ 0 (AXIOM_cero_neq_succ)
    constexpr auto PA3() {
        return BY_AXIOM(forall(n, IsNat(n) >> !Eq(S(n), Zero)));
    }
    
    // PA4: S es inyectiva (AXIOM_succ_inj)
    constexpr auto PA4() {
        return BY_AXIOM(forall(n, forall(m, 
            (IsNat(n) && IsNat(m) && Eq(S(n), S(m))) >> Eq(n, m))));
    }
    
    // PA5: Esquema de Inducción (AXIOM_induction_on_PeanoNat)
    template<typename Formula>
    constexpr auto PA5_induction(Formula phi) {
        auto base_case = phi; // φ(0)
        auto inductive_step = forall(n, phi >> Substitute_t<Formula, decltype(n), decltype(S(n))>);
        auto conclusion = forall(n, phi);
        return BY_AXIOM((base_case && inductive_step) >> conclusion);
    }
    
    // Teoremas auxiliares importantes (de PeanoNatAxioms.lean)
    
    // neq_succ: k ≠ σ k
    constexpr auto neq_succ() {
        return BY_AXIOM(forall(k, !Eq(k, S(k))));
    }
    
    // succ_neq_zero: σ n ≠ 0
    constexpr auto succ_neq_zero() {
        return BY_AXIOM(forall(n, !Eq(S(n), Zero)));
    }
    
    // Isomorfismos Λ y Ψ (conceptuales en el eDSL)
    template<size_t N>
    constexpr auto Lambda(Natural<N>) { return PeanoNat<N>{}; }
    
    template<typename PeanoNum>
    constexpr auto Psi(PeanoNum) { return Natural<0>{}; } // Simplificado
    
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
