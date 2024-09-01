import time
from multiprocessing import Pool

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

def multiply_row(args):
    A, B, row = args
    cols_B = len(B[0])
    result_row = [0] * cols_B
    for j in range(cols_B):
        for k in range(len(B)):
            result_row[j] += A[row][k] * B[k][j]
    return result_row

def multiply_matrices_parallel(A, B):
    with Pool() as pool:
        result = pool.map(multiply_row, [(A, B, i) for i in range(len(A))])
    return result

def main():
    matrices = read_matrix_from_file('../tcmatmul/128.txt')
    
    if len(matrices) != 2:
        raise ValueError("File must contain exactly two matrices")
    
    A, B = matrices

    start = time.time()
    result_serial = multiply_matrices_serial(A, B)
    print("Result of matrix multiplication (Serial):")
    # for row in result_serial:
    #     print(row)
    print("Shape:", (len(result_serial), len(result_serial[0])))
    end = time.time()
    print("Duration Serial: ", (end-start)*1000, "ms")

    start = time.time()
    result_parallel = multiply_matrices_parallel(A, B)
    print("\nResult of matrix multiplication (Parallel):")
    # for row in result_parallel:
    #     print(row)
    print("Shape:", (len(result_parallel), len(result_parallel[0])))
    end = time.time()
    print("Duration Parallel: ", (end-start)*1000, "ms")

if __name__ == "__main__":
    main()
