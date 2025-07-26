#include "structure.hpp"
#include "executor.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

bool checkParentLoopBck(const GameState& child, int current_index, const std::vector<GameState>& states) {
    if (current_index < 0 || current_index >= static_cast<int>(states.size())) {
        return false;
    }
    if (child.parent == -1) {
        return false;
    }
    if (child.parent < 0 || child.parent >= static_cast<int>(states.size())) {
        return false;
    }

    int ancestor_idx = child.parent;
    std::string child_key = child.to_key();
    while (ancestor_idx != -1) {
        if (states[ancestor_idx].to_key() == child_key) {
            return true;
        }
        ancestor_idx = states[ancestor_idx].parent;
    }
    return false;
}

bool generate_one_child(int current_index, std::vector<GameState>& states, int& action_index, GameState& child, int jar_idx) {
    if (current_index < 0 || current_index >= static_cast<int>(states.size())) {
        return false;
    }

    std::vector<Jar> jars = states[current_index].jars;
    int num_jars = jars.size();
    int action_type = action_index;

    if (action_type > 3 || (action_type == 3 && jar_idx == num_jars - 1)) {
        return false;
    }

    if (jar_idx >= num_jars) {
        return false;
    }

    if (jars[jar_idx].max_capacity <= 0 || jars[jar_idx].current_value < 0 || jars[jar_idx].current_value > jars[jar_idx].max_capacity) {
        return false;
    }

    std::vector<Jar> new_jars = jars;
    bool valid_action = false;
    std::string action_name;

    switch (action_type) {
        case 0: // Empty
            action_name = "Esvaziar";
            if (!jars[jar_idx].is_empty()) {
                new_jars[jar_idx].empty();
                valid_action = true;
            } 
            break;
        case 1: // Fill
            action_name = "Encher";
            if (!jars[jar_idx].is_full()) {
                new_jars[jar_idx].fill();
                valid_action = true;
            } 
            break;
        case 2: // Transfer Left
            action_name = "Transferir Esquerda";
            if (jar_idx > 0 && !jars[jar_idx].is_empty() && !jars[jar_idx - 1].is_full()) {
                int amount = new_jars[jar_idx].current_value;
                int difference = new_jars[jar_idx - 1].transfer(amount);
                new_jars[jar_idx].current_value = difference;
                valid_action = true;
            } 
            break;
        case 3: // Transfer Right
            action_name = "Transferir Direita";
            if (jar_idx < num_jars - 1 && !jars[jar_idx].is_empty() && !jars[jar_idx + 1].is_full()) {
                int amount = new_jars[jar_idx].current_value;
                int difference = new_jars[jar_idx + 1].transfer(amount);
                new_jars[jar_idx].current_value = difference;
                valid_action = true;
            } 
            break;
    }

    if (valid_action) {
        for (size_t i = 0; i < new_jars.size(); ++i) {
            if (new_jars[i].current_value < 0 || new_jars[i].current_value > new_jars[i].max_capacity) {
                return false;
            }
        }
        child = GameState(new_jars, current_index);
        if (checkParentLoopBck(child, current_index, states)) {
            return false;
        }
        return true;
    }

    return false;
}

GameState SearchAlgorithms::solve_with_backtracking(const std::vector<Jar>& initial_jars) {
    if (initial_jars.empty()) {
        return GameState(); // Return empty state for invalid input
    }

    states.emplace_back(initial_jars, -1);
    std::vector<int> action_indices(1, 0);
    std::vector<int> jar_indices(1, 0);
    size_t total_states = 1;

    int current_index = 0;

    while (current_index >= 0) {
        if (states[current_index].is_goal()) {
            std::cout << "Meta atingida! Total de estados explorados: " << total_states << "\n";
            return states[current_index]; // Return the goal state
        }

        if (!states[current_index].closed) {
            GameState child;
            if (generate_one_child(current_index, states, action_indices[current_index], child, jar_indices[current_index])) {
                states.push_back(child);
                action_indices.push_back(0);
                jar_indices.push_back(jar_indices[current_index]);
                action_indices[current_index]++;
                if (action_indices[current_index] > 3) {
                    action_indices[current_index] = 0;
                    jar_indices[current_index]++;
                    if (jar_indices[current_index] >= static_cast<int>(initial_jars.size())) {
                        states[current_index].closed = true;
                    }
                }
                current_index = states.size() - 1;
                total_states++;
                continue;
            } else {
                action_indices[current_index]++;
                if (action_indices[current_index] > 3) {
                    action_indices[current_index] = 0;
                    jar_indices[current_index]++;
                    if (jar_indices[current_index] >= static_cast<int>(initial_jars.size())) {
                        states[current_index].closed = true;
                    }
                }
            }
        }

        if (states[current_index].closed) {
            current_index = states[current_index].parent;
        }

        bool all_closed = true;
        for (const auto& state : states) {
            if (!state.closed) {
                all_closed = false;
                break;
            }
        }
        if (all_closed && current_index < 0) {
            std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << total_states << "\n";
            return GameState(); // Return empty state for no solution
        }
    }

    std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << total_states << "\n";
    return GameState(); // Return empty state for no solution
}