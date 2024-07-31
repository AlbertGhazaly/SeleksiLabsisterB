#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;
// Function to read matrices from file
vector<vector<vector<double>>> read_matrices_from_file(const string& file_path) {
    ifstream file(file_path);
    vector<vector<vector<double>>> matrices;
    vector<vector<double>> current_matrix;
    vector<double> matrix_row;
    string line;

    // Skip the first line
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

// Serial matrix multiplication
vector<vector<double>> multiply_matrices_serial(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    vector<vector<double>> result(n, vector<double>(m, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            for (int k = 0; k < p; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

// Parallel matrix multiplication using SIMD
vector<vector<double>> multiply_matrices_parallel(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    vector<vector<double>> result(n, vector<double>(m, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; j += 4) {
            __m256d c0 = _mm256_setzero_pd();

            for (int k = 0; k < p; ++k) {
                __m256d a0 = _mm256_broadcast_sd(&A[i][k]);
                __m256d b0 = _mm256_loadu_pd(&B[k][j]);
                c0 = _mm256_fmadd_pd(a0, b0, c0);
            }

            _mm256_storeu_pd(&result[i][j], c0);
        }
    }

    return result;
}

void print_matrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

int main() {
    string file_path = "../tcmatmul/32.txt";  // Replace with your file path
    vector<vector<vector<double>>> matrices = read_matrices_from_file(file_path);

    if (matrices.size() < 2) {
        cerr << "File should contain at least two matrices." << endl;
        return -1;
    }

    auto& A = matrices[0];
    auto& B = matrices[1];

    auto start_serial = high_resolution_clock::now();
    auto result_serial = multiply_matrices_serial(A, B);
    auto end_serial = high_resolution_clock::now();

    auto start_parallel = high_resolution_clock::now();
    auto result_parallel = multiply_matrices_parallel(A, B);
    auto end_parallel = high_resolution_clock::now();

    auto duration_serial = duration_cast<microseconds>(end_serial - start_serial).count() / 1000.0;
    auto duration_parallel = duration_cast<microseconds>(end_parallel - start_parallel).count() / 1000.0;

    cout << "Serial Result:" << endl;
    print_matrix(result_serial);
    cout << "Duration Serial: " << duration_serial << " ms\n" << endl;

    cout << "Parallel Result:" << endl;
    print_matrix(result_parallel);
    cout << "Duration Parallel: " << duration_parallel << " ms" << endl<<setprecision(3);

    return 0;
}
