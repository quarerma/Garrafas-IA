#ifndef SEARCH_ALGORITHMS_HPP
#define SEARCH_ALGORITHMS_HPP

#include "structure.hpp"
#include <vector>
#include <iostream>
#include <unordered_set>

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

    void print() const {

        int visited_count = 0;
        int closed_count = 0;
        std::unordered_set<int> unique_parents;

        for (size_t i = 0; i < states.size(); ++i) {
            const GameState& s = states[i];

            if (s.visited) visited_count++;
            if (s.closed) closed_count++;
            if (s.parent != -1) unique_parents.insert(s.parent); // -1 = root node, not counted
        }

        std::cout << "-- Summary ---\n";
        std::cout << "Visited states: " << visited_count - 1 << "\n";
        std::cout << "Closed states: " << closed_count - 1<< "\n";
        std::cout << "Expanded (unique parent IDs): " << unique_parents.size() << "\n";
        std::cout << "Total states: " << states.size() << "\n";
    }

};

#endif // SEARCH_ALGORITHMS_HPP