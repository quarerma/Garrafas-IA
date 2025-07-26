#include "structure.hpp"
#include "executor.hpp"
#include <unordered_map>
#include <set>
#include <limits>
#include <algorithm>
#include <vector>

// Checks if the child state creates a cycle with its ancestors
bool checkParentLoopIDA(const GameState& child, int current_index, const std::vector<GameState>& states) {
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

// Generates one child state for the given jar_idx and action_index
bool generate_one_child(int current_index, std::vector<GameState>& states, int action_index, GameState& child, int jar_idx) {

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
        child.g_cost = states[current_index].g_cost + states[current_index].calculate_action_cost(jar_idx, action_type);
        child.f_cost = child.g_cost + child.heuristic();
        child.closed = false;
        child.visited = false;

        // Sync values from updated jars
        for (int i = 0; i < child.num_jars; ++i) {
            child.values[i] = child.jars[i].current_value;
        }

        if (checkParentLoopIDA(child, current_index, states)) {
            return false;
        }
        return true;
    }

    return false;
}

void SearchAlgorithms::solve_with_ida_star(const std::vector<Jar>& initial_jars) {
    if (initial_jars.empty()) {
        return;
    }

    int threshold = 0;
    int patamar_old = -1;
    std::unordered_map<std::string, int> min_g_cost;

    while (true) {
        if (patamar_old == threshold) {
            std::cout << "No progress made, stopping search.\n";
            return;
        }

        std::cout << "\n🔄 Starting threshold iteration: " << threshold << "\n";

        // Clear and start fresh for this threshold
        states.clear();
        // update min_g_cost
        min_g_cost.clear();

        states.emplace_back(initial_jars, -1);
        states[0].index = 0;
        states[0].g_cost = 0;
        states[0].f_cost = states[0].heuristic();
        states[0].closed = false;
        states[0].visited = false;

        std::unordered_map<std::string, int> iteration_visited_map;
        iteration_visited_map[states[0].to_key()] = 0;
        min_g_cost[states[0].to_key()] = 0;

        std::unordered_map<int, std::pair<int, int>> expansion_status;
        expansion_status[0] = {0, 0};

        int current_index = 0;
        std::set<int> discarded;
        // clear discarded set for this iteration
        discarded.clear();

        while (current_index >= 0) {
            GameState& current = states[current_index];
            int current_f = current.f_cost;

            // Prune if over threshold
            if (current_f > threshold) {
                discarded.insert(current_f);
                current.visited = true;
                current_index = current.parent;
                continue;
            }

            // Check goal
            if (current.is_goal() && current_f <= threshold) {
                std::cout << "✅ Solution found with cost: " << current.g_cost << "\n";
                std::cout << "📦 Total nodes generated: " << states.size() << "\n";
                return;
            }

            if (!current.closed) {
                auto& exp = expansion_status[current_index];
                int jar_idx = exp.first;
                int action_idx = exp.second;
                GameState child;

                if (generate_one_child(current_index, states, action_idx, child, jar_idx)) {
                    std::string child_key = child.to_key();

                    auto global_it = min_g_cost.find(child_key);
                    if (global_it != min_g_cost.end() && child.g_cost >= global_it->second) {
                       
                        exp.second++;
                        if (exp.second > 3) {
                            exp.second = 0;
                            exp.first++;
                            if (exp.first >= static_cast<int>(current.jars.size())) {
                                current.closed = true;
                            }
                        }
                        continue;
                    }

                    min_g_cost[child_key] = child.g_cost;

                    auto it = iteration_visited_map.find(child_key);
                    if (it != iteration_visited_map.end()) {
                        int existing_idx = it->second;
                        if (child.g_cost < states[existing_idx].g_cost) {
                            child.index = existing_idx;
                            states[existing_idx] = child;
                            expansion_status[existing_idx] = {0, 0};
                        } else {
                            exp.second++;
                            if (exp.second > 3) {
                                exp.second = 0;
                                exp.first++;
                                if (exp.first >= static_cast<int>(current.jars.size())) {
                                    current.closed = true;
                                }
                            }
                            continue;
                        }
                    } else {
                        child.index = states.size();
                        states.push_back(child);
                        iteration_visited_map[child_key] = child.index;
                        expansion_status[child.index] = {0, 0};
                    }

                    // Prepare next child
                    exp.second++;
                    if (exp.second > 3) {
                        exp.second = 0;
                        exp.first++;
                        if (exp.first >= static_cast<int>(current.jars.size())) {
                            current.closed = true;
                        }
                    }

                    current_index = child.index;
                    continue;
                } else {
                    exp.second++;
                    if (exp.second > 3) {
                        exp.second = 0;
                        exp.first++;
                        if (exp.first >= static_cast<int>(current.jars.size())) {
                            current.closed = true;
                        }
                    }
                }
            }

            // Backtrack if no more options
            if (current.closed) {
                current_index = current.parent;
            }
        }



        if (discarded.empty()) {
            return;
        }

        patamar_old = threshold;
        threshold = *discarded.begin();
    }
}
