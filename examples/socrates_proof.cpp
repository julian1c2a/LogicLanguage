// Ejemplo: Demostración del Silogismo de Sócrates
// "Todos los humanos son mortales. Sócrates es humano. Por tanto, Sócrates es mortal."

#include <logic_language/logic_language.hpp>

using namespace logic;

int main() {
    // Variables
    constexpr auto socrates = "socrates"_var;
    constexpr auto x = "x"_var;
    
    // Predicados
    using Human_x = decltype(Human(x));
    using Mortal_x = decltype(Mortal(x));
    using Human_socrates = decltype(Human(socrates));
    using Mortal_socrates = decltype(Mortal(socrates));
    
    // ==========================================
    // DEMOSTRACIÓN CLÁSICA CON SINTAXIS MEJORADA
    // ==========================================
    
    // Premisa 1: ∀x.(Human(x) → Mortal(x))
    using ForallHumanMortal = Forall<std::remove_cv_t<decltype(x)>, Implies<Human_x, Mortal_x>>;
    constexpr auto premise1 = ASSUME(ForallHumanMortal{});
    
    // Premisa 2: Human(socrates)
    constexpr auto premise2 = ASSUME(Human_socrates{});
    
    // Paso 1: Instanciar el universal con 'socrates'
    // De ∀x.(Human(x) → Mortal(x)) obtenemos Human(socrates) → Mortal(socrates)
    constexpr auto step1 = FORALL_ELIM(premise1, socrates);
    
    // Paso 2: Aplicar Modus Ponens
    // De Human(socrates) y Human(socrates) → Mortal(socrates) obtenemos Mortal(socrates)
    constexpr auto conclusion = APPLY_MP(premise2, step1);
    
    // Descargar las premisas para obtener el teorema completo
    constexpr auto discharged1 = DISCHARGE(Human_socrates{}, conclusion);
    constexpr auto proof_socrates_mortal = DISCHARGE(ForallHumanMortal{}, discharged1);
    
    // Verificación estática: el teorema debe tener la forma correcta
    using ForallHumanMortal = Forall<std::remove_cv_t<decltype(x)>, Implies<Human_x, Mortal_x>>;
    using Expected = If_Then<
        ForallHumanMortal,
        If_Then<Human_socrates, Mortal_socrates>
    >;
    
    static_assert(std::is_same_v<
        typename decltype(proof_socrates_mortal)::formula_type,
        Expected
    >, "La demostración de Sócrates debe tener la forma correcta");
    
    return 0;
}
