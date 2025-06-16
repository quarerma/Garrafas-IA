#include <iostream>

using namespace std;

class Jar {
public:
    int id;
    int current_value;
    

    Jar(int jar_id, int capacity);
    
    void fill();
    bool is_full() const;
    bool is_empty() const;
    int get_capacity() const;

private:
    int max_capacity;
};

Jar::Jar(int jar_id, int capacity) {
    id = jar_id;
    max_capacity = capacity;
    current_value = 0;
}

int Jar::fill(int amount) {
    if (current_value + amount <= max_capacity) {
        current_value += amount;
    } else {
        cout << "Cannot fill jar " << id << ": exceeds capacity." << endl;
    }

    return max_capacity - (current_value + amount);
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