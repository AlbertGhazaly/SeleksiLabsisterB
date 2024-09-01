import time
from dask.distributed import Client, as_completed

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
                matrices.append(matrix)
                matrix = []
    
    if matrix:
        matrices.append(matrix)
    
    return matrices

def multiply_row(A, B, row):
    cols_B = len(B[0])
    result_row = [0] * cols_B
    for j in range(cols_B):
        for k in range(len(B)):
            result_row[j] += A[row][k] * B[k][j]
    return result_row

def multiply_matrices_dask(A, B):
    client = Client("tcp://192.168.68.55:8786")  # Ganti <SCHEDULER_IP> dengan alamat IP scheduler

    futures = client.map(multiply_row, [A] * len(A), [B] * len(A), range(len(A)))

    result = []
    for future in as_completed(futures):
        result.append(future.result())

    return result

def main():
    matrices = read_matrix_from_file('../tcmatmul/128.txt')
    
    if len(matrices) != 2:
        raise ValueError("File must contain exactly two matrices")
    
    A, B = matrices

    start = time.time()
    result_dask = multiply_matrices_dask(A, B)
    print("\nResult of matrix multiplication (Dask):")
    print("Shape:", (len(result_dask), len(result_dask[0])))
    end = time.time()
    print("Duration Dask: ", (end-start)*1000, "ms")

if __name__ == "__main__":
    main()
