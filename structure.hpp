#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Jar {
public:
    int id;
    int current_value;
    int max_capacity;

    Jar(int jar_id, int capacity);

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
    bool closed;

    GameState(const vector<Jar>& j, int p);
    string to_key() const;
};

#endif // STRUCTURE_HPP