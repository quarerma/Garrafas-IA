#include "profundidade_largura.hpp"
#include "medidasBuscas.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>

bool is_goal_state(const GameState &state)
{
    if (state.jars.empty())
        return false;
    const Jar &last = state.jars.back();
    return last.current_value == (last.max_capacity / 3);
}

enum Acao
{
    FILL,
    EMPTY,
    TRANSFER
};

void copiaEstado(GameState &origem, GameState &destino)
{
    for (int i = 0; i < origem.jars.size(); i++)
    {
        destino.jars.push_back(origem.jars[i]);
    }
    destino.custoCaminho = origem.custoCaminho;
}

bool geraFilho(Acao acao, GameState &state, int indiceJarraOrigem, int indiceJarraDestino, GameState &newState, vector<GameState> &vetorEstados)
{
    // verifica primeiro se os arrays das jarras são válidos e castar erro se não, já ajuda o transfer

    if (indiceJarraOrigem < 0 || indiceJarraOrigem >= state.jars.size() || indiceJarraDestino < 0 || indiceJarraDestino >= state.jars.size())
    {
        // std::cout << "ERRO: Índice fora dos limites ( " << indiceJarraOrigem << ", " << indiceJarraDestino << ")" << endl;
        return false;
    }

    // SWITCH CASE

    switch (acao)
    {
    case FILL:
        // ação fill
        // não pode encher se já estiver cheio

        if (state.jars[indiceJarraOrigem].is_full())
        {
            // std::cout << "Já cheio" << endl;
            return false;
        }
        else
        {
            copiaEstado(state, newState);
            newState.jars[indiceJarraOrigem].fill();

            newState.custoCaminho += state.jars[indiceJarraOrigem].space_left();
            newState.index = vetorEstados.size();
            newState.parent = state.index;
            vetorEstados.push_back(newState);

            return true;
        }
    case EMPTY:
        // não pode esvaziar se já estiver vazio

        if (state.jars[indiceJarraOrigem].is_empty())
        {
            // std::cout << "Já vazio" << endl;
            return false;
        }
        else
        {
            copiaEstado(state, newState);
            newState.jars[indiceJarraOrigem].empty();

            newState.custoCaminho += state.jars[indiceJarraOrigem].current_value;
            newState.index = vetorEstados.size();
            newState.parent = state.index;
            vetorEstados.push_back(newState);

            return true;
        }
    case TRANSFER:
        // nao pode transfer pra ele mesmo
        // não pode se estiver vazio
        // não pode se o outro estiver cheio

        if (indiceJarraOrigem == indiceJarraDestino || state.jars[indiceJarraOrigem].is_empty() || state.jars[indiceJarraDestino].is_full())
        {
            // std::cout << "Vazio, cheio ou na borda" << endl;
            return false;
        }
        else
        {
            copiaEstado(state, newState);
            newState.transfer_from_jars(newState.jars[indiceJarraOrigem], newState.jars[indiceJarraDestino]);

            newState.custoCaminho += state.get_transfer_value_from_jars(state.jars[indiceJarraOrigem], state.jars[indiceJarraDestino]);
            newState.index = vetorEstados.size();
            newState.parent = state.index;
            vetorEstados.push_back(newState);

            return true;
        }
    default:
        cout << "Ação desconhecida" << endl;
    }
}

void busca_profundidade_aux(GameState &state, int &profundidade, const int &profundidadeLimite, bool &noEncontrado, MedidasBusca &medidas, vector<GameState> &vetorEstados)
{
    medidas.nosVisitados++;
    // verifica se é vitória || profundidade limite
    if (profundidade >= profundidadeLimite || noEncontrado)
    {
        return;
    }
    if (is_goal_state(state))
    {
        std::cout << endl
                  << "Estado encontrado!" << endl;

        noEncontrado = true;

        medidas.profundidadeMax = state.imprimeCaminho(state, state.index, vetorEstados);
        medidas.custoCaminho = state.custoCaminho;
        return;
    }
    // profundidade++

    profundidade++;

    // for todas as jarras
    for (int numJarras = 0; numJarras < state.jars.size(); numJarras++)
    {
        // fill
        GameState filhoFill;
        if (geraFilho(FILL, state, numJarras, numJarras, filhoFill, vetorEstados) && !noEncontrado)
        {
            medidas.nosExpandidos++;
            filhoFill.print();
            busca_profundidade_aux(filhoFill, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados);
        }

        // empty

        GameState filhoEmpty;
        if (geraFilho(EMPTY, state, numJarras, numJarras, filhoEmpty, vetorEstados) && !noEncontrado)
        {
            medidas.nosExpandidos++;
            filhoEmpty.print();
            busca_profundidade_aux(filhoEmpty, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados);
        }

        // transfer left

        GameState filhoTransferLeft;
        if (geraFilho(TRANSFER, state, numJarras, numJarras - 1, filhoTransferLeft, vetorEstados) && !noEncontrado)
        {
            medidas.nosExpandidos++;
            filhoTransferLeft.print();
            busca_profundidade_aux(filhoTransferLeft, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados);
        }

        // transfer right

        GameState filhoTransferRight;
        if (geraFilho(TRANSFER, state, numJarras, numJarras + 1, filhoTransferRight, vetorEstados) && !noEncontrado)
        {
            medidas.nosExpandidos++;
            filhoTransferRight.print();
            busca_profundidade_aux(filhoTransferRight, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados);
        }
    }

    // profundidade--

    profundidade--;

    return;
}

