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

bool checkParentLoopPL(const GameState& child, const std::vector<GameState>& states) {
    if (child.parent == -1) return false;

    std::string child_key = child.to_key();
    int ancestor_idx = child.parent;

    while (ancestor_idx != -1) {
        if (states[ancestor_idx].to_key() == child_key) {
            return true;
        }
        ancestor_idx = states[ancestor_idx].parent;
    }
    return false;
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
        newState.g_cost = state.g_cost + action_cost;
        newState.f_cost = newState.g_cost + newState.heuristic();

        for (int i = 0; i < newState.num_jars; ++i) {
            newState.values[i] = newState.jars[i].current_value;
        }

        newState.parent = state.index;
        newState.index = states.size();

        // 🧠 Evita ciclos na árvore (loop de volta para ancestral)
        if (checkParentLoopPL(newState, states)) {

            return false;
        }

        states.push_back(newState);
        return true;
    }
    return false;
}

void busca_profundidade_aux(GameState &state, int &profundidade, const int &profundidadeLimite,
                             bool &noEncontrado, std::vector<GameState>& states) {
    if (profundidade >= profundidadeLimite || noEncontrado)
        return;

    if (state.is_goal()) {
        noEncontrado = true;
        return;
    }

    profundidade++;
    state.visited = true;

    for (int numJarro = 0; numJarro < state.num_jars; ++numJarro) {
        GameState filhoFill;
        if (geraFilhoPL(FILL, state, numJarro, filhoFill, states)) {
            busca_profundidade_aux(filhoFill, profundidade, profundidadeLimite, noEncontrado, states);
            if (noEncontrado) return;
        }

        GameState filhoEmpty;
        if (geraFilhoPL(EMPTY, state, numJarro, filhoEmpty, states)) {
            busca_profundidade_aux(filhoEmpty, profundidade, profundidadeLimite, noEncontrado, states);
            if (noEncontrado) return;
        }

        GameState filhoTransferLeft;
        if (geraFilhoPL(TRANSFER_LEFT, state, numJarro, filhoTransferLeft, states)) {
            busca_profundidade_aux(filhoTransferLeft, profundidade, profundidadeLimite, noEncontrado, states);
            if (noEncontrado) return;
        }

        GameState filhoTransferRight;
        if (geraFilhoPL(TRANSFER_RIGHT, state, numJarro, filhoTransferRight, states)) {
            busca_profundidade_aux(filhoTransferRight, profundidade, profundidadeLimite, noEncontrado, states);
            if (noEncontrado) return;
        }
    }

    state.closed = true;
    profundidade--;
}

void SearchAlgorithms::busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    estadoInicial.g_cost = 0;
    states.push_back(estadoInicial);

    int profundidade = 0;
    bool noEncontrado = false;

    busca_profundidade_aux(estadoInicial, profundidade, profundidadeLimite, noEncontrado, states);

    if (!noEncontrado) {
        std::cout << "Nenhuma solução encontrada. Total de estados explorados: " << states.size() << "\n";
    }
}

void SearchAlgorithms::busca_largura(const std::vector<Jar> &initial_jars) {
    states.clear();
    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    estadoInicial.g_cost = 0;
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
