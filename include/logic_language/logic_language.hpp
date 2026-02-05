#pragma once

#include <type_traits>
#include <concepts>
#include <tuple>
#include <cstddef>
#include <algorithm>

namespace logic
{

    // =========================================================
    // === CORE SYNTAX (Expression Templates) ===
    // =========================================================

    template <size_t N>
    struct FixedString
    {
        char buf[N]{};
        consteval FixedString(const char (&arg)[N])
        {
            std::copy_n(arg, N, buf);
        }
        auto operator<=>(const FixedString &) const = default;
    };
    template <size_t N>
    FixedString(const char (&)[N]) -> FixedString<N>;

    template <typename T>
    concept LogicExpression = requires { typename T::IsLogicExpression; };

    struct ExpressionBase
    {
        using IsLogicExpression = void;
    };

    // --- 1. VARIABLES ---
    template <FixedString Name>
    struct Var : ExpressionBase
    {
        static constexpr auto name = Name;
        using Type = Var<Name>;
    };

    template <FixedString S>
    constexpr auto operator""_var() { return Var<S>{}; }

    // --- 2. PREDICADOS ---
    template <FixedString Name, typename... Args>
    struct Predicate : ExpressionBase
    {
        static constexpr auto name = Name;
    };

    // --- 3. OPERADORES ---
    template <typename T>
    struct Not : ExpressionBase
    {
        using Operand = T;
    };

    template <typename L, typename R>
    struct And : ExpressionBase
    {
        using Left = L;
        using Right = R;
    };

    template <typename L, typename R>
    struct Or : ExpressionBase
    {
        using Left = L;
        using Right = R;
    };

    template <typename L, typename R>
    struct Implies : ExpressionBase
    {
        using Left = L;
        using Right = R;
    };

    template <typename L, typename R>
    struct Equiv : ExpressionBase
    {
        using Left = L;
        using Right = R;
    };

    // --- 4. CUANTIFICADORES ---
    template <typename V, typename Body>
    struct Forall : ExpressionBase
    {
        using Variable = V;
        using Formula = Body;
    };

    template <typename V, typename Body>
    struct Exists : ExpressionBase
    {
        using Variable = V;
        using Formula = Body;
    };

    // --- CONSTRUCTORES ---
    template <LogicExpression L, LogicExpression R>
    constexpr auto operator&&(L, R) { return And<L, R>{}; }
    template <LogicExpression L, LogicExpression R>
    constexpr auto operator||(L, R) { return Or<L, R>{}; }
    template <LogicExpression L, LogicExpression R>
    constexpr auto operator>>(L, R) { return Implies<L, R>{}; }
    template <LogicExpression L, LogicExpression R>
    constexpr auto operator==(L, R) { return Equiv<L, R>{}; }
    template <LogicExpression T>
    constexpr auto operator!(T) { return Not<T>{}; }

    template <typename V, typename Func>
    constexpr auto forall(V var, Func f)
    {
        using BodyType = decltype(f(var));
        return Forall<V, BodyType>{};
    }

    template <typename... Args>
    constexpr auto P(Args... args) { return Predicate<"P", Args...>{}; }
    template <typename... Args>
    constexpr auto Q(Args... args) { return Predicate<"Q", Args...>{}; }

    // =========================================================
    // === SUBSTITUTION ENGINE (Metaprogramming) ===
    // =========================================================

    template <typename Node, typename Target, typename Replacement>
    struct Substitute;

    template <typename Node, typename Target, typename Replacement>
    using Substitute_t = typename Substitute<Node, Target, Replacement>::type;

    // 1. Caso Var
    template <auto N, typename Target, typename Replacement>
    struct Substitute<Var<N>, Target, Replacement>
    {
        using type = std::conditional_t<std::is_same_v<Var<N>, Target>, Replacement, Var<N>>;
    };

    // 2. Caso Predicate
    template <auto N, typename... Args, typename Target, typename Replacement>
    struct Substitute<Predicate<N, Args...>, Target, Replacement>
    {
        using type = Predicate<N, Substitute_t<Args, Target, Replacement>...>;
    };

