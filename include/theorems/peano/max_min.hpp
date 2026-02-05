#pragma once

#include "axioms.hpp"
#include "order.hpp"

namespace logic::peano::max_min {
    
    // =========================================================
    // === MAX Y MIN (Traducido de PeanoNatMaxMin.lean) ===
    // =========================================================
    
    // Predicados para máximo y mínimo
    template<typename N, typename M, typename R>
    constexpr auto Max(N, M, R) { return Predicate<"Max", N, M, R>{}; }
    
    template<typename N, typename M, typename R>
    constexpr auto Min(N, M, R) { return Predicate<"Min", N, M, R>{}; }
    
    // Teoremas fundamentales (traducidos de PeanoNatMaxMin.lean)
    
    // max_idem: max n n = n
    constexpr auto max_idem() {
        return BY_AXIOM(forall(n, Max(n, n, n)));
    }
    
    // min_idem: min n n = n
    constexpr auto min_idem() {
        return BY_AXIOM(forall(n, Min(n, n, n)));
    }
    
    // min_abs_0: min 0 n = 0
    constexpr auto min_zero_left() {
        return BY_AXIOM(forall(n, Min(Zero, n, Zero)));
    }
    
    // max_not_0: max 0 n = n
    constexpr auto max_zero_left() {
        return BY_AXIOM(forall(n, Max(Zero, n, n)));
    }
    
    // max_comm: max n m = max m n
    constexpr auto max_comm() {
        return BY_AXIOM(forall(n, forall(m, forall("r1"_var, forall("r2"_var,
            (Max(n, m, "r1"_var) && Max(m, n, "r2"_var)) >> Eq("r1"_var, "r2"_var))))));
    }
    
    // min_comm: min n m = min m n
    constexpr auto min_comm() {
        return BY_AXIOM(forall(n, forall(m, forall("r1"_var, forall("r2"_var,
            (Min(n, m, "r1"_var) && Min(m, n, "r2"_var)) >> Eq("r1"_var, "r2"_var))))));
    }
    
    // max_is_any: max n m = n ∨ max n m = m
    constexpr auto max_is_either() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Max(n, m, "r"_var) >> (Eq("r"_var, n) || Eq("r"_var, m))))));
    }
    
    // min_is_any: min n m = n ∨ min n m = m
    constexpr auto min_is_either() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Min(n, m, "r"_var) >> (Eq("r"_var, n) || Eq("r"_var, m))))));
    }
    
    // lt_then_min: Lt a b → min a b = a
    constexpr auto lt_then_min_left() {
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Lt("a"_var, "b"_var) >> Min("a"_var, "b"_var, "a"_var))));
    }
    
    // max_eq_of_lt: Lt a b → max a b = b
    constexpr auto lt_then_max_right() {
        return BY_AXIOM(forall("a"_var, forall("b"_var,
            Lt("a"_var, "b"_var) >> Max("a"_var, "b"_var, "b"_var))));
    }
    
    // le_max_left: Le n (max n m)
    constexpr auto le_max_left() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Max(n, m, "r"_var) >> Le(n, "r"_var)))));
    }
    
    // le_max_right: Le m (max n m)
    constexpr auto le_max_right() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Max(n, m, "r"_var) >> Le(m, "r"_var)))));
    }
    
    // min_le_left: Le (min n m) n
    constexpr auto min_le_left() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Min(n, m, "r"_var) >> Le("r"_var, n)))));
    }
    
    // min_le_right: Le (min n m) m
    constexpr auto min_le_right() {
        return BY_AXIOM(forall(n, forall(m, forall("r"_var,
            Min(n, m, "r"_var) >> Le("r"_var, m)))));
    }
    
    // max_assoc: max (max n m) k = max n (max m k)
    constexpr auto max_associative() {
        return BY_AXIOM(forall(n, forall(m, forall(k, 
            forall("nm"_var, forall("mk"_var, forall("lhs"_var, forall("rhs"_var,
                (Max(n, m, "nm"_var) && Max("nm"_var, k, "lhs"_var) &&
                 Max(m, k, "mk"_var) && Max(n, "mk"_var, "rhs"_var)) >>
                Eq("lhs"_var, "rhs"_var)))))))));
    }
    
    // min_assoc: min (min n m) k = min n (min m k)
    constexpr auto min_associative() {
        return BY_AXIOM(forall(n, forall(m, forall(k, 
            forall("nm"_var, forall("mk"_var, forall("lhs"_var, forall("rhs"_var,
                (Min(n, m, "nm"_var) && Min("nm"_var, k, "lhs"_var) &&
                 Min(m, k, "mk"_var) && Min(n, "mk"_var, "rhs"_var)) >>
                Eq("lhs"_var, "rhs"_var)))))))));
    }
    
    // eq_iff_eq_max_min: n = m ↔ max n m = min n m
    constexpr auto eq_iff_max_eq_min() {
        return BY_AXIOM(forall(n, forall(m, forall("max_r"_var, forall("min_r"_var,
            (Max(n, m, "max_r"_var) && Min(n, m, "min_r"_var)) >>
            (Eq(n, m) == Eq("max_r"_var, "min_r"_var)))))));
    }
    
    // max_distrib_min: max n (min m k) = min (max n m) (max n k)
    constexpr auto max_distributes_over_min() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            forall("mk"_var, forall("nm"_var, forall("nk"_var, forall("lhs"_var, forall("rhs"_var,
                (Min(m, k, "mk"_var) && Max(n, "mk"_var, "lhs"_var) &&
                 Max(n, m, "nm"_var) && Max(n, k, "nk"_var) && Min("nm"_var, "nk"_var, "rhs"_var)) >>
                Eq("lhs"_var, "rhs"_var)))))))));
    }
    
    // min_distrib_max: min n (max m k) = max (min n m) (min n k)
    constexpr auto min_distributes_over_max() {
        return BY_AXIOM(forall(n, forall(m, forall(k,
            forall("mk"_var, forall("nm"_var, forall("nk"_var, forall("lhs"_var, forall("rhs"_var,
                (Max(m, k, "mk"_var) && Min(n, "mk"_var, "lhs"_var) &&
                 Min(n, m, "nm"_var) && Min(n, k, "nk"_var) && Max("nm"_var, "nk"_var, "rhs"_var)) >>
                Eq("lhs"_var, "rhs"_var)))))))));
    }
    
} // namespace logic::peano::max_min
