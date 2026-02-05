// Tests Comprehensivos para LogicLanguage - Deducción Natural
// Validación exhaustiva de todas las reglas y casos edge

#include <logic_language/logic_language.hpp>
#include <type_traits>

using namespace logic;

// Helper para verificar tipos ignorando const/volatile
template <typename T, typename U>
constexpr bool check_type = std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;
template <typename T>
using clean_t = std::remove_cv_t<T>;

int main()
{
    // Variables de prueba
    constexpr auto x = "x"_var;
    constexpr auto y = "y"_var;
    constexpr auto z = "z"_var;
    
    using P_x = decltype(P(x));
    using P_y = decltype(P(y));
    using P_z = decltype(P(z));
    using Q_x = decltype(Q(x));
    using Q_y = decltype(Q(y));

    // ==========================================
    // SECCIÓN 1: VALIDACIÓN DE CONTEXTOS BÁSICOS
    // ==========================================
    
    // Test 1.1: Assumption crea contexto correcto
    constexpr auto thm_assume_P = assume<P_x>();
    using Context_P = typename decltype(thm_assume_P)::context_type;
    using Formula_P = typename decltype(thm_assume_P)::formula_type;
    
    static_assert(std::is_same_v<Context_P, TypeList<P_x>>, 
                  "assume<P> debe crear contexto {P}");
    static_assert(std::is_same_v<Formula_P, P_x>, 
                  "assume<P> debe tener fórmula P");

    // Test 1.2: Múltiples assumptions independientes
    constexpr auto thm_assume_Q = assume<Q_x>();
    using Context_Q = typename decltype(thm_assume_Q)::context_type;
    
    static_assert(std::is_same_v<Context_Q, TypeList<Q_x>>, 
                  "assume<Q> debe crear contexto {Q}");
    static_assert(!std::is_same_v<Context_P, Context_Q>, 
                  "Contextos de assumptions diferentes deben ser distintos");

    // ==========================================
    // SECCIÓN 2: IMPLICATION INTRODUCTION (DESCARGA)
    // ==========================================
    
    // Test 2.1: Descarga simple P |- P => |- P -> P
    constexpr auto thm_identity_derived = implies_intro<P_x>(thm_assume_P);
    using Context_Identity = typename decltype(thm_identity_derived)::context_type;
    using Formula_Identity = typename decltype(thm_identity_derived)::formula_type;
    
    static_assert(std::is_same_v<Context_Identity, TypeList<>>, 
                  "Descarga de P debe resultar en contexto vacío");
    static_assert(check_type<Formula_Identity, Implies<P_x, P_x>>, 
                  "Descarga de P |- P debe resultar en P -> P");

    // Test 2.2: Descarga con contexto residual
    // Simulamos {P, Q} |- P para obtener {Q} |- P -> P
    // Primero necesitamos crear un teorema con contexto {P, Q}
    constexpr auto thm_P_in_PQ = assume<P_x>(); // {P} |- P
    constexpr auto thm_Q_separate = assume<Q_x>(); // {Q} |- Q
    
    // Usamos modus_ponens para fusionar contextos artificialmente
    // Necesitamos Q -> P para aplicar MP y obtener {P, Q->P} |- P
    using Q_implies_P = Implies<Q_x, P_x>;
    constexpr auto thm_Q_impl_P = assume<Q_implies_P>();
    constexpr auto thm_P_from_Q = modus_ponens(thm_Q_separate, thm_Q_impl_P);
    
    // Ahora tenemos {Q, Q->P} |- P, descargamos Q
    constexpr auto thm_discharged_Q = implies_intro<Q_x>(thm_P_from_Q);
    using Context_After_Discharge = typename decltype(thm_discharged_Q)::context_type;
    
    static_assert(check_type<Context_After_Discharge, TypeList<Q_implies_P>>, 
                  "Descargar Q de {Q, Q->P} debe dejar {Q->P}");

    // ==========================================
    // SECCIÓN 3: MODUS PONENS Y FUSIÓN DE CONTEXTOS
    // ==========================================
    
    // Test 3.1: MP con contextos vacíos
    constexpr auto thm_axiom_P = axiom_identity(P_x{}); // {} |- P -> P
    constexpr auto thm_axiom_impl = axiom_identity(Implies<P_x, P_x>{}); // {} |- (P->P) -> (P->P)
    constexpr auto thm_mp_empty = modus_ponens(thm_axiom_P, thm_axiom_impl);
    
    using Context_MP_Empty = typename decltype(thm_mp_empty)::context_type;
    static_assert(std::is_same_v<Context_MP_Empty, TypeList<>>, 
                  "MP con contextos vacíos debe resultar en contexto vacío");

    // Test 3.2: MP con contextos no vacíos
    constexpr auto thm_P_assumed = assume<P_x>(); // {P} |- P
    using P_implies_Q = Implies<P_x, Q_x>;
    constexpr auto thm_P_impl_Q = assume<P_implies_Q>(); // {P->Q} |- P->Q
    constexpr auto thm_Q_derived = modus_ponens(thm_P_assumed, thm_P_impl_Q);
    
    using Context_MP_Fusion = typename decltype(thm_Q_derived)::context_type;
    using Expected_Fusion = TypeList<P_x, P_implies_Q>;
    
    static_assert(std::is_same_v<Context_MP_Fusion, Expected_Fusion>, 
                  "MP debe fusionar contextos correctamente");
    static_assert(check_type<typename decltype(thm_Q_derived)::formula_type, Q_x>, 
                  "MP debe derivar la conclusión correcta");

    // Test 3.3: MP en cadena (transitividad)
    using Q_implies_R = Implies<Q_x, P_z>; // Usamos P_z como "R"
    constexpr auto thm_Q_impl_R = assume<Q_implies_R>();
    constexpr auto thm_R_final = modus_ponens(thm_Q_derived, thm_Q_impl_R);
    
    using Context_Chain = typename decltype(thm_R_final)::context_type;
    using Expected_Chain = TypeList<P_x, P_implies_Q, Q_implies_R>;
    
    static_assert(std::is_same_v<Context_Chain, Expected_Chain>, 
                  "MP en cadena debe acumular todos los contextos");

    // ==========================================
    // SECCIÓN 4: AXIOMAS Y REGLAS BÁSICAS
    // ==========================================
    
    // Test 4.1: Axiom Identity con diferentes tipos
    constexpr auto thm_id_complex = axiom_identity(Implies<P_x, Q_x>{});
    using Formula_Complex_Id = typename decltype(thm_id_complex)::formula_type;
    using Expected_Complex = Implies<Implies<P_x, Q_x>, Implies<P_x, Q_x>>;
    
    static_assert(check_type<Formula_Complex_Id, Expected_Complex>, 
                  "Axiom Identity debe funcionar con fórmulas complejas");

    // Test 4.2: Generalization preserva contexto
    constexpr auto thm_before_gen = assume<P_x>(); // {P(x)} |- P(x)
    constexpr auto thm_after_gen = generalization(x, thm_before_gen); // {P(x)} |- ∀x.P(x)
    
    using Context_Gen = typename decltype(thm_after_gen)::context_type;
    using Formula_Gen = typename decltype(thm_after_gen)::formula_type;
    
    static_assert(std::is_same_v<Context_Gen, TypeList<P_x>>, 
                  "Generalization debe preservar el contexto");
    static_assert(check_type<Formula_Gen, Forall<decltype(x), P_x>>, 
                  "Generalization debe crear cuantificador universal");

    // Test 4.3: Universal Instantiation con sustitución
    constexpr auto thm_universal = generalization(x, axiom_identity(P_x{})); // {} |- ∀x.(P(x) -> P(x))
    constexpr auto thm_instantiated = universal_instantiation(thm_universal, y); // {} |- P(y) -> P(y)
    
    using Formula_Inst = typename decltype(thm_instantiated)::formula_type;
    using Expected_Inst = Implies<P_y, P_y>;
    
    static_assert(check_type<Formula_Inst, Expected_Inst>, 
                  "Universal Instantiation debe sustituir correctamente");

    // ==========================================
    // SECCIÓN 5: CASOS EDGE Y VALIDACIONES AVANZADAS
    // ==========================================
    
    // Test 5.1: Descarga de hipótesis que no existe (debe preservar contexto)
    constexpr auto thm_P_only = assume<P_x>(); // {P} |- P
    constexpr auto thm_discharge_absent = implies_intro<Q_x>(thm_P_only); // {P} |- Q -> P
    
    using Context_Discharge_Absent = typename decltype(thm_discharge_absent)::context_type;
    static_assert(std::is_same_v<Context_Discharge_Absent, TypeList<P_x>>, 
                  "Descargar hipótesis ausente debe preservar contexto original");

    // Test 5.2: Teoremas complejos anidados
    // Construir: ((P -> Q) -> P) -> P (Ley de Peirce, no demostrable en lógica intuicionista)
    // Pero podemos construir la estructura de tipos
    using Complex_Formula = Implies<Implies<Implies<P_x, Q_x>, P_x>, P_x>;
    constexpr auto thm_complex_axiom = axiom_identity(Complex_Formula{});
    
    static_assert(check_type<typename decltype(thm_complex_axiom)::formula_type, 
                            Implies<Complex_Formula, Complex_Formula>>, 
                  "Axiom Identity debe manejar fórmulas arbitrariamente complejas");

    // Test 5.3: Verificación de que los contextos no se corrompen
    // Crear múltiples teoremas y verificar independencia
    constexpr auto thm_independent_1 = assume<P_x>();
    constexpr auto thm_independent_2 = assume<Q_x>();
    constexpr auto thm_independent_3 = assume<P_z>();
    
    // Los contextos deben permanecer independientes
    static_assert(!std::is_same_v<typename decltype(thm_independent_1)::context_type,
                                 typename decltype(thm_independent_2)::context_type>, 
                  "Teoremas independientes deben tener contextos diferentes");

    // ==========================================
    // SECCIÓN 6: VALIDACIÓN DEL MOTOR DE SUSTITUCIÓN
    // ==========================================
    
    // Test 6.1: Sustitución en predicados simples
    using Substituted_Simple = Substitute_t<P_x, decltype(x), decltype(y)>;
    static_assert(check_type<Substituted_Simple, P_y>, 
                  "Sustitución simple x->y en P(x) debe dar P(y)");

    // Test 6.2: Sustitución en fórmulas complejas
    using Complex_Before = Implies<P_x, Q_x>;
    using Complex_After = Substitute_t<Complex_Before, decltype(x), decltype(z)>;
    using Expected_After = Implies<P_z, decltype(Q(z))>;
    
    static_assert(check_type<Complex_After, Expected_After>, 
                  "Sustitución debe aplicarse recursivamente en fórmulas complejas");

    // Test 6.3: Sustitución con shadowing en cuantificadores
    using Quantified = Forall<decltype(x), P_x>;
    using Shadowed = Substitute_t<Quantified, decltype(x), decltype(y)>;
    
    static_assert(std::is_same_v<Shadowed, Quantified>, 
                  "Sustitución debe respetar shadowing en cuantificadores");

    // ==========================================
    // SECCIÓN 7: STRESS TESTS Y LÍMITES
    // ==========================================
    
    // Test 7.1: Cadena larga de MP
    constexpr auto thm_chain_1 = assume<P_x>();
    constexpr auto thm_chain_2 = assume<Implies<P_x, Q_x>>();
    constexpr auto thm_chain_3 = assume<Implies<Q_x, P_y>>();
    constexpr auto thm_chain_4 = assume<Implies<P_y, P_z>>();
    
    constexpr auto thm_result_1 = modus_ponens(thm_chain_1, thm_chain_2);
    constexpr auto thm_result_2 = modus_ponens(thm_result_1, thm_chain_3);
    constexpr auto thm_result_3 = modus_ponens(thm_result_2, thm_chain_4);
    
    static_assert(check_type<typename decltype(thm_result_3)::formula_type, P_z>, 
                  "Cadena larga de MP debe funcionar correctamente");

    // Test 7.2: Contexto con muchas hipótesis
    using Large_Context = TypeList<P_x, Q_x, P_y, Q_y, P_z>;
    // Verificar que TypeList puede manejar múltiples tipos
    static_assert(Large_Context::size == 5, 
                  "TypeList debe contar correctamente el número de elementos");

    return 0;
}
