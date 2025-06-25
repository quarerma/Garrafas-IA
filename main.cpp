#include <iostream>
#include <vector>
#include "structure.hpp"
#include "backtrack.hpp"

int main() {
    std::vector<Jar> jars = {
        Jar(0, 8),
        Jar(1, 5),
        Jar(2, 3)
    };

    jars[0].fill(); // opcional: começa cheio

    cout << "Estado inicial:\n";
    for (const Jar& jar : jars) {
        cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << endl;
    }
    Backtrack backtrack;
    backtrack.solve_with_backtracking(jars);

    return 0;
}
