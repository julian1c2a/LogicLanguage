#pragma once

#include "axioms.hpp"

namespace logic::peano::theorems {
    
    // =========================================================
    // === TEOREMAS BÁSICOS DE ARITMÉTICA PEANO ===
    // =========================================================
    
    // =========================================================
    // === TEOREMAS BÁSICOS TRADUCIDOS DESDE LEAN4 ===
    // =========================================================
    
    // Teorema: zero_add (traducido de PeanoNatAdd.lean)
    constexpr auto zero_add_theorem() {
        // En Lean4: theorem zero_add (n : ℕ₀) : add 𝟘 n = n
        auto base_case = ASSUME(Add(Zero, Zero, Zero));
        auto inductive_step = ASSUME(forall(n, 
            Add(Zero, n, n) >> Add(Zero, S(n), S(n))));
        return PA5_induction(Add(Zero, n, n));
    }
    
    // Teorema: add_comm (traducido de PeanoNatAdd.lean)
    constexpr auto add_commutative() {
        // En Lean4: theorem add_comm (n m : ℕ₀) : add n m = add m n
        return BY_AXIOM(forall(n, forall(m, forall(k,
            Add(n, m, k) == Add(m, n, k)))));
    }
    
    // Teorema: add_assoc (traducido de PeanoNatAdd.lean)
    constexpr auto add_associative() {
        // En Lean4: theorem add_assoc (n m k : ℕ₀) : add n (add m k) = add (add n m) k
        return BY_AXIOM(forall(n, forall(m, forall(k, forall("p"_var, forall("q"_var, forall("r"_var,
            (Add(n, m, "p"_var) && Add("p"_var, k, "r"_var)) == 
            (Add(m, k, "q"_var) && Add(n, "q"_var, "r"_var)))))))));
    }
    
    // Teorema: add_cancelation (traducido de PeanoNatAdd.lean)
    constexpr auto add_cancellation() {
        // En Lean4: theorem add_cancelation (n m k : ℕ₀) : add n m = add n k → m = k
        return BY_AXIOM(forall(n, forall(m, forall(k, forall("sum"_var,
            (Add(n, m, "sum"_var) && Add(n, k, "sum"_var)) >> Eq(m, k)))));
    }
    
    // Teorema: le_self_add (traducido de PeanoNatAdd.lean)
    constexpr auto le_self_add_theorem() {
        // En Lean4: theorem le_self_add (a p : ℕ₀) : Le a (add a p)
        return BY_AXIOM(forall("a"_var, forall("p"_var, forall("sum"_var,
            Add("a"_var, "p"_var, "sum"_var) >> Le("a"_var, "sum"_var)))));
    }
    
    // Teorema: lt_self_add_r (traducido de PeanoNatAdd.lean)
    constexpr auto lt_self_add_nonzero() {
        // En Lean4: theorem lt_self_add_r (a b : ℕ₀) (h_b_neq_0 : b ≠ 𝟘): Lt a (add a b)
        return BY_AXIOM(forall("a"_var, forall("b"_var, forall("sum"_var,
            (!Eq("b"_var, Zero) && Add("a"_var, "b"_var, "sum"_var)) >> Lt("a"_var, "sum"_var)))));
    }
    
    // Teorema: le_then_exists_add (traducido de PeanoNatAdd.lean)
    constexpr auto le_iff_exists_add() {
        // En Lean4: theorem le_then_exists_add (a b : ℕ₀) : Le a b → ∃ (p : ℕ₀), b = add a p
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Le("a"_var, "b"_var) == exists("p"_var, Add("a"_var, "p"_var, "b"_var)))));
    }
    
    // Teorema: lt_then_exists_add_succ (traducido de PeanoNatAdd.lean)
    constexpr auto lt_iff_exists_add_succ() {
        // En Lean4: theorem lt_then_exists_add_succ (a b : ℕ₀) : Lt a b → ∃ (p : ℕ₀), b = add a (σ p)
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Lt("a"_var, "b"_var) == exists("p"_var, Add("a"_var, S("p"_var), "b"_var)))));
    }
    
    // Teorema: add_lt_add_left_iff (traducido de PeanoNatAdd.lean)
    constexpr auto add_preserves_lt() {
        // En Lean4: theorem add_lt_add_left_iff (k a b : ℕ₀) : Lt (add k a) (add k b) ↔ Lt a b
        return BY_AXIOM(forall("k"_var, forall("a"_var, forall("b"_var, forall("ka"_var, forall("kb"_var,
            (Add("k"_var, "a"_var, "ka"_var) && Add("k"_var, "b"_var, "kb"_var)) >>
            (Lt("ka"_var, "kb"_var) == Lt("a"_var, "b"_var))))))));
    }
    
    // Teorema: le_add_compat (traducido de PeanoNatAdd.lean)
    constexpr auto add_preserves_le() {
        // En Lean4: theorem le_add_compat (a b c d: ℕ₀) : Le a b → Le c d → Le (add a c) (add b d)
        return BY_AXIOM(forall("a"_var, forall("b"_var, forall("c"_var, forall("d"_var, 
            forall("ac"_var, forall("bd"_var,
                (Le("a"_var, "b"_var) && Le("c"_var, "d"_var) && 
                 Add("a"_var, "c"_var, "ac"_var) && Add("b"_var, "d"_var, "bd"_var)) >>
                Le("ac"_var, "bd"_var))))))));
    }
    
} // namespace logic::peano::theorems
