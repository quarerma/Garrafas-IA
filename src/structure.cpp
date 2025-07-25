#include "structure.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <limits>

using namespace std;

Jar::Jar(int jar_id, int capacity, int curr_value)
    : id(jar_id), current_value(curr_value), max_capacity(capacity) {}

void Jar::fill() {
    current_value = max_capacity;
}

void Jar::empty() {
    current_value = 0;
}

bool Jar::is_full() const {
    return current_value == max_capacity;
}

bool Jar::is_empty() const {
    return current_value == 0;
}

int Jar::get_capacity() const {
    return max_capacity;
}

int Jar::space_left() const {
    return max_capacity - current_value;
}

int Jar::transfer(int amount) {
    int accepted = std::min(amount, space_left());
    current_value += accepted;
    return amount - accepted;
}

GameState::GameState()
    : parent(-1), closed(false), g_cost(0), f_cost(0), index(-1), target_Q(0), max_cap(0), num_jars(0) {}

GameState::GameState(const vector<Jar>& j, int p)
    : jars(j), parent(p), closed(false), g_cost(0), f_cost(0), num_jars(j.size()) {
    values.reserve(num_jars);
    max_cap = 0;
    target_Q = (num_jars > 0) ? INT_MAX : 0;
    for (const Jar& jar : j) {
        values.push_back(jar.current_value);
        target_Q = std::min(target_Q, jar.max_capacity);
        max_cap = std::max(max_cap, jar.max_capacity);
    }
}

string GameState::to_key() const {
    string key;
    for (int val : values) {
        key += std::to_string(val) + "|";
    }
    return key;
}

bool GameState::is_goal() const {
    if (jars.empty()) return false;
    for (const Jar& jar : jars) {
        if (jar.current_value != target_Q) return false;
    }
    return true;
}

int GameState::get_g_cost() const {
    return g_cost;
}

int GameState::heuristic() const {
    int current_sum = 0;
    int max_individual_diff = 0;
    for (const Jar& jar : jars) {
        current_sum += jar.current_value;
        int diff = std::abs(jar.current_value - target_Q);
        max_individual_diff = std::max(max_individual_diff, diff);
    }
    int target_sum = target_Q * num_jars;
    int sum_diff = std::abs(current_sum - target_sum);
    int sum_adjust_steps = (sum_diff + max_cap - 1) / max_cap;
    int individual_adjust_steps = (max_individual_diff + max_cap - 1) / max_cap;
    return std::max(sum_adjust_steps, individual_adjust_steps);
}

int GameState::calculate_action_cost(int jar_idx, int action_type) const {
    if (jar_idx < 0 || jar_idx >= num_jars) return std::numeric_limits<int>::max();
    switch (action_type) {
        case 0: // Empty
            if (jars[jar_idx].is_empty()) return std::numeric_limits<int>::max();  // Ação impossível: já vazio
            return jars[jar_idx].current_value;
        case 1: // Fill
            if (jars[jar_idx].is_full()) return std::numeric_limits<int>::max();  // Ação impossível: já cheio
            return jars[jar_idx].space_left();
        case 2: // Transfer Left
            if (jar_idx <= 0 || jars[jar_idx].is_empty() || jars[jar_idx - 1].is_full()) return std::numeric_limits<int>::max();  // Ação impossível
            return std::min(jars[jar_idx].current_value, jars[jar_idx - 1].space_left());
        case 3: // Transfer Right
            if (jar_idx >= num_jars - 1 || jars[jar_idx].is_empty() || jars[jar_idx + 1].is_full()) return std::numeric_limits<int>::max();  // Ação impossível
            return std::min(jars[jar_idx].current_value, jars[jar_idx + 1].space_left());
        default:
            return std::numeric_limits<int>::max();  // Ação inválida
    }
}

void GameState::transfer_from_jars(Jar &jarOrigin, Jar &jarDestination) {
    int availableSpace = jarDestination.get_capacity() - jarDestination.current_value;
    int transferAmount = std::min(jarOrigin.current_value, availableSpace);
    jarOrigin.current_value -= transferAmount;
    jarDestination.current_value += transferAmount;
}

int GameState::get_transfer_value_from_jars(Jar &jarOrigin, Jar &jarDestination) {
    int availableSpace = jarDestination.get_capacity() - jarDestination.current_value;
    int transferAmount = std::min(jarOrigin.current_value, availableSpace);
    return transferAmount;
}

void GameState::print() const {
    std::cout << "(";
    for (const Jar &jar : jars) {
        std::cout << jar.current_value << "/" << jar.max_capacity << " ";
    }
    std::cout << ") Custo Caminho: " << g_cost << ", Index: " << index << ", Closed: " << (closed ? "true" : "false") << to_key() << "\n";
}

int GameState::print_path(const GameState& noFinal, int indiceNoFinal, vector<GameState>& estados) {
    std::cout << endl << "Caminho até o nó final: " << endl;
    int noAtualIndice = noFinal.parent;
    vector<int> caminho;
    caminho.push_back(indiceNoFinal);
    while (noAtualIndice != -1) {
        caminho.push_back(noAtualIndice);
        noAtualIndice = estados[noAtualIndice].parent;
    }
    for (int i = caminho.size() - 1; i >= 0; i--) {
        estados[caminho[i]].print();
    }
    return caminho.size();
}