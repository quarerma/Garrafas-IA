#ifndef SEARCH_ALGORITHMS_HPP
#define SEARCH_ALGORITHMS_HPP

#include "structure.hpp"
#include <vector>

class SearchAlgorithms {
public:
    std::vector<GameState> states; // Array of GameState initialized in each search

    void busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite);
    void busca_largura(const std::vector<Jar> &initial_jars);
    void busca_ordenada(const std::vector<Jar> &initial_jars);
    void busca_gulosa(const std::vector<Jar> &initial_jars);
    void solve_with_backtracking(const std::vector<Jar> &initial_jars);
    void solve_with_astar(const std::vector<Jar> &initial_jars);
    void solve_with_ida_star(const std::vector<Jar> &initial_jars);
};

#endif // SEARCH_ALGORITHMS_HPP