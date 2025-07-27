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
    int goal_index = -1;
    int depth = 0;
    int goal_g_cost = 0;

    // Find the first goal state
    for (size_t i = 0; i < states.size(); ++i) {
        const GameState& s = states[i];
        if (s.is_goal() && goal_index == -1) { // Only set if no goal was found yet
            std::cout << "Goal state found at index: " << i << "   Key:" << s.to_key() << "\n";
            goal_index = i;
            goal_g_cost = s.g_cost;
        }
        
        if (s.visited) visited_count++;
        if (s.closed) closed_count++;
        if (s.parent != -1) unique_parents.insert(s.parent); // -1 = root node, not counted
    }

    // If no goal state found, use the last state
    if (goal_index == -1 && !states.empty()) {
        goal_index = states.size() - 1;
        goal_g_cost = states[goal_index].g_cost;
    }

    // Calculate depth by tracing parents to root
    if (goal_index != -1) {
        int current_index = goal_index;
        while (current_index != -1 && current_index < static_cast<int>(states.size())) {
            depth++;
            current_index = states[current_index].parent;
        }
        // Adjust depth if root was reached (root has parent -1, so depth includes root)
        if (current_index == -1) {
            depth--; // Subtract 1 to exclude the root node from depth count
        }
    }

    std::cout << "-- Summary ---\n";
    std::cout << "Visited states: " << visited_count - 1 << "\n";
    std::cout << "Closed states: " << closed_count - 1 << "\n";
    std::cout << "Expanded (unique parent IDs): " << unique_parents.size() << "\n";
    std::cout << "Total states: " << states.size() << "\n";
    std::cout << "Depth of goal (or last state): " << depth << "\n";
    std::cout << "Path cost (g_cost): " << goal_g_cost << "\n";
}

};

#endif // SEARCH_ALGORITHMS_HPP