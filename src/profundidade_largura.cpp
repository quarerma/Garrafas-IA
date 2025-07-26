#include "executor.hpp"
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

bool geraFilhoPL(Acao acao, GameState &state, int indiceJarra, GameState &newState, std::vector<GameState>& states) {
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
        newState.index = states.size();
        states.push_back(newState);
        return true;
    }
    return false;
}

void busca_profundidade_aux(GameState &state, int &profundidade, const int &profundidadeLimite, bool &noEncontrado, std::vector<GameState>& states, std::set<std::string> &jaVisitados) {
    if (profundidade >= profundidadeLimite || noEncontrado) {
        return;
    }

    state.visited = true;

    state.print();

    if (state.is_goal()) {
        std::cout << endl << "Estado encontrado!" << endl;
        noEncontrado = true;
        return;
    }

    profundidade++;

    bool all_actions_exhausted = true;
    for (int numJarro = 0; numJarro < state.num_jars; numJarro++) {
        GameState filho;
        if (geraFilhoPL(FILL, state, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            all_actions_exhausted = false;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        if (geraFilhoPL(EMPTY, state, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            all_actions_exhausted = false;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        if (geraFilhoPL(TRANSFER_LEFT, state, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            all_actions_exhausted = false;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        if (geraFilhoPL(TRANSFER_RIGHT, state, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second && !noEncontrado) {
            all_actions_exhausted = false;
            busca_profundidade_aux(filho, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
    }

    if (all_actions_exhausted) {
        state.closed = true;
    }

    profundidade--;
}

void SearchAlgorithms::busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    states.push_back(estadoInicial);

    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());

    int profundidade = 0;
    bool noEncontrado = false;

    busca_profundidade_aux(estadoInicial, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);

}

void SearchAlgorithms::busca_largura(const std::vector<Jar> &initial_jars) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    states.push_back(estadoInicial);

    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());


    std::queue<GameState> abertos;
    abertos.push(estadoInicial);

    while (!abertos.empty()) {
        GameState estadoAtual = abertos.front();
        abertos.pop();

        estadoAtual.visited = true;
        estadoAtual.print();

        if (estadoAtual.is_goal()) {
            std::cout << endl << "Nó encontrado!" << endl;
            return;
        }

        bool all_actions_exhausted = true;
        for (int numJarro = 0; numJarro < estadoAtual.num_jars; numJarro++) {
            GameState filho;
            if (geraFilhoPL(FILL, estadoAtual, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second) {
                all_actions_exhausted = false;
                abertos.push(filho);
            }
            if (geraFilhoPL(EMPTY, estadoAtual, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second) {
                all_actions_exhausted = false;
                abertos.push(filho);
            }
            if (geraFilhoPL(TRANSFER_LEFT, estadoAtual, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second) {
                all_actions_exhausted = false;
                abertos.push(filho);
            }
            if (geraFilhoPL(TRANSFER_RIGHT, estadoAtual, numJarro, filho, states) && jaVisitados.insert(filho.to_key()).second) {
                all_actions_exhausted = false;
                abertos.push(filho);
            }
        }

        if (all_actions_exhausted) {
            estadoAtual.closed = true;
        }
    }
}