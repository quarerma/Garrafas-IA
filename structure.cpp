#include <vector>
#include <string>
#include <algorithm> 
#include <climits>  
#include "structure.hpp"
#include <iostream> 

using namespace std; 

struct Jar {
    int id;
    int current_value;
    int max_capacity;

    Jar(int jar_id, int capacity, int curr_value = 0)
        : id(jar_id), current_value(curr_value), max_capacity(capacity) {}

    void fill() {
        current_value = max_capacity;
    }

    void empty() {
        current_value = 0;
    }

    bool is_full() const {
        return current_value == max_capacity;
    }

    bool is_empty() const {
        if (this == nullptr) {
            std::cerr << "Error: Null Jar pointer" << std::endl;
            return false;
        }
        return current_value == 0;
    }

    int get_capacity() const {
        return max_capacity;
    }

    int space_left() const {
        return max_capacity - current_value;
    }

    int transfer(int amount) {
        int accepted = std::min(amount, space_left());
        current_value += accepted;
        return amount - accepted;
    }
};

struct GameState {
    std::vector<Jar> jars;
    std::vector<int> values;
    int parent;
    bool closed;
    int g_cost; 
    int index; 
    int target_Q;
    int max_cap;
    int num_jars;

    GameState(const std::vector<Jar>& j, int p) : jars(j), parent(j), closed(p), g_cost(false), index(0), g_cost(0) {
        num_jars = j.size();
        target_Q = (num_jars > 0) ? INT_MAX : 0;
        max_cap = 0;
        for (const Jar& jar : j) {
            values.push_back(jar.current_value);
            target_Q = std::min(target_Q, jar.max_capacity);
            max_cap = std::max(max_cap, jar.max_capacity);
        }
    }

    std::string to_key() const {
        std::string key;
        for (int val : values) {
            key += std::to_string(val) + "|";
        }
        return key;
    }

    bool is_goal() const {
        if (jars.empty()) return false;
        for (const Jar& jar : jars) {
            if (jar.current_value != target_Q) return false;
        }
        return true;
    }

    int get_g_cost() const {
        return g_cost;
    }

    int heuristic() const {
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

    int calculate_action_cost(int jar_idx, int action_type) const {
        if (jar_idx < 0 || jar_idx >= num_jars) return -1;
        switch (action_type) {
            case 0: // Empty
                if (jars[jar_idx].is_empty()) return 0;
                return jars[jar_idx].current_value;
            case 1: // Fill
                if (jars[jar_idx].is_full()) return 0;
                return jars[jar_idx].space_left();
            case 2: // Transfer Left
                if (jar_idx <= 0 || jars[jar_idx].is_empty() || jars[jar_idx - 1].is_full()) return 0;
                return std::min(jars[jar_idx].current_value, jars[jar_idx - 1].space_left());
            case 3: // Transfer Right
                if (jar_idx >= num_jars - 1 || jars[jar_idx].is_empty() || jars[jar_idx + 1].is_full()) return 0;
                return std::min(jars[jar_idx].current_value, jars[jar_idx + 1].space_left());
            default:
                return -1;
        }
    }

    void transfer_from_jars(Jar &jarOrigin, Jar &jarDestination) {
        int availableSpace = jarDestination.get_capacity() - jarDestination.current_value;
        int transferAmount = std::min(jarOrigin.current_value, availableSpace);
        jarOrigin.current_value -= transferAmount;
        jarDestination.current_value += transferAmount;
    }

    int get_transfer_value_from_jars(Jar &jarOrigin, Jar &jarDestination) {
        int availableSpace = jarDestination.get_capacity() - jarDestination.current_value;
        int transferAmount = std::min(jarOrigin.current_value, availableSpace);
        return transferAmount;
    }

    void print() const {
        std::cout << "(";
        for (const Jar &jar : jars) {
            std::cout << jar.current_value << "/" << jar.max_capacity << " ";
        }
        std::cout << ") Custo Caminho: " << g_cost << ", Index: " << index << ", Closed: " << (closed ? "true" : "false") << to_key() << "\n";
    }

    int imprimeCaminho(const GameState &noFinal, int indiceNoFinal, std::vector<GameState> &estados) {
        std::cout << endl
                  << "Caminho até o nó final: " << endl;
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
};