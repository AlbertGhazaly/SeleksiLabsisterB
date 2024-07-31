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
    start = time.time()
    result = multiply_matrices(A, B)
    print("Result of matrix multiplication:")
    print(result)
    print(result.shape)
    end = time.time()
    print("Duration Serial: ", (end-start)*1000, "ms")

    start = time.time()
    result = multiply_matrices_dask(A,B)
    print(result)
    print(result.shape)
    end = time.time()
    print("Duration Parallel: ", (end-start)*1000, "ms")
    
    # Close Dask Client
    client.close()

if __name__ == "__main__":
    main()
