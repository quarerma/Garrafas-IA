#include "structure.cpp"
#include <iostream>
#include <vector>



void backtrack(std::vector<Jar>& jars, int index, int target) {
    if (target == 0) {
        std::cout << "Solution found: ";
        for (const auto& jar : jars) {
            std::cout << jar.current_value << " ";
        }
        std::cout << std::endl;
        return;
    }
    
    if (index >= jars.size() || target < 0) {
        return;
    }

    // Try to fill the current jar
    if (!jars[index].is_full()) {
        jars[index].fill();
        backtrack(jars, index + 1, target - jars[index].get_capacity());
        jars[index].current_value = 0; // Backtrack
    }

    // Skip the current jar
    backtrack(jars, index + 1, target);
}