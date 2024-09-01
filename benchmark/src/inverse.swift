import Foundation

func swapRows(matrix: inout [[Double]], row1: Int, row2: Int, n: Int) {
    let temp = matrix[row1]
    matrix[row1] = matrix[row2]
    matrix[row2] = temp
}

func gaussJordan(matrix: inout [[Double]], inverse: inout [[Double]], n: Int) {
    for i in 0..<n {
        if matrix[i][i] == 0 {
            for j in (i + 1)..<n {
                if matrix[j][i] != 0 {
                    swapRows(matrix: &matrix, row1: i, row2: j, n: n)
                    swapRows(matrix: &inverse, row1: i, row2: j, n: n)
                    break
                }
            }
        }
        let pivot = matrix[i][i]
        for j in 0..<n {
            matrix[i][j] /= pivot
            inverse[i][j] /= pivot
        }
        for j in 0..<n {
            if j != i {
                let factor = matrix[j][i]
                for k in 0..<n {
                    matrix[j][k] -= factor * matrix[i][k]
                    inverse[j][k] -= factor * inverse[i][k]
                }
            }
        }
    }
}

func main() {
    let inputFile = "input/input.txt"
    let outputFile = "output/swift.txt"
    
    var input: String
    do {
        input = try String(contentsOfFile: inputFile)
    } catch {
        print("Error reading input file!")
        return
    }
    
    let lines = input.split(separator: "\n").map { String($0) }
    let n = Int(lines[0])!
    
    var matrix = [[Double]](repeating: [Double](repeating: 0.0, count: n), count: n)
    var inverse = [[Double]](repeating: [Double](repeating: 0.0, count: n), count: n)
    
    for i in 0..<n {
        let rowValues = lines[i + 1].split(separator: " ").map { Double($0)! }
        for j in 0..<n {
            matrix[i][j] = rowValues[j]
            inverse[i][j] = (i == j) ? 1.0 : 0.0
        }
    }
    
    gaussJordan(matrix: &matrix, inverse: &inverse, n: n)

    var output = ""
    for row in inverse {
        output += row.map { String($0) }.joined(separator: " ") + "\n"
    }

    do {
        try output.write(toFile: outputFile, atomically: true, encoding: .utf8)
    } catch {
        print("Error writing output file!")
    }
}

main()
