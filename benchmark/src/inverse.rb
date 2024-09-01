def swap_rows(matrix, row1, row2, n)
    for i in 0...n
        temp = matrix[row1][i]
        matrix[row1][i] = matrix[row2][i]
        matrix[row2][i] = temp
    end
end

def gauss_jordan(matrix, inverse, n)
    for i in 0...n
        if matrix[i][i] == 0
            for j in (i + 1)...n
                if matrix[j][i] != 0
                    swap_rows(matrix, i, j, n)
                    swap_rows(inverse, i, j, n)
                    break
                end
            end
        end
        pivot = matrix[i][i]
        for j in 0...n
            matrix[i][j] /= pivot
            inverse[i][j] /= pivot
        end
        for j in 0...n
            if j != i
                factor = matrix[j][i]
                for k in 0...n
                    matrix[j][k] -= factor * matrix[i][k]
                    inverse[j][k] -= factor * inverse[i][k]
                end
            end
        end
    end
end

input_file = File.open("input/input.txt", "r")
output_file = File.open("output/ruby.txt", "w")

n = input_file.gets.to_i

matrix = Array.new(n) { Array.new(n) }
inverse = Array.new(n) { Array.new(n, 0) }

for i in 0...n
    matrix[i] = input_file.gets.chomp.split.map(&:to_f)
    inverse[i][i] = 1.0
end

gauss_jordan(matrix, inverse, n)

for i in 0...n
    output_file.puts inverse[i].join(" ")
end

input_file.close
output_file.close
