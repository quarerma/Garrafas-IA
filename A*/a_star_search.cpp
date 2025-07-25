#include "structure.hpp"
#include <queue>
#include <unordered_map>

// Comparator for priority_queue to prioritize states with lowest f_cost
class CompareGameState {
public:
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        return a.second > b.second; // Min-heap: lower f_cost has higher priority
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
    game_states[0].f_cost = game_states[0].heuristic();
    game_states[0].visited = false; // Initial state not visited yet

    // Priority Queue Explanation:
    // - This is a min-heap priority queue (customized via CompareGameState) that stores pairs of (state_index, f_cost).
    // - It ensures the state with the lowest f_cost is always at the top (popped first).
    // - Usage: push({index, f_cost}) to add states; top() to peek the lowest f_cost state; pop() to remove it.
    // - Handles stale entries by checking if popped f_cost matches current state's f_cost.
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareGameState> open_list;

    // Visited map for quick duplicate detection: maps state key to index in game_states
    std::unordered_map<std::string, int> visited_map;

    open_list.push({0, game_states[0].f_cost});
    visited_map[game_states[0].to_key()] = 0;

    int total_states = 1;

    while (!open_list.empty()) {
        auto top = open_list.top();
        int current_idx = top.first;
        int popped_f = top.second;
        open_list.pop();

        GameState& current = game_states[current_idx];
        if (popped_f > current.f_cost || current.visited) {
            continue; // Stale entry or already visited
        }

        current.visited = true;

        std::cout << "Processing state " << current_idx << ": ";
        current.print();
        std::cout << "g_cost=" << current.g_cost << ", h=" << current.heuristic() << ", f=" << current.f_cost << "\n";

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

                    int action_cost = current.calculate_action_cost(jar_idx, action_type);
                    int tentative_g = current.g_cost + action_cost;
                    int child_h = child.heuristic();
                    int child_f = tentative_g + child_h;

                    auto it = visited_map.find(child_key);
                    if (it != visited_map.end()) {
                        int existing_idx = it->second;
                        if (game_states[existing_idx].visited || tentative_g >= game_states[existing_idx].g_cost) {
                            continue; // Already visited or not better
                        }
                        // Better path found: update existing state
                        game_states[existing_idx].g_cost = tentative_g;
                        game_states[existing_idx].f_cost = child_f;
                        game_states[existing_idx].parent = current.index;
                        open_list.push({existing_idx, child_f});
                        std::cout << "Updated state " << existing_idx << ": ";
                        game_states[existing_idx].print();
                        std::cout << "g_cost=" << tentative_g << ", h=" << child_h << ", f=" << child_f << "\n";
                    } else {
                        // New state
                        child.g_cost = tentative_g;
                        child.f_cost = child_f;
                        child.index = game_states.size();
                        child.visited = false;
                        game_states.push_back(child);
                        visited_map[child_key] = child.index;
                        open_list.push({child.index, child_f});
                        total_states++;
                        std::cout << "Added child state " << child.index << ": ";
                        child.print();
                        std::cout << "g_cost=" << tentative_g << ", h=" << child_h << ", f=" << child_f << "\n";
                    }
                }
            }
        }

        current.closed = true; // Mark current state as closed
    }

    std::cout << "No solution found. Total states explored: " << total_states << "\n";
}