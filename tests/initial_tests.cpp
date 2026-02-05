// Este fichero valida el Motor de Inferencia (Fase 2)
// ADAPTADO para Fase 3 (Contextos)

#include <logic_language/logic_language.hpp>
#include <type_traits>

using namespace logic;

template <typename T, typename U>
constexpr bool check_type = std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;

template <typename T>
using clean_t = std::remove_cv_t<T>;

int main()
{
    constexpr auto x = "x"_var;
    constexpr auto y = "y"_var;
    constexpr auto p_x = P(x);

    // --- 2. PRUEBA DE AXIOMAS (Identity) ---
    // AxiomIdentity ahora retorna un teorema con contexto vacío: Theorem<TypeList<>, A->A>
    constexpr auto thm_id = axiom_identity(p_x);

    using FormulaId = Implies<clean_t<decltype(p_x)>, clean_t<decltype(p_x)>>;
    // Verificamos que sea Theorem<TypeList<>, Formula>
    using ExpectedIdentity = Theorem<TypeList<>, FormulaId>;

    static_assert(check_type<decltype(thm_id), ExpectedIdentity>,
                  "AxiomIdentity debe retornar un Theorem P -> P con contexto vacío");

    // --- 3. PRUEBA DE MODUS PONENS ---
    constexpr auto thm_base = axiom_identity(p_x);
    using FormulaBase = typename decltype(thm_base)::formula_type; // Cambio: ::type -> ::formula_type
    constexpr auto thm_impl = axiom_identity(FormulaBase{});

    // Modus ponens ahora fusiona contextos vacíos -> contexto vacío
    constexpr auto thm_res = modus_ponens(thm_base, thm_impl);

    static_assert(check_type<decltype(thm_res), decltype(thm_base)>,
                  "Modus Ponens falló en deducir el consecuente");

    // --- 5. PRUEBA DE INSTANCIACIÓN UNIVERSAL ---
    constexpr auto thm_gen = generalization(x, thm_id);
    constexpr auto thm_inst = universal_instantiation(thm_gen, y);

    using ExpectedInst = Implies<Predicate<"P", clean_t<decltype(y)>>, Predicate<"P", clean_t<decltype(y)>>>;
    // Verificar Teorema completo con contexto vacío
    static_assert(check_type<decltype(thm_inst), Theorem<TypeList<>, ExpectedInst>>,
                  "Universal Instantiation falló");

    return 0;
}