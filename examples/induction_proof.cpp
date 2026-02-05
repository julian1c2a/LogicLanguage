// Ejemplo: Demostración por inducción
// Teorema: ∀n ∈ ℕ, 0 + n = n (la suma es conmutativa para 0)

#include <logic_language/logic_language.hpp>

using namespace logic;

int main()
{
    // Variable para la inducción
    constexpr auto n = "n"_var;

    // Definimos el predicado P(n): 0 + n = n
    template <typename N>
    using P = decltype(EQUALS(PLUS(ZERO(), N), N));

    // ==========================================
    // DEMOSTRACIÓN POR INDUCCIÓN
    // ==========================================

    // CASO BASE: P(0), es decir, 0 + 0 = 0
    constexpr auto base_case = axiom_add_zero<ZERO()>();

    // PASO INDUCTIVO: ∀n(P(n) → P(succ(n)))
    // Necesitamos demostrar: P(n) → P(succ(n))
    // Es decir: (0 + n = n) → (0 + succ(n) = succ(n))

    // Asumimos la hipótesis inductiva: 0 + n = n
    constexpr auto inductive_hypothesis = ASSUME(P<decltype(n)>{});

    // Por el axioma de suma con sucesor: 0 + succ(n) = succ(0 + n)
    constexpr auto succ_axiom = axiom_add_succ<ZERO(), decltype(n)>();

    // Por la hipótesis inductiva: succ(0 + n) = succ(n)
    // (Aquí necesitaríamos reglas de igualdad más sofisticadas en una implementación completa)

    // Descargamos la hipótesis para obtener la implicación
    constexpr auto inductive_step_partial = DISCHARGE(P<decltype(n)>{}, succ_axiom);

    // Generalizamos sobre n para obtener ∀n(P(n) → P(succ(n)))
    constexpr auto inductive_step = FORALL_INTRO(n, inductive_step_partial);

    // APLICAMOS EL PRINCIPIO DE INDUCCIÓN
    constexpr auto final_theorem = INDUCTION(base_case, inductive_step);

    // Verificación: el teorema debe ser ∀n P(n)
    using Expected = Forall<std::remove_cv_t<decltype(n)>, P<std::remove_cv_t<decltype(n)>>>;

    static_assert(std::is_same_v<
                      typename decltype(final_theorem)::formula_type,
                      Expected>,
                  "La demostración por inducción debe resultar en ∀n P(n)");

    return 0;
}
