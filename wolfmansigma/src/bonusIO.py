import numpy as np
import time
import dask.array as da
import dask
from dask.delayed import delayed

# Function to read matrices from file
def read_matrix_from_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    matrices = []
    matrix = []
    for id, line in enumerate(lines):
        if id != 0:
            if line.strip():
                row = list(map(float, line.strip().split()))
                matrix.append(row)
            else:
                matrices.append(np.array(matrix))
                matrix = []
    if matrix:
        matrices.append(np.array(matrix))
    return matrices

# Function to multiply matrices (serial)
def multiply_matrices(A, B):
    rows_A, cols_A = A.shape
    rows_B, cols_B = B.shape
    if cols_A != rows_B:
        raise ValueError("Number of columns in A must be equal to number of rows in B")
    C = np.zeros((rows_A, cols_B))
    for i in range(rows_A):
        for j in range(cols_B):
            C[i, j] = sum(A[i, k] * B[k, j] for k in range(cols_A))
    return C

# Function to multiply matrices using Dask
def multiply_matrices_dask(A, B):
    dA = da.from_array(A, chunks=(A.shape[0], A.shape[1]))
    dB = da.from_array(B, chunks=(B.shape[0], B.shape[1]))
    dC = da.matmul(dA, dB)
    C = dC.compute()
    return C

# Delayed function to read matrices from file
@delayed
def delayed_read_matrix_from_file(file_path):
    return read_matrix_from_file(file_path)

# Main function
def main():
    file_path = '../tcmatmul/32.txt'
    
    # Parallel read and multiplication
    matrices = delayed_read_matrix_from_file(file_path)
    A, B = matrices.compute()
    
    # Start timing for serial multiplication
    start_serial = time.time()
    result_serial = multiply_matrices(A, B)
    end_serial = time.time()
    print("Serial multiplication result:")
    print(result_serial)
    print(result_serial.shape)
    print("Duration Serial: {:.3f} ms".format((end_serial - start_serial) * 1000))
    
    # Start timing for parallel multiplication
    start_parallel = time.time()
    result_parallel = multiply_matrices_dask(A, B)
    end_parallel = time.time()
    print("Parallel multiplication result:")
    print(result_parallel)
    print(result_parallel.shape)
    print("Duration Parallel: {:.3f} ms".format((end_parallel - start_parallel) * 1000))

if __name__ == "__main__":
    main()
