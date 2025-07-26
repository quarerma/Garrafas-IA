#include "executor.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <set>

enum Acao {
    FILL,
    EMPTY,
    TRANSFER_LEFT,
    TRANSFER_RIGHT
};

void copiaEstadoOG(const GameState &origem, GameState &destino) {
    destino.jars = origem.jars;
    destino.values = origem.values;
    destino.g_cost = origem.g_cost;
    destino.f_cost = origem.f_cost;
    destino.parent = origem.parent;
    destino.closed = false;
    destino.visited = false;
    destino.target_Q = origem.target_Q;
    destino.max_cap = origem.max_cap;
    destino.num_jars = origem.num_jars;
}

bool geraFilhoOG(Acao acao, GameState &state, int indiceJarra, GameState &newState, vector<GameState> &states) {
    if (indiceJarra < 0 || indiceJarra >= state.num_jars) {
        return false;
    }

    copiaEstadoOG(state, newState);
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
        newState.index = states.size();
        states.push_back(newState);
        return true;
    }
    return false;
}

bool comparaPorCusto(const GameState &stateA, const GameState &stateB) {
    return stateA.g_cost < stateB.g_cost; // Menor custo primeiro
}

bool comparaPorHeuristica(const GameState &stateA, const GameState &stateB) {
    return stateA.heuristic() < stateB.heuristic(); // Menor heurística primeiro
}

void SearchAlgorithms::busca_ordenada(const std::vector<Jar> &initial_jars) {
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    std::set<std::string> jaVisitados;
    std::deque<GameState> abertos;
    abertos.push_back(estadoInicial);
    jaVisitados.insert(estadoInicial.to_key());

    while (!abertos.empty()) {
        GameState estadoAtual = abertos.front();
        abertos.pop_front();

        estadoAtual.print();

        if (estadoAtual.is_goal()) {
            std::cout << endl << "Nó encontrado!" << endl;
            estadoAtual.print_path(estadoAtual, estadoAtual.index, states);
            return;
        }

        vector<GameState> filhosNovosOrdenados;

        for (int numJarro = 0; numJarro < estadoAtual.num_jars; numJarro++) {
            GameState filho;
            if (geraFilhoOG(FILL, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(EMPTY, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(TRANSFER_LEFT, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(TRANSFER_RIGHT, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
        }

        std::sort(filhosNovosOrdenados.begin(), filhosNovosOrdenados.end(), comparaPorCusto);

        for (auto it = filhosNovosOrdenados.rbegin(); it != filhosNovosOrdenados.rend(); ++it) {
            if (jaVisitados.insert(it->to_key()).second) {
                abertos.push_front(*it);
            }
        }
    }
}

void SearchAlgorithms::busca_gulosa(const std::vector<Jar> &initial_jars) {
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    std::set<std::string> jaVisitados;
    std::deque<GameState> abertos;
    abertos.push_back(estadoInicial);
    jaVisitados.insert(estadoInicial.to_key());

    while (!abertos.empty()) {
        GameState estadoAtual = abertos.front();
        abertos.pop_front();

        estadoAtual.print();

        if (estadoAtual.is_goal()) {
            std::cout << endl << "Nó encontrado!" << endl;
            estadoAtual.print_path(estadoAtual, estadoAtual.index, states);
            return;
        }

        vector<GameState> filhosNovosOrdenados;

        for (int numJarro = 0; numJarro < estadoAtual.num_jars; numJarro++) {
            GameState filho;
            if (geraFilhoOG(FILL, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(EMPTY, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(TRANSFER_LEFT, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
            if (geraFilhoOG(TRANSFER_RIGHT, estadoAtual, numJarro, filho, states)) {
                filhosNovosOrdenados.push_back(filho);
            }
        }

        std::sort(filhosNovosOrdenados.begin(), filhosNovosOrdenados.end(), comparaPorHeuristica);

        for (auto it = filhosNovosOrdenados.rbegin(); it != filhosNovosOrdenados.rend(); ++it) {
            if (jaVisitados.insert(it->to_key()).second) {
                abertos.push_front(*it);
            }
        }
    }
}