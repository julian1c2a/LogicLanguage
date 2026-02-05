#pragma once

#include "axioms.hpp"

namespace logic::peano::strict_order {
    
    // =========================================================
    // === ORDEN ESTRICTO (Traducido de PeanoNatStrictOrder.lean) ===
    // =========================================================
    
    // Predicado de orden estricto Lt
    template<typename N, typename M>
    constexpr auto Lt(N, M) { return Predicate<"Lt", N, M>{}; }
    
    // Teoremas fundamentales del orden estricto
    
    // lt_then_neq: Lt n m → n ≠ m
    constexpr auto lt_then_neq() {
        return BY_AXIOM(forall(n, forall(m, Lt(n, m) >> !Eq(n, m))));
    }
    
    // neq_then_lt_or_gt: n ≠ m → (Lt n m ∨ Lt m n)
    constexpr auto neq_then_lt_or_gt() {
        return BY_AXIOM(forall(n, forall(m, 
            !Eq(n, m) >> (Lt(n, m) || Lt(m, n)))));
    }
    
    // trichotomy: (Lt n m) ∨ (n = m) ∨ (Lt m n)
    constexpr auto trichotomy() {
        return BY_AXIOM(forall(n, forall(m,
            Lt(n, m) || Eq(n, m) || Lt(m, n))));
    }
    
    // lt_asymm: Lt n m → ¬(Lt m n)
    constexpr auto lt_asymm() {
        return BY_AXIOM(forall(n, forall(m,
            Lt(n, m) >> !Lt(m, n))));
    }
    
    // lt_irrefl: ¬(Lt n n)
    constexpr auto lt_irrefl() {
        return BY_AXIOM(forall(n, !Lt(n, n)));
    }
    
    // lt_trans: Lt n m → Lt m k → Lt n k
    constexpr auto lt_trans() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            (Lt(n, m) && Lt(m, k)) >> Lt(n, k)))));
    }
    
    // lt_succ_self: Lt n (S n)
    constexpr auto lt_succ_self() {
        return BY_AXIOM(forall(n, Lt(n, S(n))));
    }
    
    // lt_zero: Lt n 0 → False
    constexpr auto lt_zero() {
        return BY_AXIOM(forall(n, Lt(n, Zero) >> Predicate<"False">{}));
    }
    
    // zero_lt_succ: Lt 0 (S n)
    constexpr auto zero_lt_succ() {
        return BY_AXIOM(forall(n, Lt(Zero, S(n))));
    }
    
    // lt_succ_iff_lt_or_eq: Lt n (S m) ↔ Lt n m ∨ n = m
    constexpr auto lt_succ_iff_lt_or_eq() {
        return BY_AXIOM(forall(n, forall(m,
            Lt(n, S(m)) == (Lt(n, m) || Eq(n, m)))));
    }
    
    // succ_lt_succ_iff: Lt (S n) (S m) ↔ Lt n m
    constexpr auto succ_lt_succ_iff() {
        return BY_AXIOM(forall(n, forall(m,
            Lt(S(n), S(m)) == Lt(n, m))));
    }
    
} // namespace logic::peano::strict_order
