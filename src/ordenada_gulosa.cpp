#include "ordenada_gulosa.hpp"
#include "medidasBuscas.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <deque>
#include <ctime>
#include <set>



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

int retornaheuristic()(GameState &state)
{
    return state.custoCaminho;
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
            newState.heuristic() = retornaheuristic()(newState);
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
            newState.heuristic() = retornaheuristic()(newState);
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
            newState.heuristic() = retornaheuristic()(newState);
            vetorEstados.push_back(newState);

            return true;
        }
    default:
        cout << "Ação desconhecida" << endl;
    }
}

bool comparaPorCusto(const GameState &stateA, const GameState &stateB)
{
    return stateA.custoCaminho > stateB.custoCaminho;
}

bool comparaPorheuristic()(const GameState &stateA, const GameState &stateB)
{
    return stateA.heuristic() < stateB.heuristic();
}

void OrdenadaGulosa::busca_ordenada(const std::vector<Jar> &initial_jars)
{
    // cria gameState inicial como noAtual

    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    GameState estadoAtual = estadoInicial;

    // cria vetor de estados

    vector<GameState> vetorEstados;
    vetorEstados.push_back(estadoInicial);

    // cria set de antecessores

    std::set<string> jaVisitados;

    // cria profundidade e medidas

    MedidasBusca medidas;
    int profundidade = 0;
    std::clock_t c_start = std::clock();

    // cria deque de abertos e coloca o gamestate inicial

    deque<GameState> abertos;
    abertos.push_front(estadoInicial);

    // while !listaAbertos.empty()
    while (!abertos.empty())
    {
        // noAtual = primeiro da lista (tirar ele da lista)
        estadoAtual = abertos.front();
        abertos.pop_front();
        medidas.nosVisitados++;

        //  imprime nó
        estadoAtual.print();

        //  verifica vitória (se sim, imprimir)
        if (estadoAtual.is_goal())
        {
            std::cout << endl
                      << "Nó encontrado!" << endl;

            // medidas depois

            std::clock_t c_end = std::clock();
            long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;

            medidas.profundidadeMax = estadoAtual.print_path(estadoAtual, estadoAtual.index, vetorEstados);
            medidas.custoCaminho = estadoAtual.custoCaminho;
            medidas.tempoExecucao = time_ms;
            medidas.imprimirDados();
            return;
        }

        // cria vetor provisório pros filhos

        vector<GameState> filhosNovosOrdenados;

        // for jarros
        for (int numJarro = 0; numJarro < estadoAtual.jars.size(); numJarro++)
        {
            //  cria nós possíveis e vai adicionando ao final da lista (se o filho existir)
            // fill
            GameState filhoFill;
            if (geraFilho(FILL, estadoAtual, numJarro, numJarro, filhoFill, vetorEstados) && jaVisitados.insert(filhoFill.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoFill.to_key() << endl;
                //  abertos.push(filhoFill);
                filhosNovosOrdenados.push_back(filhoFill);
            }

            // empty

            GameState filhoEmpty;
            if (geraFilho(EMPTY, estadoAtual, numJarro, numJarro, filhoEmpty, vetorEstados) && jaVisitados.insert(filhoEmpty.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoEmpty.to_key() << endl;
                //  abertos.push(filhoEmpty);
                filhosNovosOrdenados.push_back(filhoEmpty);
            }

            // transfer left

            GameState filhoTransferLeft;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro - 1, filhoTransferLeft, vetorEstados) && jaVisitados.insert(filhoTransferLeft.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoTransferLeft.to_key() << endl;
                //  abertos.push(filhoTransferLeft);
                filhosNovosOrdenados.push_back(filhoTransferLeft);
            }

            // transfer right

            GameState filhoTransferRight;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro + 1, filhoTransferRight, vetorEstados) && jaVisitados.insert(filhoTransferRight.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoTransferRight.to_key() << endl;
                //  abertos.push(filhoTransferRight);
                filhosNovosOrdenados.push_back(filhoTransferRight);
            }
        }

        // ordenar filhos

        std::sort(filhosNovosOrdenados.begin(), filhosNovosOrdenados.end(), comparaPorCusto);

        // adiciona ao topo da pilha de forma que o menor filho fique no topo

        for (int i = 0; i < filhosNovosOrdenados.size(); i++)
        {
            abertos.push_front(filhosNovosOrdenados[i]);
        }
    }

    return;
}

void OrdenadaGulosa::busca_gulosa(const std::vector<Jar> &initial_jars)
{
    // cria gameState inicial como noAtual

    GameState estadoInicial(initial_jars, -1);
    estadoInicial.index = 0;
    GameState estadoAtual = estadoInicial;

    // cria vetor de estados

    vector<GameState> vetorEstados;
    vetorEstados.push_back(estadoInicial);

    // cria set de antecessores

    std::set<string> jaVisitados;

    // cria profundidade e medidas

    MedidasBusca medidas;
    int profundidade = 0;
    std::clock_t c_start = std::clock();

    // cria deque de abertos e coloca o gamestate inicial

    deque<GameState> abertos;
    abertos.push_front(estadoInicial);

    // while !listaAbertos.empty()
    while (!abertos.empty())
    {
        // noAtual = primeiro da lista (tirar ele da lista)
        estadoAtual = abertos.front();
        abertos.pop_front();
        medidas.nosVisitados++;

        //  imprime nó
        estadoAtual.print();

        //  verifica vitória (se sim, imprimir)
        if (estadoAtual.is_goal())
        {
            std::cout << endl
                      << "Nó encontrado!" << endl;

            // medidas depois

            std::clock_t c_end = std::clock();
            long time_ms = 1000 * (c_end - c_start) / CLOCKS_PER_SEC;

            medidas.profundidadeMax = estadoAtual.print_path(estadoAtual, estadoAtual.index, vetorEstados);
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
            if (geraFilho(FILL, estadoAtual, numJarro, numJarro, filhoFill, vetorEstados) && jaVisitados.insert(filhoFill.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoFill.to_key() << endl;
                //  abertos.push(filhoFill);
                abertos.push_back(filhoFill);
            }

            // empty

            GameState filhoEmpty;
            if (geraFilho(EMPTY, estadoAtual, numJarro, numJarro, filhoEmpty, vetorEstados) && jaVisitados.insert(filhoEmpty.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoEmpty.to_key() << endl;
                //  abertos.push(filhoEmpty);
                abertos.push_back(filhoEmpty);
            }

            // transfer left

            GameState filhoTransferLeft;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro - 1, filhoTransferLeft, vetorEstados) && jaVisitados.insert(filhoTransferLeft.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoTransferLeft.to_key() << endl;
                //  abertos.push(filhoTransferLeft);
                abertos.push_back(filhoTransferLeft);
            }

            // transfer right

            GameState filhoTransferRight;
            if (geraFilho(TRANSFER, estadoAtual, numJarro, numJarro + 1, filhoTransferRight, vetorEstados) && jaVisitados.insert(filhoTransferRight.to_key()).second)
            {
                medidas.nosExpandidos++;
                // std::cout << "Tentando inserir key = \"" << filhoTransferRight.to_key() << endl;
                //  abertos.push(filhoTransferRight);
                abertos.push_back(filhoTransferRight);
            }
        }

        // ordenar filhos

        std::sort(abertos.begin(), abertos.end(), comparaPorheuristic());
    }

    return;
}