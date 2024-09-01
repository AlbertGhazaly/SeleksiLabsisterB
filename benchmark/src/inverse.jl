function swap_rows!(matrix, row1, row2)
    temp = matrix[row1, :]
    matrix[row1, :] = matrix[row2, :]
    matrix[row2, :] = temp
end

function gauss_jordan(matrix, inverse)
    n = size(matrix, 1)
    for i in 1:n
        if matrix[i, i] == 0
            for j in i+1:n
                if matrix[j, i] != 0
                    swap_rows!(matrix, i, j)
                    swap_rows!(inverse, i, j)
                    break
                end
            end
        end
        pivot = matrix[i, i]
        for j in 1:n
            matrix[i, j] /= pivot
            inverse[i, j] /= pivot
        end
        for j in 1:n
            if j != i
                factor = matrix[j, i]
                for k in 1:n
                    matrix[j, k] -= factor * matrix[i, k]
                    inverse[j, k] -= factor * inverse[i, k]
                end
            end
        end
    end
end

function main()
    inputFile = open("input/input.txt", "r")
    outputFile = open("output/julia.txt", "w")

    n = parse(Int, readline(inputFile))

    matrix = zeros(Float64, n, n)
    inverse = zeros(Float64, n, n)
    for i in 1:n
        inverse[i, i] = 1.0
    end

    for i in 1:n
        row = split(readline(inputFile), " ")
        for j in 1:n
            matrix[i, j] = parse(Float64, row[j])
        end
    end

    gauss_jordan(matrix, inverse)

    for i in 1:n
        for j in 1:n
            write(outputFile, string(inverse[i, j]) * " ")
        end
        write(outputFile, "\n")
    end

    close(inputFile)
    close(outputFile)
end

main()
