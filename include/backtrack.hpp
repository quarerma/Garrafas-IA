#ifndef BACKTRACK_HPP
#define BACKTRACK_HPP

#include "structure.hpp"
#include <vector>

class Backtrack {
public:
    void solve_with_backtracking(const std::vector<Jar>& initial_jars);
};

#endif // BACKTRACK_HPP