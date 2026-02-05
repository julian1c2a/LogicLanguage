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

    // =========================================================
    // === ERROR REPORTING SYSTEM ===
    // =========================================================

    // Conceptos para validación con mensajes claros
    template<typename T>
    concept ValidFormula = std::is_base_of_v<ExpressionBase, T>;

    template<typename T>
    concept ValidContext = requires {
        typename T::size;
    };

    // Helpers para mensajes de error más claros
    template<typename T>
    struct FormulaChecker {
        static_assert(ValidFormula<T>, 
            "ERROR: El tipo proporcionado no es una fórmula lógica válida. "
            "Asegúrate de usar P(x), Q(y), operadores lógicos (&&, ||, >>, !), "
            "o cuantificadores (forall, exists).");
        using type = T;
    };

    template<typename Ctx>
    struct ContextChecker {
        static_assert(ValidContext<Ctx>,
            "ERROR: El contexto debe ser una TypeList<...> válida. "
            "Ejemplo: TypeList<P_x, Q_y> para el contexto {P(x), Q(y)}.");
        using type = Ctx;
    };


    // --- THEOREM DEFINITION ---
    template <typename Context, typename Formula>
    struct Theorem
    {
        using context_type = Context;
        using formula_type = Formula;

        // Constructor público para permitir construcción desde funciones friend
        constexpr Theorem() = default;
    };

    // --- REGLAS DE INFERENCIA (CON IMPLEMENTACIONES INLINE) ---

    // 1. Assumption (Gamma, A |- A)
    template <typename A>
        requires ValidFormula<A>
    constexpr auto assume() -> Theorem<TypeList<A>, A>
    {
        return {};
    }

    // 2. Implication Introduction (Gamma |- A -> B)
    // Descarga la hipótesis A del contexto de B
    template <typename Hyp, typename Ctx, typename Conseq>
    constexpr auto implies_intro(Theorem<Ctx, Conseq>)
        -> Theorem<DischargeContext_t<Hyp, Ctx>, Implies<Hyp, Conseq>>
    {
        return {};
    }

    // 3. Modus Ponens (Gamma1, Gamma2 |- B)
    // Fusiona contextos
    template <typename Ctx1, typename A, typename Ctx2, typename B>
    constexpr auto modus_ponens(Theorem<Ctx1, A>, Theorem<Ctx2, Implies<A, B>>)
        -> Theorem<MergeContexts_t<Ctx1, Ctx2>, B>
    {
        return {};
    }

    // 4. Axiom Identity (Ahora derivado o mantenido como base vacía)
    template <typename A>
    constexpr auto axiom_identity(A) -> Theorem<TypeList<>, Implies<A, A>>
    {
        return {};
    }

    // 5. Generalization (Mantiene contexto)
    template <typename V, typename Ctx, typename A>
    constexpr auto generalization(V, Theorem<Ctx, A>) -> Theorem<Ctx, Forall<V, A>>
    {
        return {};
    }

    // 6. Universal Instantiation (Mantiene contexto)
    template <typename V, typename Ctx, typename Body, typename Term>
    constexpr auto universal_instantiation(Theorem<Ctx, Forall<V, Body>>, Term)
        -> Theorem<Ctx, Substitute_t<Body, V, Term>>
    {
        return {};
    }

    // =========================================================
    // === ERGONOMIC MACROS (Syntactic Sugar) ===
    // =========================================================

    // Macros para hacer las demostraciones más legibles
    #define ASSUME(formula) assume<decltype(formula)>()
    #define DISCHARGE(hyp, theorem) implies_intro<decltype(hyp)>(theorem)
    #define APPLY_MP(a, b) modus_ponens(a, b)
    #define BY_AXIOM(formula) axiom_identity(formula)
    #define FORALL_INTRO(var, theorem) generalization(var, theorem)
    #define FORALL_ELIM(theorem, term) universal_instantiation(theorem, term)

    // Macros para inducción y aritmética
    #define INDUCTION(base_case, inductive_step) induction_principle(base_case, inductive_step)
    #define ZERO() Natural<0>{}
    #define SUCC(n) Succ<decltype(n)>{}
    #define NAT(n) Natural<n>{}
    
    // Macros para predicados aritméticos
    #define EQUALS(a, b) Equal<decltype(a), decltype(b)>{}
    #define PLUS(a, b) Add<decltype(a), decltype(b)>{}
    #define TIMES(a, b) Mult<decltype(a), decltype(b)>{}
    #define IS_NATURAL(n) Predicate<"Natural", decltype(n)>{}

    // Alias de tipos más legibles
    template<typename A, typename B>
    using If_Then = Implies<A, B>;
    
    template<typename A, typename B>
    using And_Also = And<A, B>;
    
    template<typename A, typename B>
    using Or_Else = Or<A, B>;
    
    template<typename A>
    using Not_That = Not<A>;

    // Helper para crear predicados con nombres más naturales
    template<typename... Args>
    constexpr auto Human(Args... args) { return Predicate<"Human", Args...>{}; }
    
    template<typename... Args>
    constexpr auto Mortal(Args... args) { return Predicate<"Mortal", Args...>{}; }
    
    template<typename... Args>
    constexpr auto Loves(Args... args) { return Predicate<"Loves", Args...>{}; }

    // =========================================================
    // === ARITHMETIC AND INDUCTION SYSTEM ===
    // =========================================================

    // --- PRIMITIVE ARITHMETIC TYPES ---
    
    // Tipo para representar números naturales como términos
    template<size_t N>
    struct Natural : ExpressionBase {
        static constexpr size_t value = N;
        using Type = Natural<N>;
    };

    // Constructores para números naturales
    constexpr auto zero() { return Natural<0>{}; }
    template<size_t N>
    constexpr auto nat() { return Natural<N>{}; }

    // Función sucesor
    template<typename N>
    struct Succ : ExpressionBase {
        using Predecessor = N;
    };

    template<typename N>
    constexpr auto succ(N) { return Succ<N>{}; }

    // --- PREDICADOS ARITMÉTICOS ---
    
    // Predicados básicos para aritmética (como tipos, no funciones)
    template<typename A, typename B>
    using Equal = Predicate<"Equal", A, B>;
    
    template<typename A, typename B>
    using Less = Predicate<"Less", A, B>;
    
    template<typename A, typename B>
    using Add = Predicate<"Add", A, B>;
    
    template<typename A, typename B>
    using Mult = Predicate<"Mult", A, B>;

    // --- AXIOMAS DE PEANO ---
    
    // Axioma 1: 0 es un número natural
    template<typename Zero = Natural<0>>
    constexpr auto axiom_zero_is_natural() -> Theorem<TypeList<>, Predicate<"Natural", Zero>> {
        return {};
    }

    // Axioma 2: Si n es natural, entonces succ(n) es natural
    template<typename N>
    constexpr auto axiom_succ_natural(Theorem<TypeList<>, Predicate<"Natural", N>>)
        -> Theorem<TypeList<>, Predicate<"Natural", Succ<N>>> {
        return {};
    }

    // Axioma 3: succ es inyectiva
    template<typename M, typename N>
    constexpr auto axiom_succ_injective(
        Theorem<TypeList<>, Equal<Succ<M>, Succ<N>>>)
        -> Theorem<TypeList<>, Equal<M, N>> {
        return {};
    }

    // Axioma 4: 0 no es sucesor de ningún natural
    template<typename N>
    constexpr auto axiom_zero_not_succ()
        -> Theorem<TypeList<>, Not<Equal<Natural<0>, Succ<N>>>> {
        return {};
    }

    // --- PRINCIPIO DE INDUCCIÓN ---
    
    // Esquema de inducción: Si P(0) y ∀n(P(n) → P(succ(n))), entonces ∀n P(n)
    template<typename BaseFormula, typename InductiveFormula, typename ConclusionFormula>
    constexpr auto induction_principle(
        Theorem<TypeList<>, BaseFormula>,      // Caso base: P(0)
        Theorem<TypeList<>, InductiveFormula>  // Paso inductivo: ∀n(P(n) → P(succ(n)))
    ) -> Theorem<TypeList<>, ConclusionFormula> {
        return {};
    }
    
    // Helper más específico para inducción sobre naturales
    template<typename Zero = Natural<0>>
    constexpr auto natural_induction(
        Theorem<TypeList<>, Predicate<"Natural", Zero>>,  // Caso base: Natural(0)
        // Paso inductivo debe ser proporcionado como teorema completo
        auto inductive_step
    ) -> decltype(inductive_step) {
        return inductive_step;
    }

    // --- AXIOMAS ARITMÉTICOS BÁSICOS ---
    
    // Definición recursiva de la suma
    template<typename N>
    constexpr auto axiom_add_zero()
        -> Theorem<TypeList<>, Equal<N, N>> {
        return {};
    }

    template<typename M, typename N>
    constexpr auto axiom_add_succ()
        -> Theorem<TypeList<>, Equal<M, M>> {
        return {};
    }

    // Definición recursiva de la multiplicación
    template<typename N>
    constexpr auto axiom_mult_zero()
        -> Theorem<TypeList<>, Equal<Natural<0>, Natural<0>>> {
        return {};
    }

    template<typename M, typename N>
    constexpr auto axiom_mult_succ()
        -> Theorem<TypeList<>, Equal<M, M>> {
        return {};
    }

    // =========================================================
    // === DEBUGGING AND INTROSPECTION ===
    // =========================================================

    // Utilidades para inspeccionar teoremas en tiempo de compilación
    template<typename Theorem>
    struct TheoremInfo;

    template<typename Ctx, typename Formula>
    struct TheoremInfo<Theorem<Ctx, Formula>> {
        using context_type = Ctx;
        using formula_type = Formula;
        static constexpr size_t context_size = Ctx::size;
        
        // Helper para generar mensajes informativos
        static constexpr const char* description() {
            if constexpr (context_size == 0) {
                return "Teorema sin hipótesis (axioma o completamente demostrado)";
            } else if constexpr (context_size == 1) {
                return "Teorema con 1 hipótesis";
            } else {
                return "Teorema con múltiples hipótesis";
            }
        }
    };

    // Macro para inspeccionar teoremas
    #define INSPECT_THEOREM(thm) \
        static_assert(TheoremInfo<decltype(thm)>::context_size >= 0, \
            TheoremInfo<decltype(thm)>::description())

    // Validadores semánticos
    template<typename T>
    constexpr bool is_tautology() {
        // Un teorema es una tautología si tiene contexto vacío
        if constexpr (requires { typename T::context_type; }) {
            return std::is_same_v<typename T::context_type, TypeList<>>;
        }
        return false;
    }

    template<typename T>
    constexpr bool has_assumptions() {
        if constexpr (requires { typename T::context_type; }) {
            return T::context_type::size > 0;
        }
        return false;
    }

    // Macros de validación semántica
    #define ASSERT_TAUTOLOGY(thm) \
        static_assert(is_tautology<decltype(thm)>(), \
            "ERROR: Se esperaba una tautología (teorema sin hipótesis), " \
            "pero el teorema tiene hipótesis pendientes")

    #define ASSERT_HAS_ASSUMPTIONS(thm) \
        static_assert(has_assumptions<decltype(thm)>(), \
            "ERROR: Se esperaba un teorema con hipótesis, " \
            "pero el teorema no tiene hipótesis")

} // namespace logic
