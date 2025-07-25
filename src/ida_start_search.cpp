#include "structure.hpp"
#include <unordered_map>
#include <set>
#include <limits>
#include <algorithm>
#include <vector>

// Checks if the child state creates a cycle with its ancestors
bool checkParentLoop(const GameState& child, int current_index, const std::vector<GameState>& game_states) {
    if (current_index < 0 || current_index >= static_cast<int>(game_states.size())) {
        return false;
    }
    if (child.parent == -1) {
        std::cout << "  Verificar Ancestral: Sem pai (estado raiz), ciclo impossível\n";
        return false;
    }
    if (child.parent < 0 || child.parent >= static_cast<int>(game_states.size())) {
        return false;
    }

    int ancestor_idx = child.parent;
    std::string child_key = child.to_key();
    while (ancestor_idx != -1) {
        if (game_states[ancestor_idx].to_key() == child_key) {
            std::cout << "  Verificar Ancestral: Chave do filho=" << child_key << ", Chave do ancestral=" << game_states[ancestor_idx].to_key()
                      << ", Ciclo detectado no índice do ancestral=" << ancestor_idx << "\n";
            return true;
        }
        ancestor_idx = game_states[ancestor_idx].parent;
    }
    std::cout << "  Verificar Ancestral: Chave do filho=" << child_key << ", Nenhum ciclo detectado nos ancestrais\n";
    return false;
}

// Generates one child state for the given jar_idx and action_index
bool generate_one_child(int current_index, std::vector<GameState>& game_states, int action_index, GameState& child, int jar_idx) {
    std::cout << "Gerando filho para o estado " << current_index << ", jar_idx=" << jar_idx << ", action_index=" << action_index << "\n";

    if (current_index < 0 || current_index >= static_cast<int>(game_states.size())) {
        return false;
    }

    std::vector<Jar> jars = game_states[current_index].jars;
    int num_jars = jars.size();
    int action_type = action_index;

    if (action_type > 3 || (action_type == 3 && jar_idx == num_jars - 1)) {
        std::cout << "  Ação inválida: action_type=" << action_type << ", jar_idx=" << jar_idx << ", num_jars=" << num_jars << "\n";
        return false;
    }

    if (jar_idx >= num_jars) {
        return false;
    }

    if (jars[jar_idx].max_capacity <= 0 || jars[jar_idx].current_value < 0 || jars[jar_idx].current_value > jars[jar_idx].max_capacity) {
        return false;
    }

    std::cout << "  Estado atual: ";
    for (const Jar& jar : jars) {
        std::cout << jar.current_value << "/" << jar.max_capacity << " ";
    }
    std::cout << "\n";

    std::vector<Jar> new_jars = jars;
    bool valid_action = false;
    std::string action_name;

    switch (action_type) {
        case 0: // Empty
            action_name = "Esvaziar";
            if (!jars[jar_idx].is_empty()) {
                new_jars[jar_idx].empty();
                valid_action = true;
            } else {
                std::cout << "  Inválido: Não pode esvaziar um jarro vazio\n";
            }
            break;
        case 1: // Fill
            action_name = "Encher";
            if (!jars[jar_idx].is_full()) {
                new_jars[jar_idx].fill();
                valid_action = true;
            } else {
                std::cout << "  Inválido: Não pode encher um jarro cheio\n";
            }
            break;
        case 2: // Transfer Left
            action_name = "Transferir Esquerda";
            if (jar_idx > 0 && !jars[jar_idx].is_empty() && !jars[jar_idx - 1].is_full()) {
                int amount = new_jars[jar_idx].current_value;
                int difference = new_jars[jar_idx - 1].transfer(amount);
                new_jars[jar_idx].current_value = difference;
                valid_action = true;
            } else {
                std::cout << "  Inválido: Não pode transferir para esquerda (jar_idx=" << jar_idx << ", origem vazia=" << jars[jar_idx].is_empty()
                          << ", destino cheio=" << (jar_idx > 0 ? jars[jar_idx - 1].is_full() : true) << ")\n";
            }
            break;
        case 3: // Transfer Right
            action_name = "Transferir Direita";
            if (jar_idx < num_jars - 1 && !jars[jar_idx].is_empty() && !jars[jar_idx + 1].is_full()) {
                int amount = new_jars[jar_idx].current_value;
                int difference = new_jars[jar_idx + 1].transfer(amount);
                new_jars[jar_idx].current_value = difference;
                valid_action = true;
            } else {
                std::cout << "  Inválido: Não pode transferir para direita (jar_idx=" << jar_idx << ", origem vazia=" << jars[jar_idx].is_empty()
                          << ", destino cheio=" << (jar_idx < num_jars - 1 ? jars[jar_idx + 1].is_full() : true) << ")\n";
            }
            break;
    }

    std::cout << "  Ação tentada: " << action_name << " no jarro " << jar_idx << ", Válida=" << (valid_action ? "Sim" : "Não") << "\n";

    if (valid_action) {
        for (size_t i = 0; i < new_jars.size(); ++i) {
            if (new_jars[i].current_value < 0 || new_jars[i].current_value > new_jars[i].max_capacity) {
                std::cerr << "Error: estado inválido após ação " << action_index << " para jarro " << i << std::endl;
                return false;
            }
        }
        child = GameState(new_jars, current_index);
        child.g_cost = game_states[current_index].g_cost + game_states[current_index].calculate_action_cost(jar_idx, action_type);
        child.f_cost = child.g_cost + child.heuristic();
        child.closed = false;
        child.visited = false;

        std::cout << "  Novo estado filho: ";
        for (const Jar& jar : child.jars) {
            std::cout << jar.current_value << "/" << jar.max_capacity << " ";
        }
        std::cout << "\n";

        if (checkParentLoop(child, current_index, game_states)) {
            std::cout << "  Filho rejeitado: Igual a um estado ancestral\n";
            return false;
        }
        std::cout << "  Filho aceito\n";
        return true;
    }

    std::cout << "  Filho rejeitado: Ação inválida\n";
    return false;
}

