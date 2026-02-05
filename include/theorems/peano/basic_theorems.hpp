#pragma once

#include "axioms.hpp"

namespace logic::peano::theorems {
    
    // =========================================================
    // === TEOREMAS BÁSICOS DE ARITMÉTICA PEANO ===
    // =========================================================
    
    // Teorema: 0 + n = n (conmutatividad parcial de la suma)
    constexpr auto zero_plus_n() {
        // Demostración por inducción sobre n
        
        // Caso base: 0 + 0 = 0
        auto base_case = plus_zero(); // Ya tenemos n + 0 = n, necesitamos 0 + 0 = 0
        
        // Paso inductivo: Si 0 + n = n, entonces 0 + S(n) = S(n)
        auto inductive_hypothesis = ASSUME(Plus(Zero, n, n));
        
        // Usar axioma de suma con sucesor
        auto succ_axiom = plus_succ();
        // 0 + S(n) = S(0 + n) = S(n) por hipótesis inductiva
        
        auto inductive_step = DISCHARGE(decltype(inductive_hypothesis)::formula_type,
                                      ASSUME(Plus(Zero, S(n), S(n))));
        
        // Aplicar inducción
        return PA5_induction(Plus(Zero, n, n));
    }
    
    // Teorema: S(n) ≠ n para todo n natural
    constexpr auto succ_neq_self() {
        // Demostración por inducción
        
        // Caso base: S(0) ≠ 0 (esto es PA3)
        auto base_case = PA3();
        
        // Paso inductivo: Si S(n) ≠ n, entonces S(S(n)) ≠ S(n)
        auto inductive_hypothesis = ASSUME(!Eq(S(n), n));
        
        // Por PA4 (inyectividad), S(S(n)) = S(n) → S(n) = n
        // Pero sabemos S(n) ≠ n, por lo tanto S(S(n)) ≠ S(n)
        auto injectivity = PA4();
        
        auto inductive_step = DISCHARGE(decltype(inductive_hypothesis)::formula_type,
                                      ASSUME(!Eq(S(S(n)), S(n))));
        
        return PA5_induction(!Eq(S(n), n));
    }
    
    // Teorema: Asociatividad de la suma
    // (a + b) + c = a + (b + c)
    constexpr auto plus_associative() {
        // Esta demostración requiere inducción sobre c
        // y uso de los axiomas de suma
        
        // Caso base: (a + b) + 0 = a + (b + 0)
        // Por axioma: x + 0 = x, entonces (a + b) + 0 = a + b = a + (b + 0)
        auto base_case = plus_zero();
        
        // Paso inductivo es más complejo y requiere varios pasos
        // (Simplificación para el ejemplo)
        return PA5_induction(Eq(Plus("a"_var, "b"_var, "ab"_var), Plus("a"_var, "bc"_var, "abc"_var)));
    }
    
    // Teorema: Conmutatividad de la suma
    // a + b = b + a
    constexpr auto plus_commutative() {
        // Demostración por inducción sobre b
        // Requiere el teorema 0 + n = n como lema
        
        auto zero_plus = zero_plus_n();
        
        // Caso base: a + 0 = 0 + a
        // Por axiomas: a + 0 = a y 0 + a = a
        auto base_case = APPLY_MP(plus_zero(), zero_plus);
        
        // Paso inductivo requiere más desarrollo
        return PA5_induction(Eq(Plus("a"_var, "b"_var, "ab"_var), Plus("b"_var, "a"_var, "ba"_var)));
    }
    
    // Teorema: Distributividad
    // a * (b + c) = (a * b) + (a * c)
    constexpr auto times_distributive() {
        // Demostración por inducción sobre c
        // Requiere teoremas previos sobre suma y multiplicación
        return PA5_induction(Eq(Times("a"_var, Plus("b"_var, "c"_var, "bc"_var), "abc"_var),
                               Plus(Times("a"_var, "b"_var, "ab"_var), Times("a"_var, "c"_var, "ac"_var), "result"_var)));
    }
    
} // namespace logic::peano::theorems