void ProfundidadeLargura::busca_profundidade(const std::vector<Jar> &initial_jars, const int &profundidadeLimite)
{
    // cria gameState inicial

    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    vector<GameState> vetorEstados;
    vetorEstados.push_back(estadoInicial);

    // cria classe medidas e inicia

    MedidasBusca medidas;
    int profundidade = 0;
    std::clock_t c_start = std::clock();

    // cria flag

    bool noEncontrado = false;

    // chama busca_profundidade_aux

    busca_profundidade_aux(estadoInicial, profundidade, profundidadeLimite, noEncontrado, medidas, vetorEstados);

    // medidas no final e impressão

    std::clock_t c_end = std::clock();
    long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;

    medidas.tempoExecucao = time_ms;
    medidas.imprimirDados();

    return;
}

void ProfundidadeLargura::busca_largura(const std::vector<Jar> &initial_jars)
{
    // cria gameState inicial como noAtual

    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    GameState estadoAtual = estadoInicial;

    // cria vetor de estados

    vector<GameState> vetorEstados;
    vetorEstados.push_back(estadoInicial);

    // cria profundidade e medidas

    MedidasBusca medidas;
    int profundidade = 0;
    std::clock_t c_start = std::clock();

    // cria queue de abertos e coloca o gamestate inicial

    queue<GameState> abertos;
    abertos.push(estadoInicial);

    // while !listaAbertos.empty()
    while (!abertos.empty())
    {
        // noAtual = primeiro da lista (tirar ele da lista)
        estadoAtual = abertos.front();
        abertos.pop();
        medidas.nosVisitados++;

        //  imprime nó
        estadoAtual.print();

        //  verifica vitória (se sim, imprimir)
        if (is_goal_state(estadoAtual))
        {
            std::cout << endl
                      << "Nó encontrado!" << endl;

            // medidas depois

            std::clock_t c_end = std::clock();
            long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;

            medidas.profundidadeMax = estadoAtual.imprimeCaminho(estadoAtual, estadoAtual.index, vetorEstados);
            medidas.custoCaminho = estadoAtual.custoCaminho;
            medidas.tempoExecucao = time_ms;
            medidas.imprimirDados();
            return;
        }

        // for jarros
        for (int numJarro = 0; numJarro < estadoAtual.jars.size(); numJarro++)
        {
            //  cria nós possíveis e vai adicionando ao final da lista (se o filho existir)
            // fill
            GameState filhoFill;
            if (geraFilho(FILL, estadoAtual, numJarro, numJarro, filhoFill, vetorEstados))
            {
                medidas.nosExpandidos++;
                abertos.push(filhoFill);
            }

            // empty

            GameState filhoEmpty;
            if (geraFilho(EMPTY, estadoAtual, numJarro, numJarro, filhoEmpty, vetorEstados))
            {
                medidas.nosExpandidos++;
                abertos.push(filhoEmpty);
            }

            // transfer left

            GameState filhoTransferLeft;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro - 1, filhoTransferLeft, vetorEstados))
            {
                medidas.nosExpandidos++;
                abertos.push(filhoTransferLeft);
            }

            // transfer right

            GameState filhoTransferRight;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro + 1, filhoTransferRight, vetorEstados))
            {
                medidas.nosExpandidos++;
                abertos.push(filhoTransferRight);
            }
        }
    }

    return;
}
