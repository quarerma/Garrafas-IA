#include "structure.hpp"

Jar::Jar(int jar_id, int capacity)
    : id(jar_id), current_value(0), max_capacity(capacity) {}

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
    if (this == nullptr) {
        std::cerr << "Error: Null Jar pointer" << std::endl;
        return false;
    }
    std::cout << "Current value: " << current_value << std::endl;
    return current_value == 0;
}

int Jar::get_capacity() const {
    return max_capacity;
}

int Jar::space_left() const {
    return max_capacity - current_value;
}

int Jar::transfer(int amount) {
    int accepted = min(amount, space_left());
    current_value += accepted;
    return amount - accepted;
}

GameState::GameState(const vector<Jar>& j, int p) : jars(j), parent(p), closed(false) {
    for (const Jar& jar : j) {
        values.push_back(jar.current_value);
    }
}

string GameState::to_key() const {
    string key;
    for (int val : values) {
        key += to_string(val) + "|";
    }
    return key;
}