void solve_with_ida_star(const std::vector<Jar>& initial_jars) {
    if (initial_jars.empty()) {
        std::cout << "Error: Empty jar list\n";
        return;
    }

    std::cout << "Solving with IDA*...\n";
    std::cout << "Initial state:\n";
    for (const Jar& jar : initial_jars) {
        std::cout << "Jar " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << std::endl;
    }

    int threshold = 0; // Initial max_value (threshold), starting with 0
    int patamar_old = -1; // Tracks previous threshold to detect no progress
    int iteration = 0;

    while (true) {
        std::cout << "\nIteration " << ++iteration << " with threshold " << threshold << "\n";

        if (patamar_old == threshold) {
            std::cout << "No solution found: threshold unchanged. Total states explored: " << game_states.size() << "\n";
            return;
        }

        // Initialize game_states with only the initial state
        std::vector<GameState> game_states;
        game_states.emplace_back(initial_jars, -1);
        game_states[0].index = 0;
        game_states[0].g_cost = 0;
        game_states[0].f_cost = game_states[0].heuristic();
        game_states[0].closed = false;
        game_states[0].visited = false;

        std::unordered_map<std::string, int> visited_map;
        visited_map[game_states[0].to_key()] = 0;

        std::vector<int> jar_indices(1, 0);
        std::vector<int> action_indices(1, 0);
        int total_states = 1;
        int current_index = 0;
        std::set<int> discarded; // Tracks f_cost of pruned states

        while (current_index >= 0) {
            GameState& current = game_states[current_index];
            int current_f = current.f_cost;

            std::cout << "\nProcessando estado " << current_index << ": jar_idx=" << jar_indices[current_index]
                      << ", action_idx=" << action_indices[current_index] << ", closed=" << current.closed
                      << ", visited=" << current.visited << ", total_estados=" << total_states << "\n";
            std::cout << "  Valores do estado: ";
            current.print();
            std::cout << "g_cost=" << current.g_cost << ", h=" << current.heuristic() << ", f=" << current_f << "\n";

            // Prune if f_cost exceeds threshold
            if (current_f > threshold) {
                discarded.insert(current_f);
                current.visited = true;
                std::cout << "  Pruned: f=" << current_f << " exceeds threshold " << threshold << "\n";
            }

            // Check if goal is reached and within threshold
            if (current.is_goal() && current_f <= threshold) {
                std::cout << "Goal reached! Total states explored: " << total_states << "\n";
                current.print_path(current, current_index, game_states);
                return;
            }

            if (!current.visited && !current.closed) {
                GameState child;
                if (generate_one_child(current_index, game_states, action_indices[current_index], child, jar_indices[current_index])) {
                    std::string child_key = child.to_key();
                    auto it = visited_map.find(child_key);
                    if (it != visited_map.end()) {
                        int existing_idx = it->second;
                        if (game_states[existing_idx].visited || child.g_cost >= game_states[existing_idx].g_cost) {
                            action_indices[current_index]++;
                            if (action_indices[current_index] > 3) {
                                action_indices[current_index] = 0;
                                jar_indices[current_index]++;
                                if (jar_indices[current_index] >= static_cast<int>(current.jars.size())) {
                                    current.closed = true;
                                    std::cout << "  Estado " << current_index << " fechado: Todos os jarros e ações esgotados\n";
                                }
                            }
                            std::cout << "  Filho rejeitado: Estado existente com custo melhor ou visitado\n";
                            continue;
                        }
                        // Update existing state
                        game_states[existing_idx].g_cost = child.g_cost;
                        game_states[existing_idx].f_cost = child.f_cost;
                        game_states[existing_idx].parent = current_index;
                        game_states[existing_idx].closed = false;
                        game_states[existing_idx].visited = false;
                        std::cout << "  Updated state " << existing_idx << ": ";
                        game_states[existing_idx].print();
                        std::cout << "g_cost=" << child.g_cost << ", h=" << child.heuristic() << ", f=" << child.f_cost << "\n";
                        child.index = existing_idx;
                    } else {
                        // New state
                        child.index = game_states.size();
                        game_states.push_back(child);
                        visited_map[child_key] = child.index;
                        total_states++;
                    }

                    action_indices.push_back(0);
                    jar_indices.push_back(0);
                    action_indices[current_index]++;
                    if (action_indices[current_index] > 3) {
                        action_indices[current_index] = 0;
                        jar_indices[current_index]++;
                        if (jar_indices[current_index] >= static_cast<int>(current.jars.size())) {
                            current.closed = true;
                            std::cout << "  Estado " << current_index << " fechado: Todos os jarros e ações esgotados\n";
                        }
                    }
                    current.visited = true;
                    current_index = child.index;
                    std::cout << "  Movendo para novo estado filho " << current_index << "\n";
                    continue;
                } else {
                    action_indices[current_index]++;
                    if (action_indices[current_index] > 3) {
                        action_indices[current_index] = 0;
                        jar_indices[current_index]++;
                        if (jar_indices[current_index] >= static_cast<int>(current.jars.size())) {
                            current.closed = true;
                            std::cout << "  Estado " << current_index << " fechado: Todos os jarros e ações esgotados\n";
                        }
                    }
                    std::cout << "  Tentando próxima ação: jar_idx=" << jar_indices[current_index]
                              << ", action_idx=" << action_indices[current_index] << "\n";
                }
            }

            if (current.closed) {
                current_index = current.parent;
                if (current_index >= 0) {
                    action_indices.pop_back();
                    jar_indices.pop_back();
                }
                std::cout << "  Retrocedendo para o estado pai " << current_index << "\n";
            }

            if (current_index < 0) {
                std::cout << "No solution within threshold " << threshold << ". Total states explored in this iteration: " << total_states << "\n";
                break;
            }
        }

        // Update threshold using minimum f_cost from discarded states
        if (discarded.empty()) {
            std::cout << "No solution found: no states pruned to increase threshold.\n";
            return;
        }
        patamar_old = threshold;
        threshold = *discarded.begin(); // Minimum f_cost from pruned states
        std::cout << "Increasing threshold to " << threshold << "\n";
    }
}