#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

class Jar {
public:
    int id;
    int current_value;
    int max_capacity;

    Jar(int jar_id, int capacity, int curr_value = 0);

    void fill();
    void empty();
    bool is_full() const;
    bool is_empty() const;
    int get_capacity() const;
    int space_left() const;
    int transfer(int amount);
};

class GameState {
public:
    vector<Jar> jars;
    vector<int> values;
    int parent;
    bool visited;
    bool closed;
    int g_cost; // Path cost to node
    int f_cost; // f = g + h
    int index;
    int target_Q;
    int max_cap;
    int num_jars;

    GameState();
    GameState(const vector<Jar>& j, int p);
    string to_key() const;
    bool is_goal() const;
    int get_g_cost() const;
    int heuristic() const;
    int calculate_action_cost(int jar_idx, int action_type) const;
    void transfer_from_jars(Jar& jarOrigin, Jar& jarDestination);
    int get_transfer_value_from_jars(Jar& jarOrigin, Jar& jarDestination);

};

#endif // STRUCTURE_HPP