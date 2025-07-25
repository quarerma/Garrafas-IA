#ifndef PROFUNDIDADE_LARGURA_HPP
#define PROFUNDIDADE_LARGURA_HPP

#include "structure.hpp"
#include <vector>

class ProfundidadeLargura
{
public:
    void busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite);
    void busca_largura(const std::vector<Jar> &initial_jars);
};

#endif // PROFUNDIDADE_LARGURA_HPP