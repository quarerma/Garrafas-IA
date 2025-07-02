#include "backtrack.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

bool is_goal_state(const GameState& state) {
    if (state.jars.empty()) return false;
    const Jar& last = state.jars.back();
    return last.current_value == (last.max_capacity / 3);
}

bool checkParentLoop(const GameState& child, int current_index, const std::vector<GameState>& game_states) {
    if (current_index < 0 || current_index >= static_cast<int>(game_states.size())) {
        return false;
    }
    if (child.parent == -1) {
        std::cout << "  Verificar Ancentral: Sem pai (estado raiz), ciclo impossível\n";
        return false;
    }
    if (child.parent < 0 || child.parent >= static_cast<int>(game_states.size())) {
        return false;
    }

    int ancestor_idx = child.parent;
    std::string child_key = child.to_key();
    while (ancestor_idx != -1) {
        if (game_states[ancestor_idx].to_key() == child_key) {
            std::cout << "  Verificar Ancentral: Chave do filho=" << child_key << ", Chave do ancestral=" << game_states[ancestor_idx].to_key()
                      << ", Ciclo detectado no índice do ancestral=" << ancestor_idx << "\n";
            return true;
        }
        ancestor_idx = game_states[ancestor_idx].parent;
    }
    std::cout << "  Verificar Ancentral: Chave do filho=" << child_key << ", Nenhum ciclo detectado nos ancestrais\n";
    return false;
}

bool generate_one_child(int current_index, std::vector<GameState>& game_states, int& action_index, GameState& child, int jar_idx) {
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
                std::cerr << "Error:estado invalido após ação " << action_index << " pra jarro " << i << std::endl;
                return false;
            }
        }
        child = GameState(new_jars, current_index);
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

void Backtrack::solve_with_backtracking(const std::vector<Jar>& initial_jars) {
    std::cout << "Resolvendo com backtracking...\n";
    if (initial_jars.empty()) {
        std::cout << "Erro: Lista de jarros vazia\n";
        return;
    }

    std::cout << "Estados iniciais:\n";
    for (const Jar& jar : initial_jars) {
        std::cout << "Jarro " << jar.id << ": " << jar.current_value << "/" << jar.max_capacity << std::endl;
    }

    std::vector<GameState> game_states;
    game_states.emplace_back(initial_jars, -1);
    std::vector<int> action_indices(1, 0);
    std::vector<int> jar_indices(1, 0);
    size_t total_states = 1;

    int current_index = 0;

    while (current_index >= 0) {
        std::cout << "\nProcessando estado " << current_index << ": jar_idx=" << jar_indices[current_index]
                  << ", action_idx=" << action_indices[current_index] << ", fechado=" << game_states[current_index].closed
                  << ", total_estados=" << total_states << "\n";
        std::cout << "  Valores do estado: ";
        for (const Jar& jar : game_states[current_index].jars) {
            std::cout << jar.current_value << "/" << jar.max_capacity << " ";
        }
        std::cout << "\n";

        if (is_goal_state(game_states[current_index])) {
            std::cout << "Meta atingida! Total de estados explorados: " << total_states << "\n";
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
            GameState child;
            if (generate_one_child(current_index, game_states, action_indices[current_index], child, jar_indices[current_index])) {
                game_states.push_back(child);
                action_indices.push_back(0);
                jar_indices.push_back(jar_indices[current_index]);
                action_indices[current_index]++;
                if (action_indices[current_index] > 3) {
                    action_indices[current_index] = 0;
                    jar_indices[current_index]++;
                    if (jar_indices[current_index] >= static_cast<int>(initial_jars.size())) {
                        game_states[current_index].closed = true;
                        std::cout << "  Estado " << current_index << " fechado: Todos os jarros e ações esgotados\n";
                    }
                }
                current_index = game_states.size() - 1;
                total_states++;
                std::cout << "  Movendo para novo estado filho " << current_index << "\n";
                continue;
            } else {
                action_indices[current_index]++;
                if (action_indices[current_index] > 3) {
                    action_indices[current_index] = 0;
                    jar_indices[current_index]++;
                    if (jar_indices[current_index] >= static_cast<int>(initial_jars.size())) {
                        game_states[current_index].closed = true;
                        std::cout << "  Estado " << current_index << " fechado: Todos os jarros e ações esgotados\n";
                    }
                }
                std::cout << "  Tentando próxima ação: jar_idx=" << jar_indices[current_index]
                          << ", action_idx=" << action_indices[current_index] << "\n";
            }
        }

        if (game_states[current_index].closed) {
            current_index = game_states[current_index].parent;
            std::cout << "  Retrocedendo para o estado pai " << current_index << "\n";
        }

        bool all_closed = true;
        for (const auto& state : game_states) {
            if (!state.closed) {
                all_closed = false;
                break;
            }
        }
        if (all_closed && current_index < 0) {
            std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << total_states << "\n";
            return;
        }
    }

    std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << total_states << "\n";
}