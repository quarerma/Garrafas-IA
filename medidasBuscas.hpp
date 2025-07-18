#ifndef MEDIDASBUSCA_HPP
#define MEDIDASBUSCA_HPP

#include <iostream>

class MedidasBusca
{
public:
    int profundidadeMax = 0;
    int nosVisitados = 0;
    int nosExpandidos = 0;
    int totalFilhosGerados = 0;
    double tempoExecucao = 0.0;

    void imprimirDados() const
    {
        std::cout << "\n--- Dados da Busca ---\n";
        std::cout << "Tempo de execucao: " << tempoExecucao << " ms\n";
        std::cout << "Profundidade: " << profundidadeMax << "\n";
        std::cout << "Nos visitados: " << nosVisitados << "\n";
        std::cout << "Nos expandidos: " << nosExpandidos << "\n";
        std::cout << "Media de ramificacao: ";
        if (nosExpandidos > 0)
            std::cout << static_cast<double>(totalFilhosGerados) / nosExpandidos << "\n";
        else
            std::cout << "0\n";
        std::cout << "-----------------------\n";
    }
};

#endif // MEDIDASBUSCA_HPP