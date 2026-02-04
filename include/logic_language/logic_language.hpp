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
    // Guía de deducción explícita para ayudar al compilador
    template <size_t N>
    FixedString(const char (&)[N]) -> FixedString<N>;

    // Helper para static_assert
    template <typename T>
    struct always_false : std::false_type
    {
    };

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
        // Identidad para sustitución
        using Type = Var<Name>;
    };

    template <FixedString S>
    constexpr auto operator""_var() { return Var<S>{}; }

    // --- 2. PREDICADOS ---
    template <FixedString Name, typename... Args>
    struct Predicate : ExpressionBase
    {
        using ArgTypes = std::tuple<Args...>;
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

    template <typename V, typename Func>
    constexpr auto exists(V var, Func f)
    {
        using BodyType = decltype(f(var));
        return Exists<V, BodyType>{};
    }

    template <typename... Args>
    constexpr auto P(Args... args) { return Predicate<"P", Args...>{}; }
    template <typename... Args>
    constexpr auto Q(Args... args) { return Predicate<"Q", Args...>{}; }

    // Funciones helper ASCII para evitar operadores si se prefiere estilo funcional
    template <LogicExpression L, LogicExpression R>
    constexpr auto implies(L, R) { return Implies<L, R>{}; }

    template <LogicExpression L, LogicExpression R>
    constexpr auto and_(L, R) { return And<L, R>{}; }

    template <LogicExpression L, LogicExpression R>
    constexpr auto or_(L, R) { return Or<L, R>{}; }

    template <LogicExpression L, LogicExpression R>
    constexpr auto equiv(L, R) { return Equiv<L, R>{}; }

    template <LogicExpression T>
    constexpr auto not_(T) { return Not<T>{}; }

    // =========================================================
    // === SUBSTITUTION ENGINE (Metaprogramming) ===
    // =========================================================

    template <typename Node, typename Target, typename Replacement>
    struct Substitute;

    // Alias de conveniencia
    template <typename Node, typename Target, typename Replacement>
    using Substitute_t = typename Substitute<Node, Target, Replacement>::type;

    // 1. Caso Var
    template <auto N, typename Target, typename Replacement>
    struct Substitute<Var<N>, Target, Replacement>
    {
        using type = std::conditional_t<std::is_same_v<Var<N>, Target>, Replacement, Var<N>>;
    };

    // 2. Caso Predicate: Sustitución variádica recursiva
    template <auto N, typename... Args, typename Target, typename Replacement>
    struct Substitute<Predicate<N, Args...>, Target, Replacement>
    {
        using type = Predicate<N, Substitute_t<Args, Target, Replacement>...>;
    };

    // 3. Operadores Binarios
    template <typename L, typename R, typename T, typename Rep>
    struct Substitute<And<L, R>, T, Rep>
    {
        using type = And<Substitute_t<L, T, Rep>, Substitute_t<R, T, Rep>>;
    };

    template <typename L, typename R, typename T, typename Rep>
    struct Substitute<Or<L, R>, T, Rep>
    {
        using type = Or<Substitute_t<L, T, Rep>, Substitute_t<R, T, Rep>>;
    };

    template <typename L, typename R, typename T, typename Rep>
    struct Substitute<Implies<L, R>, T, Rep>
    {
        using type = Implies<Substitute_t<L, T, Rep>, Substitute_t<R, T, Rep>>;
    };

    template <typename L, typename R, typename T, typename Rep>
    struct Substitute<Equiv<L, R>, T, Rep>
    {
        using type = Equiv<Substitute_t<L, T, Rep>, Substitute_t<R, T, Rep>>;
    };

    // 4. Operador Unario
    template <typename Op, typename T, typename Rep>
    struct Substitute<Not<Op>, T, Rep>
    {
        using type = Not<Substitute_t<Op, T, Rep>>;
    };

    // 5. Cuantificadores
    template <typename V, typename Body, typename T, typename Rep>
    struct Substitute<Forall<V, Body>, T, Rep>
    {
        using type = std::conditional_t<
            std::is_same_v<V, T>,
            Forall<V, Body>,                      // Shadowing: No sustituir dentro
            Forall<V, Substitute_t<Body, T, Rep>> // Recurrir
            >;
    };

    template <typename V, typename Body, typename T, typename Rep>
    struct Substitute<Exists<V, Body>, T, Rep>
    {
        using type = std::conditional_t<
            std::is_same_v<V, T>,
            Exists<V, Body>,                      // Shadowing: No sustituir dentro
            Exists<V, Substitute_t<Body, T, Rep>> // Recurrir
            >;
    };

    // =========================================================
    // === DEDUCTIVE SYSTEM (Kernel) ===
    // =========================================================

    template <typename Formula>
    struct Theorem;

    // --- REGLAS DE INFERENCIA ---

    template <typename A>
    constexpr auto AxiomIdentity(A) -> Theorem<Implies<A, A>>;

    template <typename A, typename B>
    constexpr auto ModusPonens(Theorem<A>, Theorem<Implies<A, B>>) -> Theorem<B>;

    template <typename V, typename A>
    constexpr auto Generalization(V var, Theorem<A> thm) -> Theorem<Forall<V, A>>;

    template <typename V, typename Body, typename Term>
    constexpr auto UniversalInstantiation(Theorem<Forall<V, Body>>, Term)
        -> Theorem<Substitute_t<Body, V, Term>>;

    // --- THEOREM WRAPPER ---
    template <typename Formula>
    struct Theorem
    {
        using type = Formula;

    private:
        constexpr Theorem() = default;

        template <typename A>
        friend constexpr auto AxiomIdentity(A) -> Theorem<Implies<A, A>>;

        template <typename A, typename B>
        friend constexpr auto ModusPonens(Theorem<A>, Theorem<Implies<A, B>>) -> Theorem<B>;

        template <typename V, typename A>
        friend constexpr auto Generalization(V, Theorem<A>) -> Theorem<Forall<V, A>>;

        template <typename V, typename B, typename Term>
        friend constexpr auto UniversalInstantiation(Theorem<Forall<V, B>>, Term)
            -> Theorem<Substitute_t<B, V, Term>>;
    };

    // --- IMPLEMENTACIÓN ---

    template <typename A>
    constexpr auto AxiomIdentity(A) -> Theorem<Implies<A, A>>
    {
        return {};
    }

    template <typename A, typename B>
    constexpr auto ModusPonens(Theorem<A>, Theorem<Implies<A, B>>) -> Theorem<B>
    {
        return {};
    }

    template <typename V, typename A>
    constexpr auto Generalization(V, Theorem<A>) -> Theorem<Forall<V, A>>
    {
        return {};
    }

    template <typename V, typename Body, typename Term>
    constexpr auto UniversalInstantiation(Theorem<Forall<V, Body>>, Term)
        -> Theorem<Substitute_t<Body, V, Term>>
    {
        return {};
    }

} // namespace logic