#ifndef SEARCH_ALGORITHMS_HPP
#define SEARCH_ALGORITHMS_HPP

#include "structure.hpp"
#include <vector>
#include <iostream>

class SearchAlgorithms {
public:
    std::vector<GameState> states; // Array of GameState initialized in each search

    void busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite);
    void busca_largura(const std::vector<Jar> &initial_jars);
    void busca_ordenada(const std::vector<Jar> &initial_jars);
    void busca_gulosa(const std::vector<Jar> &initial_jars);
    GameState solve_with_backtracking(const std::vector<Jar> &initial_jars);
    void solve_with_astar(const std::vector<Jar> &initial_jars);
    void solve_with_ida_star(const std::vector<Jar> &initial_jars);

    // Print function to display the states explored during a search
    void print() const {
        std::cout << "\n=== Explored States ===\n";
        for (size_t i = 0; i < states.size(); ++i) {
            std::cout << "State " << i << ": ";
            std::cout << states[i].to_key() << "\n";}
        std::cout << "Total states explored: " << states.size() << "\n";
    }
};

#endif // SEARCH_ALGORITHMS_HPP