#include <iostream>
#include <vector>
#include <ctime>
#include "search_algorithms.hpp"

int main() {
    // Sample datasets
    std::vector<std::vector<Jar>> samples = {
        // Sample 1: 3 jars
        {Jar(0, 3, 0), Jar(1, 5, 0), Jar(2, 2, 0)},
        // Sample 2: 5 jars
        {Jar(0, 3, 0), Jar(1, 5, 0), Jar(2, 2, 0), Jar(3, 4, 0), Jar(4, 6, 0)},
        // Sample 3: 10 jars
        {Jar(0, 3, 0), Jar(1, 5, 0), Jar(2, 2, 0), Jar(3, 4, 0), Jar(4, 6, 0),
         Jar(5, 7, 0), Jar(6, 8, 0), Jar(7, 9, 0), Jar(8, 10, 0), Jar(9, 12, 0)}
    };

    SearchAlgorithms search;

    for (int i = 0; i < samples.size(); ++i) {
        std::cout << "\n=== Sample " << (i + 1) << " with " << samples[i].size() << " jars ===\n";
        std::cout << "Initial state:\n";
        for (const Jar& jar : samples[i]) {
            std::cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << std::endl;
        }

        // Set a target_Q (e.g., half the max capacity of the first jar for simplicity)
        search.states[0].target_Q = samples[i][0].max_capacity / 2; // Adjust as needed

        // Timing each algorithm
        std::clock_t start, end;
        double time_taken;

        start = std::clock();
        std::cout << "\nRunning busca_profundidade (depth limit 10):\n";
        search.busca_profundidade(samples[i], 10);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000; // Time in milliseconds
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning busca_largura:\n";
        search.busca_largura(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning busca_ordenada:\n";
        search.busca_ordenada(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning busca_gulosa:\n";
        search.busca_gulosa(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning solve_with_backtracking:\n";
        search.solve_with_backtracking(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning solve_with_astar:\n";
        search.solve_with_astar(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";

        start = std::clock();
        std::cout << "\nRunning solve_with_ida_star:\n";
        search.solve_with_ida_star(samples[i]);
        end = std::clock();
        time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000;
        std::cout << "Time taken: " << time_taken << " ms\n";
    }

    return 0;
}