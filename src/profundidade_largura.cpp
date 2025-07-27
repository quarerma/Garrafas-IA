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
        newState.g_cost = state.g_cost + action_cost; // Explicitly set g_cost from parent
        newState.f_cost = newState.g_cost + newState.heuristic();
        // Sync values from updated jars
        for (int i = 0; i < newState.num_jars; ++i) {
            newState.values[i] = newState.jars[i].current_value;
        }
        newState.parent = state.index;
        newState.index = states.size();
        states.push_back(newState);
        return true;
    }
    return false;
}

void busca_profundidade_aux(GameState &state, int &profundidade, const int &profundidadeLimite, bool &noEncontrado, std::vector<GameState>& states, std::set<std::string> &jaVisitados) {
    state.closed = true;
    state.visited = true;
    if (profundidade >= profundidadeLimite || noEncontrado) {
        return;
    }

    if (state.is_goal()) {
        noEncontrado = true;
       
        return;
    }

    profundidade++;

    for (int numJarro = 0; numJarro < state.num_jars; ++numJarro) {
        GameState filhoFill;
        if (geraFilhoPL(FILL, state, numJarro, filhoFill, states) && jaVisitados.insert(filhoFill.to_key()).second && !noEncontrado) {
            busca_profundidade_aux(filhoFill, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        GameState filhoEmpty;
        if (geraFilhoPL(EMPTY, state, numJarro, filhoEmpty, states) && jaVisitados.insert(filhoEmpty.to_key()).second && !noEncontrado) {
            busca_profundidade_aux(filhoEmpty, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        GameState filhoTransferLeft;
        if (geraFilhoPL(TRANSFER_LEFT, state, numJarro, filhoTransferLeft, states) && jaVisitados.insert(filhoTransferLeft.to_key()).second && !noEncontrado) {
            busca_profundidade_aux(filhoTransferLeft, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
        GameState filhoTransferRight;
        if (geraFilhoPL(TRANSFER_RIGHT, state, numJarro, filhoTransferRight, states) && jaVisitados.insert(filhoTransferRight.to_key()).second && !noEncontrado) {
            busca_profundidade_aux(filhoTransferRight, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);
        }
    }

    profundidade--;
}

void SearchAlgorithms::busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    estadoInicial.g_cost = 0; // Initialize g_cost for initial state
    states.push_back(estadoInicial);

    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());

    int profundidade = 0;
    bool noEncontrado = false;

    busca_profundidade_aux(estadoInicial, profundidade, profundidadeLimite, noEncontrado, states, jaVisitados);

    if (!noEncontrado) {
        std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << states.size() << "\n";
    }
}

void SearchAlgorithms::busca_largura(const std::vector<Jar> &initial_jars) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    estadoInicial.g_cost = 0; // Initialize g_cost for initial state
    states.push_back(estadoInicial);

    std::set<std::string> jaVisitados;
    jaVisitados.insert(estadoInicial.to_key());

    std::queue<GameState> abertos;
    abertos.push(estadoInicial);

    while (!abertos.empty()) {
        GameState estadoAtual = abertos.front();
        abertos.pop();

        states[estadoAtual.index].closed = true;
        states[estadoAtual.index].visited = true;

        if (estadoAtual.is_goal()) {
           
            return;
        }

        for (int numJarro = 0; numJarro < estadoAtual.num_jars; ++numJarro) {
            GameState filhoFill;
            if (geraFilhoPL(FILL, estadoAtual, numJarro, filhoFill, states) && jaVisitados.insert(filhoFill.to_key()).second) {
                abertos.push(filhoFill);
            }
            GameState filhoEmpty;
            if (geraFilhoPL(EMPTY, estadoAtual, numJarro, filhoEmpty, states) && jaVisitados.insert(filhoEmpty.to_key()).second) {
                abertos.push(filhoEmpty);
            }
            GameState filhoTransferLeft;
            if (geraFilhoPL(TRANSFER_LEFT, estadoAtual, numJarro, filhoTransferLeft, states) && jaVisitados.insert(filhoTransferLeft.to_key()).second) {
                abertos.push(filhoTransferLeft);
            }
            GameState filhoTransferRight;
            if (geraFilhoPL(TRANSFER_RIGHT, estadoAtual, numJarro, filhoTransferRight, states) && jaVisitados.insert(filhoTransferRight.to_key()).second) {
                abertos.push(filhoTransferRight);
            }
        }
    }

    std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << states.size() << "\n";
}