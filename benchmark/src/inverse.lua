local function readMatrix(filename)
    local file = io.open(filename, "r")
    if not file then
        error("Could not open file " .. filename)
    end

    local n = tonumber(file:read("*line"))
    local matrix = {}

    for i = 1, n do
        local line = file:read("*line")
        local row = {}
        for num in line:gmatch("%S+") do
            table.insert(row, tonumber(num))
        end
        table.insert(matrix, row)
    end

    file:close()
    return n, matrix
end

local function invertMatrix(matrix, n)
    local augmented = {}
    for i = 1, n do
        augmented[i] = {}
        for j = 1, n do
            augmented[i][j] = matrix[i][j]
        end
        for j = 1, n do
            augmented[i][j + n] = (i == j) and 1 or 0
        end
    end

    for i = 1, n do
        local pivot = augmented[i][i]
        for j = 1, 2 * n do
            augmented[i][j] = augmented[i][j] / pivot
        end
        for j = 1, n do
            if j ~= i then
                local factor = augmented[j][i]
                for k = 1, 2 * n do
                    augmented[j][k] = augmented[j][k] - factor * augmented[i][k]
                end
            end
        end
    end

    local inverse = {}
    for i = 1, n do
        inverse[i] = {}
        for j = 1, n do
            inverse[i][j] = augmented[i][j + n]
        end
    end

    return inverse
end

local function writeMatrixToFile(matrix, filename)
    local file = io.open(filename, "w")
    if not file then
        error("Could not open file " .. filename)
    end

    for i = 1, #matrix do
        for j = 1, #matrix[i] do
            file:write(matrix[i][j] .. " ")
        end
        file:write("\n")
    end

    file:close()
end

local n, matrix = readMatrix("input/input.txt")
local inverse = invertMatrix(matrix, n)
writeMatrixToFile(inverse, "output/lua.txt")

