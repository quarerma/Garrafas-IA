#ifndef ORDENADA_GULOSA_HPP
#define ORDENADA_GULOSA_HPP

#include "structure.hpp"
#include <vector>

class OrdenadaGulosa
{
public:
    void busca_ordenada(const std::vector<Jar> &initial_jars);
    void busca_gulosa(const std::vector<Jar> &initial_jars);
};

#endif // ORDENADA_GULOSA_HPP