#include "executor.hpp"
#include "medidasBuscas.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
#include <set>

enum Acao {
    FILL,
    EMPTY,
    TRANSFER_LEFT,
    TRANSFER_RIGHT
};

void copiaEstadoPL(const GameState &origem, GameState &destino) {
    destino.jars = origem.jars;
    destino.values = origem.values;
    destino.parent = origem.parent;
    destino.closed = false;
    destino.visited = false;
    destino.g_cost = origem.g_cost;
    destino.f_cost = origem.f_cost;
    destino.index = origem.index;
    destino.target_Q = origem.target_Q;
    destino.max_cap = origem.max_cap;
    destino.num_jars = origem.num_jars;
}

bool geraFilhoPL(Acao acao, GameState &state, int indiceJarra, GameState &newState, vector<GameState> &vetorEstados) {
    if (indiceJarra < 0 || indiceJarra >= state.num_jars) {
        return false;
    }

    copiaEstadoPL(state, newState);
    bool valid_action = false;
    int action_cost = 0;

    switch (acao) {
    case FILL:
        if (!state.jars[indiceJarra].is_full()) {
            action_cost = state.jars[indiceJarra].space_left();
            newState.jars[indiceJarra].fill();
            valid_action = true;
        }
        break;
    case EMPTY:
        if (!state.jars[indiceJarra].is_empty()) {
            action_cost = state.jars[indiceJarra].current_value;
            newState.jars[indiceJarra].empty();
            valid_action = true;
        }
        break;
    case TRANSFER_LEFT:
        if (indiceJarra > 0 && !state.jars[indiceJarra].is_empty() && !state.jars[indiceJarra - 1].is_full()) {
            int transferred = state.get_transfer_value_from_jars(state.jars[indiceJarra], state.jars[indiceJarra - 1]);
            action_cost = transferred;
            newState.transfer_from_jars(newState.jars[indiceJarra], newState.jars[indiceJarra - 1]);
            valid_action = true;
        }
        break;
    case TRANSFER_RIGHT:
        if (indiceJarra < state.num_jars - 1 && !state.jars[indiceJarra].is_empty() && !state.jars[indiceJarra + 1].is_full()) {
            int transferred = state.get_transfer_value_from_jars(state.jars[indiceJarra], state.jars[indiceJarra + 1]);
            action_cost = transferred;
            newState.transfer_from_jars(newState.jars[indiceJarra], newState.jars[indiceJarra + 1]);
            valid_action = true;
        }
        break;
    }

    if (valid_action) {
        newState.g_cost += action_cost;
        newState.f_cost = newState.g_cost + newState.heuristic();
        newState.index = vetorEstados.size();
        newState.parent = state.index;
        vetorEstados.push_back(newState);
        return true;
    }
    return false;
}

void busca_profundidade_aux(GameState &state, int &profundidade, const int &profundidadeLimite, bool &noEncontrado, MedidasBusca &medidas, vector<GameState> &vetorEstados, std::set<std::string> &jaVisitados) {
    if (profundidade >= profundidadeLimite || noEncontrado) {
        return;
    }

    state.visited = true;
    medidas.nosVisitados++;

    state.print();

    if (state.is_goal()) {
        std::cout << endl << "Estado encontrado!" << endl;
        noEncontrado = true;
        medidas.profundidadeMax = state.print_path(state, state.index, vetorEstados);
        medidas.custoCaminho = state.g_cost;
        return;
    }

    profundidade++;

    for (int numJarro = 0; numJarro < state.num_jars; numJarro++) {
        GameState filho;
        if (geraFilhoPL(FILL, state, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            medidas.nosExpandidos++;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados, jaVisitados);
        }
        if (geraFilhoPL(EMPTY, state, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            medidas.nosExpandidos++;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados, jaVisitados);
        }
        if (geraFilhoPL(TRANSFER_LEFT, state, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            medidas.nosExpandidos++;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados, jaVisitados);
        }
        if (geraFilhoPL(TRANSFER_RIGHT, state, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            medidas.nosExpandidos++;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados, jaVisitados);
        }
    }

    profundidade--;
}

void SearchAlgorithms::busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite) {
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    vector<GameState> vetorEstados = {estadoInicial};
    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());

    MedidasBusca medidas;
    int profundidade = 0;
    std::clock_t c_start = std::clock();
    bool noEncontrado = false;

    busca_profundidade_aux(estadoInicial, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados, jaVisitados);

    std::clock_t c_end = std::clock();
    long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;
    medidas.tempoExecucao = time_ms;
    medidas.imprimirDados();
}

void SearchAlgorithms::busca_largura(const std::vector<Jar> &initial_jars) {
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    vector<GameState> vetorEstados = {estadoInicial};
    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());

    MedidasBusca medidas;
    std::clock_t c_start = std::clock();

    std::queue<GameState> abertos;
    abertos.push(estadoInicial);

    while (!abertos.empty()) {
        GameState estadoAtual = abertos.front();
        abertos.pop();
        medidas.nosVisitados++;

        estadoAtual.print();

        if (estadoAtual.is_goal()) {
            std::cout << endl << "Nó encontrado!" << endl;
            std::clock_t c_end = std::clock();
            long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;
            medidas.profundidadeMax = estadoAtual.print_path(estadoAtual, estadoAtual.index, vetorEstados);
            medidas.custoCaminho = estadoAtual.g_cost;
            medidas.tempoExecucao = time_ms;
            medidas.imprimirDados();
            return;
        }

        for (int numJarro = 0; numJarro < estadoAtual.num_jars; numJarro++) {
            GameState filho;
            if (geraFilhoPL(FILL, estadoAtual, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second) {
                medidas.nosExpandidos++;
                abertos.push(filho);
            }
            if (geraFilhoPL(EMPTY, estadoAtual, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second) {
                medidas.nosExpandidos++;
                abertos.push(filho);
            }
            if (geraFilhoPL(TRANSFER_LEFT, estadoAtual, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second) {
                medidas.nosExpandidos++;
                abertos.push(filho);
            }
            if (geraFilhoPL(TRANSFER_RIGHT, estadoAtual, numJarro, filho, vetorEstados) && jaVisitados.insert(filho.to_key()).second) {
                medidas.nosExpandidos++;
                abertos.push(filho);
            }
        }
    }
}