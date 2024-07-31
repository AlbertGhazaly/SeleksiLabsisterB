package main

import (
    "bufio"
    "fmt"
    "os"
    "strconv"
    "strings"
    "sync"
    "time"
)

// Membaca matriks dari file
func readMatricesFromFile(filePath string) ([][]float64, [][]float64, error) {
    file, err := os.Open(filePath)
    if err != nil {
        return nil, nil, err
    }
    defer file.Close()

    var matrices [][][]float64
    var matrix [][]float64
    scanner := bufio.NewScanner(file)
    id := 0

    for scanner.Scan() {
        line := scanner.Text()
        if id != 0 {
            if strings.TrimSpace(line) != "" {
                parts := strings.Fields(line)
                row := make([]float64, len(parts))
                for i, part := range parts {
                    row[i], _ = strconv.ParseFloat(part, 64)
                }
                matrix = append(matrix, row)
            } else {
                if len(matrix) > 0 {
                    matrices = append(matrices, matrix)
                    matrix = nil
                }
            }
        }
        id++
    }
    if len(matrix) > 0 {
        matrices = append(matrices, matrix)
    }

    if len(matrices) != 2 {
        return nil, nil, fmt.Errorf("file must contain exactly two matrices")
    }

    return matrices[0], matrices[1], nil
}

// Perkalian matriks secara serial
func multiplyMatricesSerial(firstMatrix, secondMatrix [][]float64) [][]float64 {
    rowsInFirstMatrix := len(firstMatrix)
    columnsInFirstMatrix := len(firstMatrix[0])
    columnsInSecondMatrix := len(secondMatrix[0])
    result := make([][]float64, rowsInFirstMatrix)
    for i := range result {
        result[i] = make([]float64, columnsInSecondMatrix)
    }

    for i := 0; i < rowsInFirstMatrix; i++ {
        for j := 0; j < columnsInSecondMatrix; j++ {
            for k := 0; k < columnsInFirstMatrix; k++ {
                result[i][j] += firstMatrix[i][k] * secondMatrix[k][j]
            }
        }
    }
    return result
}

// Perkalian matriks secara paralel
func multiplyMatricesParallel(firstMatrix, secondMatrix [][]float64) [][]float64 {
    rowsInFirstMatrix := len(firstMatrix)
    columnsInFirstMatrix := len(firstMatrix[0])
    columnsInSecondMatrix := len(secondMatrix[0])
    result := make([][]float64, rowsInFirstMatrix)
    for i := range result {
        result[i] = make([]float64, columnsInSecondMatrix)
    }

    var wg sync.WaitGroup
    numWorkers := rowsInFirstMatrix
    wg.Add(numWorkers)

    for i := 0; i < numWorkers; i++ {
        go func(row int) {
            defer wg.Done()
            for j := 0; j < columnsInSecondMatrix; j++ {
                for k := 0; k < columnsInFirstMatrix; k++ {
                    result[row][j] += firstMatrix[row][k] * secondMatrix[k][j]
                }
            }
        }(i)
    }

    wg.Wait()
    return result
}

// Mencetak matriks ke konsol
func printMatrix(matrix [][]float64) {
    for _, row := range matrix {
        for _, value := range row {
            fmt.Printf("%.10e ", value)
        }
        fmt.Println()
    }
}

// Program utama
func main() {
    firstMatrix, secondMatrix, err := readMatricesFromFile("../tcmatmul/32.txt")
    if err != nil {
        fmt.Println("Error reading matrices:", err)
        return
    }

    // Mengukur durasi perhitungan serial
    startTimeSerial := time.Now()
    resultSerial := multiplyMatricesSerial(firstMatrix, secondMatrix)
	endTimeSerial := time.Now()
    durationSerial := endTimeSerial.Sub(startTimeSerial).Microseconds()

    fmt.Println("Hasil perkalian matriks (Serial):")
    printMatrix(resultSerial)
    fmt.Printf("Durasi perhitungan (Serial): %3f ms\n", float64(durationSerial)/1000)

    // Mengukur durasi perhitungan parallel
    startTimeParallel := time.Now()
    resultParallel := multiplyMatricesParallel(firstMatrix, secondMatrix)
	endTImeParallel := time.Now()
    durationParallel := endTImeParallel.Sub(startTimeParallel).Microseconds()

    fmt.Println("Hasil perkalian matriks (Parallel):")
    printMatrix(resultParallel)
    fmt.Printf("Durasi perhitungan (Parallel): %3f ms\n", float64(durationParallel)/1000)
}
