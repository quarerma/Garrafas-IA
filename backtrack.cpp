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

enum ActionType { FILL, EMPTY, TRANSFER };

struct Action {
    ActionType type;
    int jar_index;
    int target_index;
    Action(ActionType t, int j, int tj = -1) : type(t), jar_index(j), target_index(tj) {}
};

bool generate_one_child(int current_index, std::vector<GameState>& game_states, int& action_index, GameState& child) {
    if (current_index < 0 || current_index >= static_cast<int>(game_states.size())) {
        std::cerr << "Error: Invalid current_index: " << current_index << std::endl;
        return false;
    }

    std::vector<Jar> jars = game_states[current_index].jars;
    int num_jars = jars.size();
    int actions_per_jar = 2 + 2 * (num_jars - 1);
    int total_actions = num_jars * actions_per_jar;

    if (action_index >= total_actions) return false;

    int jar_idx = action_index / actions_per_jar;
    int action_offset = action_index % actions_per_jar;

    if (jars[jar_idx].max_capacity <= 0 || jars[jar_idx].current_value < 0 || jars[jar_idx].current_value > jars[jar_idx].max_capacity) {
        std::cerr << "Error: Invalid jar at index " << jar_idx << std::endl;
        return false;
    }

    std::vector<Jar> new_jars = jars;
    bool valid_action = false;

    if (action_offset == 0 && !jars[jar_idx].is_full()) {
        new_jars[jar_idx].fill();
        valid_action = true;
    } else if (action_offset == 1 && !jars[jar_idx].is_empty()) {
        new_jars[jar_idx].empty();
        valid_action = true;
    } else if (action_offset >= 2) {
        int transfer_idx = (action_offset - 2) / 2;
        int dir = (transfer_idx == 0) ? -1 : 1;
        int target_idx = jar_idx + dir;
        bool is_left = (action_offset % 2 == 0);

        if (is_left && dir == -1 && target_idx >= 0 && !jars[jar_idx].is_empty() && !jars[target_idx].is_full()) {
            int amount = new_jars[jar_idx].current_value;
            int sobra = new_jars[target_idx].transfer(amount);
            new_jars[jar_idx].current_value = sobra;
            valid_action = true;
        } else if (!is_left && dir == 1 && target_idx < num_jars && !jars[jar_idx].is_empty() && !jars[target_idx].is_full()) {
            int amount = new_jars[jar_idx].current_value;
            int sobra = new_jars[target_idx].transfer(amount);
            new_jars[jar_idx].current_value = sobra;
            valid_action = true;
        }
    }

    if (valid_action) {
        for (size_t i = 0; i < new_jars.size(); ++i) {
            if (new_jars[i].current_value < 0 || new_jars[i].current_value > new_jars[i].max_capacity) {
                std::cerr << "Error: Invalid state after action " << action_index << " for jar " << i << std::endl;
                return false;
            }
        }
        child = GameState(new_jars, current_index);
        return is_unique_state(child, game_states);
    }

    return false;
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
    game_states.emplace_back(initial_jars, -1);
    std::vector<int> action_indices(game_states.size(), 0);

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
            GameState child; // Declare here, will be initialized by generate_one_child
            if (generate_one_child(current_index, game_states, action_indices[current_index], child)) {
                game_states.push_back(child);
                action_indices.push_back(0);
                current_index = game_states.size() - 1;
                continue;
            } else {
                action_indices[current_index]++;
                if (action_indices[current_index] >= initial_jars.size() * (2 + 2 * (initial_jars.size() - 1))) {
                    game_states[current_index].closed = true;
                }
            }
        }

        if (game_states[current_index].closed) {
            current_index = game_states[current_index].parent;
        }
    }

    std::cout << "Nenhuma solução encontrada.\n";
}