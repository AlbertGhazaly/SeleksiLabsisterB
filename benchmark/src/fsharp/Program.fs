open System.IO

let swapRows (matrix: double[,]) (inverse: double[,]) (row1: int) (row2: int) (n: int) =
    for i in 0 .. n-1 do
        let temp = matrix.[row1, i]
        matrix.[row1, i] <- matrix.[row2, i]
        matrix.[row2, i] <- temp

        let tempInv = inverse.[row1, i]
        inverse.[row1, i] <- inverse.[row2, i]
        inverse.[row2, i] <- tempInv

let gaussJordan (matrix: double[,]) (inverse: double[,]) (n: int) =
    for i in 0 .. n-1 do
        if matrix.[i, i] = 0.0 then
            let mutable found = false
            for j in i+1 .. n-1 do
                if not found && matrix.[j, i] <> 0.0 then
                    swapRows matrix inverse i j n
                    found <- true
        let pivot = matrix.[i, i]
        for j in 0 .. n-1 do
            matrix.[i, j] <- matrix.[i, j] / pivot
            inverse.[i, j] <- inverse.[i, j] / pivot
        for j in 0 .. n-1 do
            if j <> i then
                let factor = matrix.[j, i]
                for k in 0 .. n-1 do
                    matrix.[j, k] <- matrix.[j, k] - factor * matrix.[i, k]
                    inverse.[j, k] <- inverse.[j, k] - factor * inverse.[i, k]

let main =
    let inputFile = "input/input.txt"
    let outputFile = "output/fsharp.txt"
    
    let lines = File.ReadAllLines(inputFile)
    let n = int(lines.[0])

    let matrix = Array2D.init n n (fun i j -> double (lines.[i + 1].Split(' ').[j]))
    let inverse = Array2D.init n n (fun i j -> if i = j then 1.0 else 0.0)

    gaussJordan matrix inverse n

    use writer = new StreamWriter(outputFile)
    for i in 0 .. n-1 do
        for j in 0 .. n-1 do
            writer.Write("{0} ", inverse.[i, j])
        writer.WriteLine()

main
