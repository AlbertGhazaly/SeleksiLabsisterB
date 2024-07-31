#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <chrono>

using namespace std;

// Fungsi untuk membaca matriks dari file
vector<vector<vector<double>>> read_matrices_from_file(const string& file_path) {
    ifstream file(file_path);
    vector<vector<vector<double>>> matrices;
    vector<vector<double>> current_matrix;
    vector<double> matrix_row;
    string line;

    // Abaikan baris pertama
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) {
            if (!current_matrix.empty()) {
                matrices.push_back(current_matrix);
                current_matrix.clear();
            }
        } else {
            stringstream ss(line);
            double value;
            matrix_row.clear();
            while (ss >> value) {
                matrix_row.push_back(value);
            }
            current_matrix.push_back(matrix_row);
        }
    }

    if (!current_matrix.empty()) {
        matrices.push_back(current_matrix);
    }

    return matrices;
}

// Fungsi untuk melakukan perkalian matriks secara serial
vector<vector<double>> multiply_matrices_serial(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t colsB = B[0].size();

    vector<vector<double>> result(rowsA, vector<double>(colsB, 0.0));

    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            for (size_t k = 0; k < colsA; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

// Fungsi untuk melakukan perkalian matriks secara paralel dengan OpenMP
vector<vector<double>> multiply_matrices_parallel(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t colsB = B[0].size();

    vector<vector<double>> result(rowsA, vector<double>(colsB, 0.0));

    #pragma omp parallel for collapse(2)
    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < colsA; ++k) {
                sum += A[i][k] * B[k][j];
            }
            result[i][j] = sum;
        }
    }

    return result;
}

// Fungsi untuk mencetak matriks
void print_matrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double value : row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

int main() {
    // Path ke file matriks
    string file_path = "../tcmatmul/32.txt";

    // Membaca matriks dari file
    vector<vector<vector<double>>> matrices = read_matrices_from_file(file_path);

    if (matrices.size() != 2) {
        cerr << "File harus berisi tepat dua matriks" << endl;
        return 1;
    }

    vector<vector<double>> A = matrices[0];
    vector<vector<double>> B = matrices[1];

    cout << "Matriks Pertama:" << endl;
    print_matrix(A);
    cout << "Matriks Kedua:" << endl;
    print_matrix(B);

    // Mengukur waktu eksekusi untuk versi serial
    auto start_time_serial = chrono::high_resolution_clock::now();
    vector<vector<double>> result_serial = multiply_matrices_serial(A, B);
    auto end_time_serial = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_serial = end_time_serial - start_time_serial;

    cout << "Hasil perkalian matriks (Serial):" << endl;
    print_matrix(result_serial);
    cout << "Durasi perhitungan (Serial): " << duration_serial.count() << " ms" << endl;

    // Mengukur waktu eksekusi untuk versi paralel
    auto start_time_parallel = chrono::high_resolution_clock::now();
    vector<vector<double>> result_parallel = multiply_matrices_parallel(A, B);
    auto end_time_parallel = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_parallel = end_time_parallel - start_time_parallel;

    cout << "Hasil perkalian matriks (Parallel):" << endl;
    print_matrix(result_parallel);
    cout << "Durasi perhitungan (Parallel): " << duration_parallel.count() << " ms" << endl;

    return 0;
}
