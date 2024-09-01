def swap_rows(matrix, row1, row2, n):
    for i in range(n):
        matrix[row1][i], matrix[row2][i] = matrix[row2][i], matrix[row1][i]

def gauss_jordan(matrix, inverse, n):
    for i in range(n):
        if matrix[i][i] == 0:
            for j in range(i + 1, n):
                if matrix[j][i] != 0:
                    swap_rows(matrix, i, j, n)
                    swap_rows(inverse, i, j, n)
                    break
        pivot = matrix[i][i]
        for j in range(n):
            matrix[i][j] /= pivot
            inverse[i][j] /= pivot
        for j in range(n):
            if j != i:
                factor = matrix[j][i]
                for k in range(n):
                    matrix[j][k] -= factor * matrix[i][k]
                    inverse[j][k] -= factor * inverse[i][k]

inputFile = 'input/input.txt'
outputFile = 'output/python3.txt'

with open(inputFile, 'r') as f:
    n = int(f.readline())
    matrix = [[0] * n for _ in range(n)]
    inverse = [[1 if i == j else 0 for j in range(n)] for i in range(n)]
    for i in range(n):
        matrix[i] = list(map(float, f.readline().split()))

gauss_jordan(matrix, inverse, n)

with open(outputFile, 'w') as f:
    for i in range(n):
        f.write(' '.join(f'{inverse[i][j]:.6f}' for j in range(n)) + '\n')
