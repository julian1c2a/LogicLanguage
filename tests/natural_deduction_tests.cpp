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
    // Variables y Predicados
    constexpr auto x = "x"_var;
    using P_x = decltype(P(x));
    using Q_x = decltype(Q(x));

    // ==========================================
    // TEST 1: Identidad (P -> P) via Deducción Natural
    // ==========================================
    // Estrategia:
    // 1. Asumir P(x)           => Contexto: { P(x) } |- P(x)
    // 2. Intro Implicación P(x)=> Contexto: {}       |- P(x) -> P(x)

    // Paso 1: Asumir
    constexpr auto thm_assume_P = assume<P_x>();
    
    // Verificar que el contexto contiene P(x)
    using Context1 = typename decltype(thm_assume_P)::context_type;
    static_assert(std::is_same_v<Context1, TypeList<P_x>>, "El contexto debería tener P(x)");

    // Paso 2: Descargar hipótesis
    constexpr auto thm_identity = implies_intro<P_x>(thm_assume_P);

    // Verificar resultado: Contexto vacío, Fórmula P->P
    using FinalContext = typename decltype(thm_identity)::context_type;
    using FinalFormula = typename decltype(thm_identity)::formula_type;
    
    static_assert(std::is_same_v<FinalContext, TypeList<>>, "El contexto final debería estar vacío");
    static_assert(check_type<FinalFormula, Implies<P_x, P_x>>, "La fórmula debería ser P -> P");


    // ==========================================
    // TEST 2: Context Management en Modus Ponens
    // ==========================================
    // Premisa 1: A |- P(x)
    // Premisa 2: B |- P(x) -> Q(x)
    // Conclusión: A, B |- Q(x)
    
    // Simulamos teoremas con contextos diferentes (usando assume)
    constexpr auto thm_A = assume<P_x>(); // Context: {P}
    
    using ImpliesPQ = Implies<P_x, Q_x>;
    constexpr auto thm_A_impl_B = assume<ImpliesPQ>(); // Context: {P->Q}

    // Aplicar Modus Ponens
    constexpr auto thm_result = modus_ponens(thm_A, thm_A_impl_B);

    // Verificar fusión de contextos
    using ResultContext = typename decltype(thm_result)::context_type;
    // El orden depende de la implementación de Concat, normalmente {P, P->Q}
    using ExpectedContext = TypeList<P_x, ImpliesPQ>;
    
    static_assert(std::is_same_v<ResultContext, ExpectedContext>, "Modus Ponens debería fusionar contextos");
    static_assert(check_type<typename decltype(thm_result)::formula_type, Q_x>, "MP debería deducir Q(x)");


    // ==========================================
    // TEST 3: Nested Implication (A -> (B -> A))
    // ==========================================
    // 1. Assume A          => {A} |- A
    // 2. Assume B          => {B} |- B
    // 3. Weaken (truco): Usamos A dentro del contexto ampliado.
    //    Nota: En nuestra impl actual, no tenemos regla explícita de 'Weaken' 
    //    pero podemos 'simular' el flujo lógico si tuviéramos una forma de combinar.
    //    
    //    Como no tenemos 'Weaken' explícito aun, lo haremos al revés, descargando 
    //    hipótesis irrelevantes si nuestra lógica de eliminación lo permite, 
    //    o simplemente probamos que implies_intro elimina LA QUE PEDIMOS.

    // Probemos: {A, B} |- A  ==> {A} |- B -> A
    // Forzamos un teorema artificial para testear la metaprogramación de 'RemoveType'
    /* Supongamos que de alguna manera llegamos a Theorem<{A, B}, A>.
       (Esto requeriría regla de Monotonicidad/Debilitamiento: Gamma |- P => Gamma, Delta |- P)
    */

    return 0;
}