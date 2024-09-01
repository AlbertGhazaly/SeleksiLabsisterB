#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void swap_rows(vector<vector<double>> &matrix, int row1, int row2, int n) {
    for (int i = 0; i < n; i++) {
        double temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

void gauss_jordan(vector<vector<double>> &matrix, vector<vector<double>> &inverse, int n) {
    for (int i = 0; i < n; i++) {
        // Cari elemen utama
        if (matrix[i][i] == 0) {
            for (int j = i + 1; j < n; j++) {
                if (matrix[j][i] != 0) {
                    swap_rows(matrix, i, j, n);
                    swap_rows(inverse, i, j, n);
                    break;
                }
            }
        }

        // Normalisasi baris utama
        double pivot = matrix[i][i];
        for (int j = 0; j < n; j++) {
            matrix[i][j] /= pivot;
            inverse[i][j] /= pivot;
        }

        // Eliminasi baris lain
        for (int j = 0; j < n; j++) {
            if (j != i) {
                double factor = matrix[j][i];
                for (int k = 0; k < n; k++) {
                    matrix[j][k] -= factor * matrix[i][k];
                    inverse[j][k] -= factor * inverse[i][k];
                }
            }
        }
    }
}

int main() {
    ifstream inputFile("input/input.txt");
    ofstream outputFile("output/cpp.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cout << "Error membuka file!" << endl;
        return 1;
    }

    int n;
    inputFile >> n;

    // Alokasi memori untuk matriks dan matriks invers
    vector<vector<double>> matrix(n, vector<double>(n));
    vector<vector<double>> inverse(n, vector<double>(n));

    // Baca matriks dari file input dan inisialisasi matriks invers sebagai matriks identitas
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inputFile >> matrix[i][j];
            inverse[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    // Hitung invers matriks menggunakan metode Gauss-Jordan
    gauss_jordan(matrix, inverse, n);

    // Tulis hasil invers ke file output
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            outputFile << inverse[i][j] << " ";
        }
        outputFile << endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
