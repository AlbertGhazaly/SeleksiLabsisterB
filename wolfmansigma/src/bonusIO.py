import time
from multiprocessing import Process, Queue
from concurrent.futures import ProcessPoolExecutor

def read_matrix_from_file(file_path, queue):
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
                matrices.append(matrix)
                matrix = []

    if matrix:
        matrices.append(matrix)

    queue.put(matrices)

def multiply_row(args):
    A, B, row = args
    cols_B = len(B[0])
    result_row = [0] * cols_B
    for j in range(cols_B):
        for k in range(len(B)):
            result_row[j] += A[row][k] * B[k][j]
    return result_row

def multiply_matrices_parallel(A, B):
    with ProcessPoolExecutor() as executor:
        result = list(executor.map(multiply_row, [(A, B, i) for i in range(len(A))]))
    return result

def main():
    file_path = '../tcmatmul/128.txt'
    queue = Queue()

    reader_process = Process(target=read_matrix_from_file, args=(file_path, queue))
    reader_process.start()

    matrices = queue.get()
    reader_process.join()

    if len(matrices) != 2:
        raise ValueError("File must contain exactly two matrices")

    A, B = matrices
    # print("Matrix A shape:", (len(A), len(A[0])))
    # print("Matrix B shape:", (len(B), len(B[0])))

    start_multiplication = time.time()
    result_parallel = multiply_matrices_parallel(A, B)
    end_multiplication = time.time()

    print("\nResult of matrix multiplication (Parallel):")
    print("Shape:", (len(result_parallel), len(result_parallel[0])))
    print("Duration: ", (end_multiplication - start_multiplication) * 1000, "ms")

if __name__ == "__main__":
    main()
