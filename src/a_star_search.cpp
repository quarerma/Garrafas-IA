#include "structure.hpp"
#include "executor.hpp"
#include <queue>
#include <unordered_map>

// Comparator for priority_queue to prioritize states with lowest f_cost
class CompareGameState {
public:
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        return a.second > b.second; // Min-heap: lower f_cost has higher priority
    }
};

bool generate_child(GameState& current, std::vector<GameState>& states, int jar_idx, int action_type, GameState& child) {
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

void SearchAlgorithms::solve_with_astar(const std::vector<Jar>& initial_jars) {
    if (initial_jars.empty()) {
        return;
    }

 

    states.emplace_back(initial_jars, -1);
    states[0].index = 0;
    states[0].g_cost = 0;
    states[0].f_cost = states[0].heuristic();
    states[0].visited = false; // Initial state not visited yet

    // Priority Queue Explanation:
    // - This is a min-heap priority queue (customized via CompareGameState) that stores pairs of (state_index, f_cost).
    // - It ensures the state with the lowest f_cost is always at the top (popped first).
    // - Usage: push({index, f_cost}) to add states; top() to peek the lowest f_cost state; pop() to remove it.
    // - Handles stale entries by checking if popped f_cost matches current state's f_cost.
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareGameState> open_list;

    // Visited map for quick duplicate detection: maps state key to index in states
    std::unordered_map<std::string, int> visited_map;

    open_list.push({0, states[0].f_cost});
    visited_map[states[0].to_key()] = 0;

    int total_states = 1;

    while (!open_list.empty()) {
        auto top = open_list.top();
        int current_idx = top.first;
        int popped_f = top.second;
        open_list.pop();

        GameState& current = states[current_idx];
        if (popped_f > current.f_cost || current.visited) {
            continue; // Stale entry or already visited
        }

        current.visited = true;

        if (current.is_goal()) {
            return;
        }

        // Generate all possible children
        for (int jar_idx = 0; jar_idx < static_cast<int>(current.jars.size()); ++jar_idx) {
            for (int action_type = 0; action_type < 4; ++action_type) {
                if (action_type == 2 && jar_idx == 0) continue; // No transfer left from first jar
                if (action_type == 3 && jar_idx == static_cast<int>(current.jars.size()) - 1) continue; // No transfer right from last jar

                GameState child;
                if (generate_child(current, states, jar_idx, action_type, child)) {
                    std::string child_key = child.to_key();

                    int action_cost = current.calculate_action_cost(jar_idx, action_type);
                    int tentative_g = current.g_cost + action_cost;
                    int child_h = child.heuristic();
                    int child_f = tentative_g + child_h;

                    auto it = visited_map.find(child_key);
                    if (it != visited_map.end()) {
                        int existing_idx = it->second;
                        if (states[existing_idx].visited || tentative_g >= states[existing_idx].g_cost) {
                            continue; // Already visited or not better
                        }
                        // Better path found: update existing state
                        states[existing_idx].g_cost = tentative_g;
                        states[existing_idx].f_cost = child_f;
                        states[existing_idx].parent = current.index;
                        open_list.push({existing_idx, child_f});
                     
                    } else {
                        // New state
                        child.g_cost = tentative_g;
                        child.f_cost = child_f;
                        child.index = states.size();
                        child.visited = false;
                        states.push_back(child);
                        visited_map[child_key] = child.index;
                        open_list.push({child.index, child_f});
                        total_states++;
                                         }
                }
            }
        }

        current.closed = true; // Mark current state as closed
    }

    std::cout << "No solution found. Total states explored: " << total_states << "\n";
}