#include "structure.hpp"
#include <queue>
#include <unordered_map>
#include <set>

class CompareGameState {
public:
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        return a.second > b.second; // Compare based on f_cost (g_cost + h)
    }
};

bool generate_child(GameState& current, std::vector<GameState>& game_states, int jar_idx, int action_type, GameState& child) {
    if (jar_idx >= static_cast<int>(current.jars.size())) {
        return false;
    }

    std::vector<Jar> new_jars = current.jars;
    bool valid_action = false;

    switch (action_type) {
        case 0: // Empty
            if (!new_jars[jar_idx].is_empty()) {
                new_jars[jar_idx].empty();
                valid_action = true;
            }
            break;
        case 1: // Fill
            if (!new_jars[jar_idx].is_full()) {
                new_jars[jar_idx].fill();
                valid_action = true;
            }
            break;
        case 2: // Transfer Left
            if (jar_idx > 0 && !new_jars[jar_idx].is_empty() && !new_jars[jar_idx - 1].is_full()) {
                current.transfer_from_jars(new_jars[jar_idx], new_jars[jar_idx - 1]);
                valid_action = true;
            }
            break;
        case 3: // Transfer Right
            if (jar_idx < static_cast<int>(new_jars.size()) - 1 && !new_jars[jar_idx].is_empty() && !new_jars[jar_idx + 1].is_full()) {
                current.transfer_from_jars(new_jars[jar_idx], new_jars[jar_idx + 1]);
                valid_action = true;
            }
            break;
        default:
            return false;
    }

    if (valid_action) {
        child = GameState(new_jars, current.index);
        child.g_cost = current.g_cost + current.calculate_action_cost(jar_idx, action_type);
        return true;
    }
    return false;
}

void solve_with_astar(const std::vector<Jar>& initial_jars) {
    if (initial_jars.empty()) {
        std::cout << "Error: Empty jar list\n";
        return;
    }

    std::cout << "Solving with A*...\n";
    std::cout << "Initial state:\n";
    for (const Jar& jar : initial_jars) {
        std::cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << std::endl;
    }

    std::vector<GameState> game_states;
    game_states.emplace_back(initial_jars, -1);
    game_states[0].index = 0;
    game_states[0].g_cost = 0;

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareGameState> open_list;
    std::unordered_map<std::string, int> visited;
    std::set<std::string> closed_set;

    open_list.push({0, game_states[0].g_cost + game_states[0].heuristic()});
    visited[game_states[0].to_key()] = 0;

    int total_states = 1;

    while (!open_list.empty()) {
        int current_idx = open_list.top().first;
        open_list.pop();

        GameState& current = game_states[current_idx];
        std::string current_key = current.to_key();

        if (closed_set.find(current_key) != closed_set.end()) {
            continue;
        }

        closed_set.insert(current_key);
        current.closed = true;

        std::cout << "Processing state " << current_idx << ": ";
        current.print();
        std::cout << "g_cost=" << current.g_cost << ", h=" << current.heuristic() << ", f=" << current.g_cost + current.heuristic() << "\n";

        if (current.is_goal()) {
            std::cout << "Goal reached! Total states explored: " << total_states << "\n";
            current.print_path(current, current_idx, game_states);
            return;
        }

        // Generate all possible children
        for (int jar_idx = 0; jar_idx < static_cast<int>(current.jars.size()); ++jar_idx) {
            for (int action_type = 0; action_type < 4; ++action_type) {
                if (action_type == 2 && jar_idx == 0) continue; // No transfer left from first jar
                if (action_type == 3 && jar_idx == static_cast<int>(current.jars.size()) - 1) continue; // No transfer right from last jar

                GameState child;
                if (generate_child(current, game_states, jar_idx, action_type, child)) {
                    std::string child_key = child.to_key();
                    if (closed_set.find(child_key) != closed_set.end()) {
                        continue;
                    }

                    int tentative_g_cost = current.g_cost + current.calculate_action_cost(jar_idx, action_type);
                    auto it = visited.find(child_key);

                    if (it == visited.end() || tentative_g_cost < game_states[it->second].g_cost) {
                        child.index = game_states.size();
                        game_states.push_back(child);
                        visited[child_key] = child.index;
                        open_list.push({child.index, tentative_g_cost + child.heuristic()});
                        total_states++;
                        std::cout << "Added child state " << child.index << ": ";
                        child.print();
                        std::cout << "g_cost=" << child.g_cost << ", h=" << child.heuristic() << ", f=" << child.g_cost + child.heuristic() << "\n";
                    }
                }
            }
        }
    }

    std::cout << "No solution found. Total states explored: " << total_states << "\n";
}