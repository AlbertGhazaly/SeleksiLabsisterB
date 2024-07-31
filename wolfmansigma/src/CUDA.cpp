#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;

// CUDA kernel function for matrix multiplication
__global__ void matrixMultiplyKernel(const double* A, const double* B, double* C, int rowsA, int colsA, int colsB) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rowsA && col < colsB) {
        double value = 0;
        for (int k = 0; k < colsA; ++k) {
            value += A[row * colsA + k] * B[k * colsB + col];
        }
        C[row * colsB + col] = value;
    }
}

// Function to read matrices from a file
vector<vector<double>> read_matrix(const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        exit(EXIT_FAILURE);
    }

    int num_matrices;
    file >> num_matrices; // Skip the first line

    vector<vector<double>> matrices;
    for (int m = 0; m < num_matrices; ++m) {
        int rows, cols;
        file >> rows >> cols;
        vector<double> matrix(rows * cols);
        for (int i = 0; i < rows * cols; ++i) {
            file >> matrix[i];
        }
        matrices.push_back(matrix);
    }
    file.close();
    return matrices;
}

// Function for serial matrix multiplication
vector<vector<double>> multiply_matrices_serial(const vector<double>& A, int rowsA, int colsA, const vector<double>& B, int rowsB, int colsB) {
    if (colsA != rowsB) {
        cerr << "Matrix dimensions do not match for multiplication" << endl;
        exit(EXIT_FAILURE);
    }

    vector<vector<double>> C(rowsA, vector<double>(colsB, 0));
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            for (int k = 0; k < colsA; ++k) {
                C[i][j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
    return C;
}

// Function for parallel matrix multiplication using CUDA
vector<vector<double>> multiply_matrices_cuda(const vector<double>& A, int rowsA, int colsA, const vector<double>& B, int rowsB, int colsB) {
    if (colsA != rowsB) {
        cerr << "Matrix dimensions do not match for multiplication" << endl;
        exit(EXIT_FAILURE);
    }

    size_t sizeA = rowsA * colsA * sizeof(double);
    size_t sizeB = rowsB * colsB * sizeof(double);
    size_t sizeC = rowsA * colsB * sizeof(double);

    double* d_A, * d_B, * d_C;
    cudaMalloc(&d_A, sizeA);
    cudaMalloc(&d_B, sizeB);
    cudaMalloc(&d_C, sizeC);

    cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks((colsB + threadsPerBlock.x - 1) / threadsPerBlock.x,
                   (rowsA + threadsPerBlock.y - 1) / threadsPerBlock.y);

    matrixMultiplyKernel<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C, rowsA, colsA, colsB);

    cudaDeviceSynchronize(); // Ensure the kernel finishes before accessing results

    vector<double> C(rowsA * colsB);
    cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    vector<vector<double>> result(rowsA, vector<double>(colsB));
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            result[i][j] = C[i * colsB + j];
        }
    }

    return result;
}

int main() {
    string filename = "../tcmatmul/32.txt";
    vector<vector<double>> matrices = read_matrix(filename);

    // Assuming matrices are in correct format
    int rowsA = matrices[0].size() / 2;
    int colsA = matrices[0].size() / 2;
    int rowsB = matrices[1].size() / 2;
    int colsB = matrices[1].size() / 2;

    auto start = chrono::high_resolution_clock::now();
    vector<vector<double>> result_serial = multiply_matrices_serial(matrices[0], rowsA, colsA, matrices[1], rowsB, colsB);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_serial = end - start;
    cout << "Durasi perhitungan (Serial): " << duration_serial.count() << " ms" << endl;

    start = chrono::high_resolution_clock::now();
    vector<vector<double>> result_parallel = multiply_matrices_cuda(matrices[0], rowsA, colsA, matrices[1], rowsB, colsB);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_parallel = end - start;
    cout << "Durasi perhitungan (Parallel dengan CUDA): " << duration_parallel.count() << " ms" << endl;

    return 0;
}
