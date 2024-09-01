use std::fs::File;
use std::io::{BufRead, BufReader, Write};

fn swap_rows(matrix: &mut Vec<Vec<f64>>, inverse: &mut Vec<Vec<f64>>, row1: usize, row2: usize, n: usize) {
    for i in 0..n {
        matrix[row1][i] = matrix[row1][i] + matrix[row2][i];
        matrix[row2][i] = matrix[row1][i] - matrix[row2][i];
        matrix[row1][i] = matrix[row1][i] - matrix[row2][i];

        inverse[row1][i] = inverse[row1][i] + inverse[row2][i];
        inverse[row2][i] = inverse[row1][i] - inverse[row2][i];
        inverse[row1][i] = inverse[row1][i] - inverse[row2][i];
    }
}

fn gauss_jordan(matrix: &mut Vec<Vec<f64>>, inverse: &mut Vec<Vec<f64>>, n: usize) {
    for i in 0..n {
        if matrix[i][i] == 0.0 {
            for j in (i + 1)..n {
                if matrix[j][i] != 0.0 {
                    swap_rows(matrix, inverse, i, j, n);
                    break;
                }
            }
        }
        let pivot = matrix[i][i];
        for j in 0..n {
            matrix[i][j] /= pivot;
            inverse[i][j] /= pivot;
        }
        for j in 0..n {
            if j != i {
                let factor = matrix[j][i];
                for k in 0..n {
                    matrix[j][k] -= factor * matrix[i][k];
                    inverse[j][k] -= factor * inverse[i][k];
                }
            }
        }
    }
}

fn main() {
    let input_file = File::open("input/input.txt").unwrap();
    let reader = BufReader::new(input_file);
    let mut lines = reader.lines();

    let n: usize = lines.next().unwrap().unwrap().parse().unwrap();
    let mut matrix: Vec<Vec<f64>> = vec![vec![0.0; n]; n];
    let mut inverse: Vec<Vec<f64>> = vec![vec![0.0; n]; n];

    for i in 0..n {
        let line = lines.next().unwrap().unwrap();
        let values: Vec<f64> = line.split_whitespace().map(|s| s.parse().unwrap()).collect();
        for j in 0..n {
            matrix[i][j] = values[j];
            inverse[i][j] = if i == j { 1.0 } else { 0.0 };
        }
    }

    gauss_jordan(&mut matrix, &mut inverse, n);

    let mut output_file = File::create("output/rust.txt").unwrap();
    for i in 0..n {
        for j in 0..n {
            write!(output_file, "{} ", inverse[i][j]).unwrap();
        }
        writeln!(output_file).unwrap();
    }
}