    // 3. Operador Unario
    template <typename Op, typename T, typename Rep>
    struct Substitute<Not<Op>, T, Rep>
    {
        using type = Not<Substitute_t<Op, T, Rep>>;
    };

    // 4. Cuantificadores (especializaciones específicas para evitar ambigüedad)
    template <typename V, typename Body, typename T, typename Rep>
    struct Substitute<Forall<V, Body>, T, Rep>
    {
        using type = std::conditional_t<
            std::is_same_v<std::remove_cv_t<V>, std::remove_cv_t<T>>,
            Forall<V, Body>, // Shadowing
            Forall<V, Substitute_t<Body, T, Rep>>>;
    };

    template <typename V, typename Body, typename T, typename Rep>
    struct Substitute<Exists<V, Body>, T, Rep>
    {
        using type = std::conditional_t<
            std::is_same_v<std::remove_cv_t<V>, std::remove_cv_t<T>>,
            Exists<V, Body>, // Shadowing
            Exists<V, Substitute_t<Body, T, Rep>>>;
    };

    // 5. Operadores Binarios (debe ir después de cuantificadores para evitar ambigüedad)
    template <template <typename, typename> class Op, typename L, typename R, typename T, typename Rep>
        requires std::is_base_of_v<ExpressionBase, Op<L, R>>
    struct Substitute<Op<L, R>, T, Rep>
    {
        using type = Op<Substitute_t<L, T, Rep>, Substitute_t<R, T, Rep>>;
    };

    // =========================================================
    // === CONTEXT MANAGEMENT (TypeList Ops) ===
    // =========================================================

    // Contenedor básico de hipótesis
    template <typename... Ts>
    struct TypeList
    {
        static constexpr size_t size = sizeof...(Ts);
    };

    // --- Concat (Unión de contextos) ---
    template <typename List1, typename List2>
    struct ConcatLists;

    template <typename... As, typename... Bs>
    struct ConcatLists<TypeList<As...>, TypeList<Bs...>>
    {
        using type = TypeList<As..., Bs...>;
    };

    // --- Remove (Para descargar hipótesis) ---
    // Elimina TODAS las ocurrencias del tipo T de la lista
    template <typename Target, typename List>
    struct RemoveType;

    template <typename Target>
    struct RemoveType<Target, TypeList<>>
    {
        using type = TypeList<>;
    };

    template <typename Target, typename Head, typename... Tail>
    struct RemoveType<Target, TypeList<Head, Tail...>>
    {
        using TailRemoved = typename RemoveType<Target, TypeList<Tail...>>::type;
        // Si Head es Target, no lo incluimos. Si es distinto, lo concatenamos.
        using type = std::conditional_t<
            std::is_same_v<Target, Head>,
            TailRemoved,
            typename ConcatLists<TypeList<Head>, TailRemoved>::type>;
    };

    // --- Unique (Para evitar contextos gigantes con duplicados) ---
    // Nota: Para una implementación completa, se necesitaría un 'Filter' o 'Unique' más complejo.
    // Por ahora, permitiremos duplicados en la lista pero 'Remove' los quitará todos.
    // Alias helper:
    template <typename L1, typename L2>
    using MergeContexts_t = typename ConcatLists<L1, L2>::type;

    template <typename T, typename List>
    using DischargeContext_t = typename RemoveType<T, List>::type;

    // =========================================================
    // === DEDUCTIVE SYSTEM (Natural Deduction) ===
    // =========================================================

    // Theorem ahora lleva el Contexto (Gamma |- Formula)
    template <typename Context, typename Formula>
    struct Theorem;

    // --- REGLAS DE INFERENCIA ---

    // 1. Assumption (Gamma, A |- A)
    template <typename A>
    constexpr auto assume() -> Theorem<TypeList<A>, A>;

