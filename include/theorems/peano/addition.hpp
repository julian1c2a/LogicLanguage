#pragma once

#include "axioms.hpp"
#include "order.hpp"

namespace logic::peano::addition {
    
    // =========================================================
    // === SUMA (Traducido de PeanoNatAdd.lean) ===
    // =========================================================
    
    // Predicado de suma: Add(n, m, k) significa n + m = k
    template<typename N, typename M, typename K>
    constexpr auto Add(N, M, K) { return Predicate<"Add", N, M, K>{}; }
    
    // Axiomas de la suma
    
    // add_zero: Add(n, 0, n)
    constexpr auto add_zero() {
        return BY_AXIOM(forall(n, Add(n, Zero, n)));
    }
    
    // add_succ: Add(n, m, k) → Add(n, S(m), S(k))
    constexpr auto add_succ() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            Add(n, m, k) >> Add(n, S(m), S(k))))));
    }
    
    // Teoremas fundamentales de la suma
    
    // zero_add: Add(0, n, n)
    constexpr auto zero_add() {
        return BY_AXIOM(forall(n, Add(Zero, n, n)));
    }
    
    // add_comm: Add(n, m, k) ↔ Add(m, n, k)
    constexpr auto add_comm() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            Add(n, m, k) == Add(m, n, k)))));
    }
    
    // add_assoc: Add(n, m, p) ∧ Add(p, k, r) ↔ Add(m, k, q) ∧ Add(n, q, r)
    constexpr auto add_assoc() {
        return BY_AXIOM(forall(n, forall(m, forall(k, forall("p"_var, forall("q"_var, forall("r"_var,
            (Add(n, m, "p"_var) && Add("p"_var, k, "r"_var)) == 
            (Add(m, k, "q"_var) && Add(n, "q"_var, "r"_var)))))))));
    }
    
    // add_cancelation: Add(n, m, k) ∧ Add(n, p, k) → m = p
    constexpr auto add_cancelation() {
        return BY_AXIOM(forall(n, forall(m, forall(k, forall("p"_var,
            (Add(n, m, k) && Add(n, "p"_var, k)) >> Eq(m, "p"_var))))));
    }
    
    // le_self_add: Le(n, k) donde Add(n, m, k)
    constexpr auto le_self_add() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            Add(n, m, k) >> Le(n, k)))));
    }
    
    // lt_self_add: m ≠ 0 ∧ Add(n, m, k) → Lt(n, k)
    constexpr auto lt_self_add() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            (!Eq(m, Zero) && Add(n, m, k)) >> Lt(n, k)))));
    }
    
    // add_lt_add_left: Lt(a, b) → Lt(Add(c, a), Add(c, b))
    constexpr auto add_lt_add_left() {
        return BY_AXIOM(forall("a"_var, forall("b"_var, forall("c"_var, forall("ca"_var, forall("cb"_var,
            (Lt("a"_var, "b"_var) && Add("c"_var, "a"_var, "ca"_var) && Add("c"_var, "b"_var, "cb"_var)) >> 
            Lt("ca"_var, "cb"_var)))))));
    }
    
    // le_then_exists_add: Le(a, b) → ∃p. Add(a, p, b)
    constexpr auto le_then_exists_add() {
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Le("a"_var, "b"_var) >> exists("p"_var, Add("a"_var, "p"_var, "b"_var)))));
    }
    
    // lt_then_exists_add_succ: Lt(a, b) → ∃p. Add(a, S(p), b)
    constexpr auto lt_then_exists_add_succ() {
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Lt("a"_var, "b"_var) >> exists("p"_var, Add("a"_var, S("p"_var), "b"_var)))));
    }
    
} // namespace logic::peano::addition
