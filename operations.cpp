#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <iostream>
#include <vector>

using namespace std;

class Operations {
private:
    vector<vector<int>> matrix; // Matriz de estados
    int rows; // N-1 linhas
    int cols; // N colunas

public:
    // Construtor: recebe número de jarros (N) e inicializa matriz (N-1) x N
    Operations(int num_jars);
    
    // Verifica se todos os elementos da matriz são 0
    bool all_zero() const;
    
    // Seta a diagonal principal para 0
    void set_diagonal_zero();
    
    // Esvaziar: se diagonal tem 3, seta para 2
    void empty_diagonal();
    
    // Encher: se diagonal tem 3, seta para 1
    void fill_diagonal();
    
    // Acessa elemento na posição (i,j)
    int get_value(int i, int j) const;
    
    // Define elemento na posição (i,j)
    void set_value(int i, int j, int value);
    
    // Exibe a matriz
    void print() const;
    
    // Retorna dimensões
    int get_rows() const;
    int get_cols() const;
};

// Definições dos métodos fora da classe

Operations::Operations(int num_jars) : cols(num_jars), rows(num_jars - 1) {
    if (num_jars <= 1) {
        rows = 0;
        cols = 0;
        return;
    }
    matrix.resize(rows, vector<int>(cols, 1)); // Inicializa com 1
    // Inicializa diagonal principal com 3
    for (int i = 0; i < min(rows, cols); ++i) {
        matrix[i][i] = 3;
    }
}

bool Operations::all_zero() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

void Operations::set_diagonal_zero() {
    for (int i = 0; i < min(rows, cols); ++i) {
        matrix[i][i] = 0;
    }
}

void Operations::empty_diagonal() {
    for (int i = 0; i < min(rows, cols); ++i) {
        if (matrix[i][i] == 3) {
            matrix[i][i] = 2;
        }
        // Se for 1, seta para 0
        else if (matrix[i][i] == 1) {
            matrix[i][i] = 0;
        }
    }
}

void Operations::fill_diagonal() {
    for (int i = 0; i < min(rows, cols); ++i) {
        if (matrix[i][i] == 3) {
            matrix[i][i] = 1;
        }
        // Se for 1, seta para 0
        else if (matrix[i][i] == 1) {
            matrix[i][i] = 0;
        }
    }
}

int Operations::get_value(int i, int j) const {
    if (i >= 0 && i < rows && j >= 0 && j < cols) {
        return matrix[i][j];
    }
    return -1; // Valor inválido para índices fora do intervalo
}

void Operations::set_value(int i, int j, int value) {
    if (i >= 0 && i < rows && j >= 0 && j < cols) {
        matrix[i][j] = value;
    }
}

void Operations::print() const {
    if (rows == 0 || cols == 0) {
        cout << "Matriz vazia" << endl;
        return;
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
}

int Operations::get_rows() const {
    return rows;
}

int Operations::get_cols() const {
    return cols;
}

#endif // Operations_H