#include "structure.hpp"
#include <algorithm>

Jar::Jar(int jar_id, int capacity, int curr_value = 0)
    : id(jar_id), current_value(curr_value), max_capacity(capacity) {}

void Jar::fill()
{
    current_value = max_capacity;
}

void Jar::empty()
{
    current_value = 0;
}

bool Jar::is_full() const
{
    return current_value == max_capacity;
}

bool Jar::is_empty() const
{
    if (this == nullptr)
    {
        std::cerr << "Error: Null Jar pointer" << std::endl;
        return false;
    }
    return current_value == 0;
}

int Jar::get_capacity() const
{
    return max_capacity;
}

int Jar::space_left() const
{
    return max_capacity - current_value;
}

int Jar::transfer(int amount)
{
    int accepted = min(amount, space_left());
    current_value += accepted;
    return amount - accepted;
}

GameState::GameState(const vector<Jar> &j, int p) : jars(j), parent(p), closed(false)
{
    for (const Jar &jar : j)
    {
        values.push_back(jar.current_value);
    }
}

string GameState::to_key() const
{
    string key;
    for (int val : values)
    {
        key += to_string(val) + "|";
    }
    return key;
}

void GameState::transfer_from_jars(Jar &jarOrigin, Jar &jarDestination)
{
    // Quanto espaço ainda cabe no destino?
    int availableSpace = jarDestination.get_capacity() - jarDestination.current_value;

    // Só transfere o mínimo entre o que tem na origem e o espaço livre
    int transferAmount = std::min(jarOrigin.current_value, availableSpace);

    // Subtrai da origem e soma ao destino
    jarOrigin.current_value -= transferAmount;
    jarDestination.current_value += transferAmount;
}

void GameState::print() const
{
    std::cout << "(";
    for (const Jar &jar : jars)
    {
        std::cout << jar.current_value << "/" << jar.max_capacity << " ";
    }
    std::cout << ") Parent: " << parent << ", Index: " << index << ", Closed: " << (closed ? "true" : "false") << "\n";
}

int GameState::imprimeCaminho(const GameState &noFinal, int indiceNoFinal, std::vector<GameState> &estados)
{
    std::cout << endl
              << "Caminho até o nó final: " << endl;
    int noAtualIndice = noFinal.parent;
    vector<int> caminho;
    caminho.push_back(indiceNoFinal);
    while (noAtualIndice != -1)
    {
        caminho.push_back(noAtualIndice);
        noAtualIndice = estados[noAtualIndice].parent;
    }

    for (int i = caminho.size() - 1; i >= 0; i--)
    {
        estados[caminho[i]].print();
    }

    return caminho.size();
}