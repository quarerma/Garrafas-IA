#include "backtrack.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

bool is_goal_state(const GameState& state) {
    if (state.jars.empty()) return false;
    const Jar& last = state.jars.back();
    return last.current_value == (last.max_capacity / 3);
}

bool is_unique_state(const GameState& new_state, const std::vector<GameState>& game_states) {
    std::string key = new_state.to_key();
    for (const GameState& state : game_states) {
        if (state.to_key() == key) return false;
    }
    return true;
}

void generate_children(int current_index, std::vector<GameState>& game_states) {
    if (current_index < 0 || current_index >= static_cast<int>(game_states.size())) {
        std::cerr << "Error: Invalid current_index: " << current_index << std::endl;
        return;
    }

    // Copy jars to avoid reference invalidation
    std::vector<Jar> jars = game_states[current_index].jars;

    std::cout << "Generating children for state " << current_index << ":\n";
    for (size_t i = 0; i < jars.size(); ++i) {
        std::cout << "Jar " << i << ": id=" << jars[i].id << ", value=" << jars[i].current_value << ", capacity=" << jars[i].max_capacity << std::endl;
        if (jars[i].max_capacity <= 0 || jars[i].current_value < 0 || jars[i].current_value > jars[i].max_capacity) {
            std::cerr << "Error: Invalid jar at index " << i << std::endl;
            continue;
        }

        // Fill
        if (!jars[i].is_full()) {
            std::vector<Jar> new_jars = jars;
            new_jars[i].fill();
            GameState child(new_jars, current_index);
            if (is_unique_state(child, game_states)) {
                game_states.push_back(child);
            }
        }

        // Empty
        std::cout << "Checking is_empty for jar " << i << std::endl;
        if (!jars[i].is_empty()) {
            std::vector<Jar> new_jars = jars;
            new_jars[i].empty();
            GameState child(new_jars, current_index);
            if (is_unique_state(child, game_states)) {
                game_states.push_back(child);
            }
        }

        // Transfer
        for (int dir : {-1, 1}) {
            int j = i + dir;
            if (j >= 0 && j < static_cast<int>(jars.size())) {
                if (!jars[i].is_empty() && !jars[j].is_full()) {
                    std::vector<Jar> new_jars = jars;
                    int amount = new_jars[i].current_value;
                    int sobra = new_jars[j].transfer(amount);
                    new_jars[i].current_value = sobra;
                    if (new_jars[i].current_value < 0 || new_jars[j].current_value < 0 ||
                        new_jars[i].current_value > new_jars[i].max_capacity ||
                        new_jars[j].current_value > new_jars[j].max_capacity) {
                        std::cerr << "Error: Invalid transfer from jar " << i << " to " << j << std::endl;
                        continue;
                    }
                    GameState child(new_jars, current_index);
                    if (is_unique_state(child, game_states)) {
                        game_states.push_back(child);
                    }
                }
            }
        }
    }

    game_states[current_index].closed = true;
}

void Backtrack::solve_with_backtracking(const std::vector<Jar>& initial_jars) {
    std::cout << "Resolvendo com backtracking...\n";
    if (initial_jars.empty()) {
        std::cout << "Error: Empty jar list\n";
        return;
    }

    std::cout << "Estado inicial Game States:\n";
    for (const Jar& jar : initial_jars) {
        std::cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << std::endl;
    }

    std::vector<GameState> game_states;
    game_states.emplace_back(initial_jars, -1); // root

    int current_index = 0;

    while (current_index >= 0) {
        if (is_goal_state(game_states[current_index])) {
            std::cout << "Meta atingida!\n";
            std::vector<int> path;
            int idx = current_index;
            while (idx != -1) {
                path.push_back(idx);
                idx = game_states[idx].parent;
            }
            std::reverse(path.begin(), path.end());
            for (int i : path) {
                std::cout << "Estado " << i << ": ";
                for (int val : game_states[i].values) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }
            return;
        }

        if (!game_states[current_index].closed) {
            generate_children(current_index, game_states);
        }

        int next_index = -1;
        for (int i = current_index + 1; i < static_cast<int>(game_states.size()); ++i) {
            if (game_states[i].parent == current_index && !game_states[i].closed) {
                next_index = i;
                break;
            }
        }

        if (next_index != -1) {
            current_index = next_index;
        } else {
            current_index = game_states[current_index].parent;
        }
    }

    std::cout << "Nenhuma solução encontrada.\n";
}