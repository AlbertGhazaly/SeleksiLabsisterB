import numpy as np
import time
import dask.array as da
from dask.distributed import Client

def read_matrix_from_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    matrices = []
    matrix = []
    for id, line in enumerate(lines):
        if (id != 0): 
            if line.strip():
                row = list(map(float, line.strip().split()))
                matrix.append(row)
            else:
                matrices.append(np.array(matrix))
                matrix = []
    
    if matrix:
        matrices.append(np.array(matrix))
    
    return matrices

def multiply_matrices_serial(A, B):
    rows_A, cols_A = len(A), len(A[0])
    rows_B, cols_B = len(B), len(B[0])
    
    if cols_A != rows_B:
        raise ValueError("Number of columns in A must be equal to number of rows in B")
    
    # Initialize result matrix C
    C = [[0 for _ in range(cols_B)] for _ in range(rows_A)]
    
    for i in range(rows_A):
        for j in range(cols_B):
            for k in range(cols_A):
                C[i][j] += A[i][k] * B[k][j]
    
    return C

def multiply_matrices_dask(A, B):
    # Convert NumPy arrays to Dask arrays
    dA = da.from_array(A, chunks=(A.shape[0], A.shape[1]))
    dB = da.from_array(B, chunks=(B.shape[0], B.shape[1]))
    
    # Perform matrix multiplication using Dask
    dC = da.matmul(dA, dB)
    
    # Compute the result
    C = dC.compute()
    
    return C

def main():
    # Connect to Dask Scheduler
    client = Client('tcp://192.168.0.1:8786')  # Ganti dengan alamat IP dan port scheduler Anda
    
    matrices = read_matrix_from_file('../tcmatmul/32.txt')
    
    if len(matrices) != 2:
        raise ValueError("File must contain exactly two matrices")
    
    A, B = matrices

    # Matriks perkalian versi serial
    start = time.time()
    result_serial = multiply_matrices_serial(A, B)
    print("Result of matrix multiplication (Serial):")
    # for row in result_serial:
    #     print(row)
    print("Shape:", (len(result_serial), len(result_serial[0])))
    end = time.time()
    print("Duration Serial: ", (end-start)*1000, "ms")

    # Matriks perkalian versi paralel
    start = time.time()
    result_parallel = multiply_matrices_dask(A, B)
    print("\nResult of matrix multiplication (Parallel):")
    # for row in result_parallel:
    #     print(row)
    print("Shape:", (len(result_parallel), len(result_parallel[0])))
    end = time.time()
    print("Duration Parallel: ", (end-start)*1000, "ms")
    
    # Close Dask Client
    client.close()

if __name__ == "__main__":
    main()
