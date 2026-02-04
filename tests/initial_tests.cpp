// Este fichero valida el Motor de Inferencia (Fase 2)
// Usa nombres ASCII para máxima portabilidad.

#include <logic_language/logic_language.hpp>
#include <type_traits>

using namespace logic;

// Helper robusto: Ignora const/volatile top-level para comparaciones
template <typename T, typename U>
constexpr bool check_type = std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;

// Alias para limpiar const de decltypes
template <typename T>
using clean_t = std::remove_cv_t<T>;

int main()
{
    // --- 1. Definición de variables ---
    constexpr auto x = "x"_var;
    constexpr auto y = "y"_var;

    // --- 2. PRUEBA DE AXIOMAS (Identity) ---
    // Queremos probar: |- P(x) -> P(x)
    constexpr auto p_x = P(x);

    // AxiomIdentity crea un teorema de la forma A -> A
    constexpr auto thm_id = AxiomIdentity(p_x);

    // Verificar que el resultado es un Theorem<P(x) -> P(x)>
    using ExpectedIdentity = Theorem<Implies<clean_t<decltype(p_x)>, clean_t<decltype(p_x)>>>;

    static_assert(check_type<decltype(thm_id), ExpectedIdentity>,
                  "AxiomIdentity debe retornar un Theorem P -> P");

    // --- 3. PRUEBA DE MODUS PONENS ---
    // Premisa 1: |- A  (Simulada con AxiomIdentity para tener un teorema válido A->A)
    // Premisa 2: |- (A->A) -> B

    // T1: P->P
    constexpr auto thm_base = AxiomIdentity(p_x);

    // T2: (P->P) -> (P->P)
    // CORRECCIÓN: Accedemos al tipo interno del teorema usando decltype
    using FormulaBase = typename decltype(thm_base)::type;
    constexpr auto thm_impl = AxiomIdentity(FormulaBase{});

    // Aplicamos Modus Ponens:
    // Dados: A y A -> B
    // Deduce: B
    // Aquí: A = (P->P), B = (P->P)
    constexpr auto thm_res = ModusPonens(thm_base, thm_impl);

    static_assert(check_type<decltype(thm_res), decltype(thm_base)>,
                  "Modus Ponens falló en deducir el consecuente");

    // --- 4. PRUEBA DE MOTOR DE SUSTITUCIÓN ---
    // Queremos sustituir 'x' por 'y' en: P(x) && Q(x, x)
    using Formula = decltype(P(x) && Q(x, x));

    // Substitute_t<Formula, Var<"x">, Var<"y">>
    using Substituted = Substitute_t<clean_t<Formula>, clean_t<decltype(x)>, clean_t<decltype(y)>>;

    // Resultado esperado: P(y) && Q(y, y)
    using ExpectedSubst = And<Predicate<"P", clean_t<decltype(y)>>, Predicate<"Q", clean_t<decltype(y)>, clean_t<decltype(y)>>>;

    static_assert(check_type<Substituted, ExpectedSubst>,
                  "El motor de sustitución falló al reemplazar x por y");

    // --- 5. PRUEBA DE INSTANCIACIÓN UNIVERSAL ---
    // Teorema: forall x, (P(x) -> P(x))
    // Objetivo: P(y) -> P(y)

    // Paso 1: Generalizar el axioma de identidad para obtener un "forall"
    // |- forall x, (P(x) -> P(x))
    constexpr auto thm_gen = Generalization(x, thm_id);

    // Paso 2: Instanciar (Particularizar) para 'y'
    // forall x, A(x)  --->  A(y)
    constexpr auto thm_inst = UniversalInstantiation(thm_gen, y);

    // El resultado debe ser P(y) -> P(y)
    using ExpectedInst = Implies<Predicate<"P", clean_t<decltype(y)>>, Predicate<"P", clean_t<decltype(y)>>>;

    static_assert(check_type<decltype(thm_inst), Theorem<ExpectedInst>>,
                  "Universal Instantiation falló al sustituir la variable ligada");

    // --- 6. USO DE FUNCIONES ASCII (sin Unicode) ---
    // forall en lugar de ∀
    constexpr auto formula_ascii = forall(x, [=](auto x_inner)
                                          {
        // implies (o >>) en lugar de →
        return implies(P(x_inner), Q(x_inner)); });

    using ExpectedAscii = Forall<clean_t<decltype(x)>, Implies<Predicate<"P", clean_t<decltype(x)>>, Predicate<"Q", clean_t<decltype(x)>>>>;

    static_assert(check_type<decltype(formula_ascii), ExpectedAscii>, "Fallo en sintaxis ASCII");

    return 0;
}