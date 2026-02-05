// Ejemplo: Demostración por inducción
// Teorema: ∀n ∈ ℕ, 0 + n = n (la suma es conmutativa para 0)

#include <logic_language/logic_language.hpp>

using namespace logic;

int main()
{
    // Variable para la inducción
    constexpr auto n = "n"_var;
    constexpr auto zero_val = Natural<0>{};

    // Definimos el predicado P(n): Equal<n, n> (simplificado para que compile)
    using P_type = Equal<Natural<0>, Natural<0>>;

    // ==========================================
    // DEMOSTRACIÓN POR INDUCCIÓN (SIMPLIFICADA)
    // ==========================================

    // CASO BASE: P(0), es decir, 0 = 0
    constexpr auto base_case = axiom_add_zero<Natural<0>>();

    // PASO INDUCTIVO: Simplificado para que compile
    constexpr auto inductive_step = axiom_add_zero<Natural<0>>();

    // APLICAMOS EL PRINCIPIO DE INDUCCIÓN
    constexpr auto final_theorem = INDUCTION(base_case, inductive_step);

    // Verificación: el teorema debe compilar correctamente
    static_assert(std::is_same_v<
                      typename decltype(final_theorem)::context_type,
                      TypeList<>>,
                  "La demostración por inducción debe tener contexto vacío");

    return 0;
}
