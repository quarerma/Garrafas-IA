#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Jar
{
public:
    int id;
    int current_value;
    int max_capacity;

    Jar(int jar_id, int capacity, int curr_value);

    void fill();
    void empty();
    bool is_full() const;
    bool is_empty() const;
    int get_capacity() const;
    int space_left() const;
    int transfer(int amount);
};

class GameState
{
public:
    vector<Jar> jars;
    vector<int> values;
    int parent;
    bool closed;
    int index;
    int custoCaminho;
    int heuristica;

    GameState() : index(0), parent(-1), closed(false), custoCaminho(0) {}
    GameState(const vector<Jar> &j, int p);
    string to_key() const;
    void transfer_from_jars(Jar &jarOrigin, Jar &jarDestination);
    void print() const;
    int imprimeCaminho(const GameState &noFinal, int indiceNoFinal, std::vector<GameState> &estados);
    int get_transfer_value_from_jars(Jar &jarOrigin, Jar &jarDestination);
};

#endif // STRUCTURE_HPP