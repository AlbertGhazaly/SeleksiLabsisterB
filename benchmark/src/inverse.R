swap_rows <- function(matrix, row1, row2) {
  temp <- matrix[row1, ]
  matrix[row1, ] <- matrix[row2, ]
  matrix[row2, ] <- temp
}

gauss_jordan <- function(matrix) {
  n <- nrow(matrix)
  augmented_matrix <- cbind(matrix, diag(n))  
  
  for (i in 1:n) {
    augmented_matrix[i, ] <- augmented_matrix[i, ] / augmented_matrix[i, i]
    
    for (j in 1:n) {
      if (i != j) {
        augmented_matrix[j, ] <- augmented_matrix[j, ] - augmented_matrix[i, ] * augmented_matrix[j, i]
      }
    }
  }
  
  inverse_matrix <- augmented_matrix[, (n + 1):(2 * n)]
  return(inverse_matrix)
}

main <- function() {
  input_file <- file("input/input.txt", "r")
  output_file <- file("output/r.txt", "w")

  n <- as.integer(readLines(input_file, n = 1))
  matrix <- matrix(0, n, n)
  inverse <- diag(1, n)

  for (i in 1:n) {
    line <- readLines(input_file, n = 1)
    matrix[i, ] <- as.numeric(unlist(strsplit(line, " ")))
  }

  inverse <- gauss_jordan(matrix)
  
  for (i in 1:n) {
    writeLines(paste(inverse[i, ], collapse = " "), output_file)
  }

  close(input_file)
  close(output_file)
}

main()