    // 2. Implication Introduction (Gamma |- A -> B)
    // Descarga la hipótesis A del contexto de B
    template <typename Hyp, typename Ctx, typename Conseq>
    constexpr auto implies_intro(Theorem<Ctx, Conseq>)
        -> Theorem<DischargeContext_t<Hyp, Ctx>, Implies<Hyp, Conseq>>;

    // 3. Modus Ponens (Gamma1, Gamma2 |- B)
    // Fusiona contextos
    template <typename Ctx1, typename A, typename Ctx2, typename B>
    constexpr auto modus_ponens(Theorem<Ctx1, A>, Theorem<Ctx2, Implies<A, B>>)
        -> Theorem<MergeContexts_t<Ctx1, Ctx2>, B>;

    // 4. Axiom Identity (Ahora derivado o mantenido como base vacía)
    template <typename A>
    constexpr auto axiom_identity(A) -> Theorem<TypeList<>, Implies<A, A>>;

    // 5. Generalization (Mantiene contexto)
    template <typename V, typename Ctx, typename A>
    constexpr auto generalization(V var, Theorem<Ctx, A> thm) -> Theorem<Ctx, Forall<V, A>>;

    // 6. Universal Instantiation (Mantiene contexto)
    template <typename V, typename Ctx, typename Body, typename Term>
    constexpr auto universal_instantiation(Theorem<Ctx, Forall<V, Body>>, Term)
        -> Theorem<Ctx, Substitute_t<Body, V, Term>>;

    // --- THEOREM DEFINITION ---
    template <typename Context, typename Formula>
    struct Theorem
    {
        using context_type = Context;
        using formula_type = Formula;

    private:
        constexpr Theorem() = default;

        // Friend declarations
        template <typename A>
        friend constexpr auto assume() -> Theorem<TypeList<A>, A>;

        template <typename Hyp, typename C, typename Cons>
        friend constexpr auto implies_intro(Theorem<C, Cons>)
            -> Theorem<DischargeContext_t<Hyp, C>, Implies<Hyp, Cons>>;

        template <typename C1, typename X, typename C2, typename Y>
        friend constexpr auto modus_ponens(Theorem<C1, X>, Theorem<C2, Implies<X, Y>>)
            -> Theorem<MergeContexts_t<C1, C2>, Y>;

        template <typename A>
        friend constexpr auto axiom_identity(A) -> Theorem<TypeList<>, Implies<A, A>>;

        template <typename V, typename C, typename A>
        friend constexpr auto generalization(V, Theorem<C, A>) -> Theorem<C, Forall<V, A>>;

        template <typename V, typename C, typename B, typename T>
        friend constexpr auto universal_instantiation(Theorem<C, Forall<V, B>>, T)
            -> Theorem<C, Substitute_t<B, V, T>>;
    };

    // --- IMPLEMENTACIÓN INLINE ---

    template <typename A>
    constexpr auto assume() -> Theorem<TypeList<A>, A>
    {
        return {};
    }

    template <typename Hyp, typename Ctx, typename Conseq>
    constexpr auto implies_intro(Theorem<Ctx, Conseq>)
        -> Theorem<DischargeContext_t<Hyp, Ctx>, Implies<Hyp, Conseq>>
    {
        return {};
    }

    template <typename Ctx1, typename A, typename Ctx2, typename B>
    constexpr auto modus_ponens(Theorem<Ctx1, A>, Theorem<Ctx2, Implies<A, B>>)
        -> Theorem<MergeContexts_t<Ctx1, Ctx2>, B>
    {
        return {};
    }

    template <typename A>
    constexpr auto axiom_identity(A) -> Theorem<TypeList<>, Implies<A, A>>
    {
        return {};
    }

    template <typename V, typename Ctx, typename A>
    constexpr auto generalization(V, Theorem<Ctx, A>) -> Theorem<Ctx, Forall<V, A>>
    {
        return {};
    }

    template <typename V, typename Ctx, typename Body, typename Term>
    constexpr auto universal_instantiation(Theorem<Ctx, Forall<V, Body>>, Term)
        -> Theorem<Ctx, Substitute_t<Body, V, Term>>
    {
        return {};
    }

} // namespace logic
