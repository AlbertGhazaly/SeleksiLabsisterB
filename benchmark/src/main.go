package main

import (
    "fmt"
    "os"
    "strconv"
)

func swapRows(matrix [][]float64, inverse [][]float64, row1 int, row2 int, n int) {
    for i := 0; i < n; i++ {
        matrix[row1][i], matrix[row2][i] = matrix[row2][i], matrix[row1][i]
        inverse[row1][i], inverse[row2][i] = inverse[row2][i], inverse[row1][i]
    }
}

func gaussJordan(matrix [][]float64, inverse [][]float64, n int) {
    for i := 0; i < n; i++ {
        if matrix[i][i] == 0 {
            for j := i + 1; j < n; j++ {
                if matrix[j][i] != 0 {
                    swapRows(matrix, inverse, i, j, n)
                    break
                }
            }
        }
        pivot := matrix[i][i]
        for j := 0; j < n; j++ {
            matrix[i][j] /= pivot
            inverse[i][j] /= pivot
        }
        for j := 0; j < n; j++ {
            if j != i {
                factor := matrix[j][i]
                for k := 0; k < n; k++ {
                    matrix[j][k] -= factor * matrix[i][k]
                    inverse[j][k] -= factor * inverse[i][k]
                }
            }
        }
    }
}

func main() {
    inputFile, err := os.Open("input/input.txt")
    if err != nil {
        fmt.Println("Error membuka file!")
        return
    }
    defer inputFile.Close()

    var n int
    fmt.Fscanf(inputFile, "%d\n", &n)

    matrix := make([][]float64, n)
    inverse := make([][]float64, n)
    for i := 0; i < n; i++ {
        matrix[i] = make([]float64, n)
        inverse[i] = make([]float64, n)
        for j := 0; j < n; j++ {
            fmt.Fscanf(inputFile, "%f", &matrix[i][j])
            if i == j {
                inverse[i][j] = 1.0
            } else {
                inverse[i][j] = 0.0
            }
        }
    }

    gaussJordan(matrix, inverse, n)

    outputFile, err := os.Create("output/go.txt")
    if err != nil {
        fmt.Println("Error membuka file output!")
        return
    }
    defer outputFile.Close()

    for i := 0; i < n; i++ {
        for j := 0; j < n; j++ {
            outputFile.WriteString(strconv.FormatFloat(inverse[i][j], 'f', -1, 64) + " ")
        }
        outputFile.WriteString("\n")
    }
}
