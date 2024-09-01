#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <mpi.h>
#include <chrono>

using namespace std;

vector<vector<vector<double>>> read_matrices_from_file(const string& file_path) {
    ifstream file(file_path);
    vector<vector<vector<double>>> matrices;
    vector<vector<double>> current_matrix;
    vector<double> matrix_row;
    string line;

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

vector<vector<double>> multiply_matrices_mpi(const vector<vector<double>>& A_input, const vector<vector<double>>& B_input) {
    int num_procs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    size_t rowsA = A_input.size();
    size_t colsA = A_input[0].size();
    size_t rowsB = B_input.size();
    size_t colsB = B_input[0].size();

    size_t local_rows = rowsA / num_procs + (rank < (rowsA % num_procs) ? 1 : 0);
    vector<double> local_result(local_rows * colsB, 0.0);

    vector<double> A_flat(rowsA * colsA);
    vector<double> B_flat(rowsB * colsB);

    if (rank == 0) {
        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsA; ++j) {
                A_flat[i * colsA + j] = A_input[i][j];
            }
        }
        for (size_t i = 0; i < rowsB; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                B_flat[i * colsB + j] = B_input[i][j];
            }
        }
    }

    MPI_Bcast(&rowsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    MPI_Bcast(A_flat.data(), rowsA * colsA, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B_flat.data(), rowsB * colsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<vector<double>> A(rowsA, vector<double>(colsA));
    vector<vector<double>> B(rowsB, vector<double>(colsB));

    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsA; ++j) {
            A[i][j] = A_flat[i * colsA + j];
        }
    }
    for (size_t i = 0; i < rowsB; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            B[i][j] = B_flat[i * colsB + j];
        }
    }

    size_t start_row = rank * (rowsA / num_procs) + min(rank, static_cast<int>(rowsA % num_procs));
    size_t end_row = start_row + (rowsA / num_procs) + (rank < static_cast<int>(rowsA % num_procs) ? 1 : 0);

    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < colsA; ++k) {
                sum += A[i][k] * B[k][j];
            }
            local_result[(i - start_row) * colsB + j] = sum;
        }
    }

    vector<double> result(rowsA * colsB, 0.0);
    MPI_Gather(local_result.data(), local_result.size(), MPI_DOUBLE,
               result.data(), local_result.size(), MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        vector<vector<double>> final_result(rowsA, vector<double>(colsB, 0.0));
        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                final_result[i][j] = result[i * colsB + j];
            }
        }
        return final_result;
    } else {
        return vector<vector<double>>();  
    }
}

void print_matrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double value : row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int num_procs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (num_procs != 16) {
        if (rank == 0) {
            cerr << "This program must be run with 16 processes" << endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string file_path = "../tcmatmul/128.txt";

    vector<vector<vector<double>>> matrices;
    if (rank == 0) {
        matrices = read_matrices_from_file(file_path);
        if (matrices.size() != 2) {
            cerr << "File must contain exactly two matrices" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    size_t rowsA, colsA, rowsB, colsB;
    if (rank == 0) {
        rowsA = matrices[0].size();
        colsA = matrices[0][0].size();
        rowsB = matrices[1].size();
        colsB = matrices[1][0].size();
    }

    MPI_Bcast(&rowsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    vector<vector<double>> A(rowsA, vector<double>(colsA));
    vector<vector<double>> B(rowsB, vector<double>(colsB));

    if (rank == 0) {
        vector<double> A_flat(rowsA * colsA);
        vector<double> B_flat(rowsB * colsB);

        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsA; ++j) {
                A_flat[i * colsA + j] = matrices[0][i][j];
            }
        }
        for (size_t i = 0; i < rowsB; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                B_flat[i * colsB + j] = matrices[1][i][j];
            }
        }

        MPI_Bcast(A_flat.data(), rowsA * colsA, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(B_flat.data(), rowsB * colsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsA; ++j) {
                A[i][j] = A_flat[i * colsA + j];
            }
        }
        for (size_t i = 0; i < rowsB; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                B[i][j] = B_flat[i * colsB + j];
            }
        }
    } else {
        vector<double> A_flat(rowsA * colsA);
        vector<double> B_flat(rowsB * colsB);

        MPI_Bcast(A_flat.data(), rowsA * colsA, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(B_flat.data(), rowsB * colsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsA; ++j) {
                A[i][j] = A_flat[i * colsA + j];
            }
        }
        for (size_t i = 0; i < rowsB; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                B[i][j] = B_flat[i * colsB + j];
            }
        }
    }

    vector<vector<double>> result_serial;
    if (rank == 0) {
        auto start_time_serial = chrono::high_resolution_clock::now();
        result_serial = multiply_matrices_serial(A, B);
        auto end_time_serial = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration_serial = end_time_serial - start_time_serial;
        cout << "Hasil perkalian matriks (Serial):" << endl;
        // print_matrix(result_serial);
        cout<<"size: "<<result_serial.size()<<"x"<<result_serial[0].size()<<endl;
        cout << "Durasi perhitungan (Serial): " << duration_serial.count() << " ms" << endl;
    }

    auto start_time_parallel = chrono::high_resolution_clock::now();
    vector<vector<double>> result_parallel = multiply_matrices_mpi(A, B);
    auto end_time_parallel = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_parallel = end_time_parallel - start_time_parallel;

    if (rank == 0) {
        cout << "Hasil perkalian matriks (Parallel dengan MPI):" << endl;
        // print_matrix(result_parallel);
        cout<<"size: "<<result_parallel.size()<<"x"<<result_parallel[0].size()<<endl;
        cout << "Durasi perhitungan (Parallel dengan MPI): " << duration_parallel.count() << " ms" << endl;
    }

    MPI_Finalize();
    return 0;
}
