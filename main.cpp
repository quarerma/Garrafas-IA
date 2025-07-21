#include <iostream>
#include <vector>
#include "structure.hpp"
#include "backtrack.hpp"
#include "profundidade_largura.hpp"
#include "ordenada_gulosa.hpp"

int main()
{
    std::vector<Jar> jars = {
        Jar(0, 9, 4),
        Jar(1, 5, 1),
        Jar(2, 3, 3)};

    cout << "Estado inicial:\n";
    for (const Jar &jar : jars)
    {
        cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << endl;
    }

    // ProfundidadeLargura buscas;
    // buscas.busca_profundidade(jars, 5);
    // buscas.busca_largura(jars);

    OrdenadaGulosa buscas;
    buscas.busca_ordenada(jars);

    return 0;
}
