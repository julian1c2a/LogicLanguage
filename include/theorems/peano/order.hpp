#pragma once

#include "axioms.hpp"
#include "strict_order.hpp"

namespace logic::peano::order {
    
    // =========================================================
    // === ORDEN PARCIAL (Traducido de PeanoNatOrder.lean) ===
    // =========================================================
    
    // Predicado de orden parcial Le (menor o igual)
    template<typename N, typename M>
    constexpr auto Le(N, M) { return Predicate<"Le", N, M>{}; }
    
    // Definición: Le n m ≡ Lt n m ∨ n = m
    constexpr auto le_definition() {
        return BY_AXIOM(forall(n, forall(m,
            Le(n, m) == (Lt(n, m) || Eq(n, m)))));
    }
    
    // Teoremas fundamentales del orden parcial
    
    // zero_le: Le 0 n
    constexpr auto zero_le() {
        return BY_AXIOM(forall(n, Le(Zero, n)));
    }
    
    // le_refl: Le n n
    constexpr auto le_refl() {
        return BY_AXIOM(forall(n, Le(n, n)));
    }
    
    // le_trans: Le n m → Le m k → Le n k
    constexpr auto le_trans() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            (Le(n, m) && Le(m, k)) >> Le(n, k)))));
    }
    
    // le_antisymm: Le n m → Le m n → n = m
    constexpr auto le_antisymm() {
        return BY_AXIOM(forall(n, forall(m,
            (Le(n, m) && Le(m, n)) >> Eq(n, m))));
    }
    
    // le_total: Le n m ∨ Le m n
    constexpr auto le_total() {
        return BY_AXIOM(forall(n, forall(m,
            Le(n, m) || Le(m, n))));
    }
    
    // succ_le_succ_iff: Le (S n) (S m) ↔ Le n m
    constexpr auto succ_le_succ_iff() {
        return BY_AXIOM(forall(n, forall(m,
            Le(S(n), S(m)) == Le(n, m))));
    }
    
    // le_iff_lt_succ: Le n m ↔ Lt n (S m)
    constexpr auto le_iff_lt_succ() {
        return BY_AXIOM(forall(n, forall(m,
            Le(n, m) == Lt(n, S(m)))));
    }
    
    // lt_imp_le: Lt n m → Le n m
    constexpr auto lt_imp_le() {
        return BY_AXIOM(forall(n, forall(m,
            Lt(n, m) >> Le(n, m))));
    }
    
    // le_succ_self: Le n (S n)
    constexpr auto le_succ_self() {
        return BY_AXIOM(forall(n, Le(n, S(n))));
    }
    
    // le_zero_eq_zero: Le n 0 ↔ n = 0
    constexpr auto le_zero_eq_zero() {
        return BY_AXIOM(forall(n,
            Le(n, Zero) == Eq(n, Zero)));
    }
    
} // namespace logic::peano::order
