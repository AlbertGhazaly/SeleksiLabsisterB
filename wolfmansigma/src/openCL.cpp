#define CL_TARGET_OPENCL_VERSION 220
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <CL/cl.h>
#include <chrono>

using namespace std;

const char* kernelSource = R"(
__kernel void matrixMultiplyKernel(__global const double* A,
                                   __global const double* B,
                                   __global double* C,
                                   const int rowsA,
                                   const int colsA,
                                   const int colsB) {
    int row = get_global_id(1);
    int col = get_global_id(0);

    if (row < rowsA && col < colsB) {
        double value = 0;
        for (int k = 0; k < colsA; ++k) {
            value += A[row * colsA + k] * B[k * colsB + col];
        }
        C[row * colsB + col] = value;
    }
}
)";

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


vector<vector<double>> multiply_matrices_opencl(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int rowsA = A.size();
    int colsA = A[0].size();
    int rowsB = B.size();
    int colsB = B[0].size();

    if (colsA != rowsB) {
        cerr << "Matrix dimensions do not match for multiplication" << endl;
        exit(EXIT_FAILURE);
    }

    vector<double> flatA(rowsA * colsA);
    vector<double> flatB(rowsB * colsB);

    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsA; ++j) {
            flatA[i * colsA + j] = A[i][j];
        }
    }

    for (int i = 0; i < rowsB; ++i) {
        for (int j = 0; j < colsB; ++j) {
            flatB[i * colsB + j] = B[i][j];
        }
    }

    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        cerr << "Error getting platform ID: " << err << endl;
        exit(EXIT_FAILURE);
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        cerr << "Error getting device ID: " << err << endl;
        exit(EXIT_FAILURE);
    }

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        cerr << "Error creating context: " << err << endl;
        exit(EXIT_FAILURE);
    }

    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        cerr << "Error creating command queue: " << err << endl;
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, flatA.size() * sizeof(double), flatA.data(), &err);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, flatB.size() * sizeof(double), flatB.data(), &err);
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rowsA * colsB * sizeof(double), NULL, &err);
    if (err != CL_SUCCESS) {
        cerr << "Error creating buffer: " << err << endl;
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
    if (err != CL_SUCCESS) {
        cerr << "Error creating program: " << err << endl;
        clReleaseMemObject(d_A);
        clReleaseMemObject(d_B);
        clReleaseMemObject(d_C);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        cerr << "Error building program: " << err << endl;

        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), NULL);
        cerr << "Build Log:\n" << log.data() << endl;

        clReleaseProgram(program);
        clReleaseMemObject(d_A);
        clReleaseMemObject(d_B);
        clReleaseMemObject(d_C);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    cl_kernel kernel = clCreateKernel(program, "matrixMultiplyKernel", &err);
    if (err != CL_SUCCESS) {
        cerr << "Error creating kernel: " << err << endl;
        clReleaseProgram(program);
        clReleaseMemObject(d_A);
        clReleaseMemObject(d_B);
        clReleaseMemObject(d_C);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
    clSetKernelArg(kernel, 3, sizeof(int), &rowsA);
    clSetKernelArg(kernel, 4, sizeof(int), &colsA);
    clSetKernelArg(kernel, 5, sizeof(int), &colsB);

    size_t global_work_size[2] = { (size_t)colsB, (size_t)rowsA };
    size_t local_work_size[2] = { 16, 16 };

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        cerr << "Error enqueuing kernel: " << err << endl;
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseMemObject(d_A);
        clReleaseMemObject(d_B);
        clReleaseMemObject(d_C);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    vector<double> flatC(rowsA * colsB);
    err = clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, flatC.size() * sizeof(double), flatC.data(), 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        cerr << "Error reading buffer: " << err << endl;
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseMemObject(d_A);
        clReleaseMemObject(d_B);
        clReleaseMemObject(d_C);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        exit(EXIT_FAILURE);
    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    vector<vector<double>> C(rowsA, vector<double>(colsB));
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            C[i][j] = flatC[i * colsB + j];
        }
    }

    return C;
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
// Main function
int main() {
    // Read matrices from file
    string input_file = "../tcmatmul/128.txt";
    auto matrices = read_matrices_from_file(input_file);
    if (matrices.size() != 2) {
        cerr << "Error: Expected exactly two matrices." << endl;
        return EXIT_FAILURE;
    }

    auto A = matrices[0];
    auto B = matrices[1];

    // serial
    cout << "Result Matrix Parallel:" << endl;

    auto start = chrono::high_resolution_clock::now();

    vector<vector<double>> result_serial = multiply_matrices_serial(A, B);
    auto end = chrono::high_resolution_clock::now();
    // for (const auto& row : result_serial) {
    //     for (const auto& elem : row) {
    //         cout << elem << " ";
    //     }
    //     cout << endl;
    // }
    cout<<"size: "<<result_serial.size()<<"x"<<result_serial[0].size()<<endl;

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Serial execution time: " << duration << " ms" << endl;
    
    // Perform matrix multiplication using OpenCL
    start = chrono::high_resolution_clock::now();
    vector<vector<double>> result = multiply_matrices_opencl(A, B);
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    // Output the result
    cout << "Result Matrix Parallel:" << endl;
    // for (const auto& row : result) {
    //     for (const auto& elem : row) {
    //         cout << elem << " ";
    //     }
    //     cout << endl;
    // }
    cout<<"size: "<<result.size()<<"x"<<result[0].size()<<endl;
   

    cout << "Parallel execution time: " << duration << " ms" << endl;

    return 0;
}
       
