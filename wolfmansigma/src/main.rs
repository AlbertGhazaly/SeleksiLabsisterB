use ndarray::Array2;
use rayon::prelude::*;
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::time::Instant;

fn read_matrices_from_file(file_path: &str) -> Result<(Array2<f64>, Array2<f64>), Box<dyn std::error::Error>> {
    let file = File::open(file_path)?;
    let reader = BufReader::new(file);

    let mut matrices = Vec::new();
    let mut matrix = Vec::new();
    let mut num_cols = 0;
    let mut row_count = 0;

    for (line_id, line) in reader.lines().enumerate() {
        let line = line?;
        if line_id == 0 {
            continue;
        }
        if line.trim().is_empty() {
            if !matrix.is_empty() {
                matrices.push(matrix.clone());
                matrix.clear();
                row_count += 1;
                if row_count == 2 {
                    break;
                }
            }
        } else {
            let row: Vec<f64> = line.split_whitespace()
                .map(|s| s.parse().unwrap_or(0.0))
                .collect();
            if num_cols == 0 {
                num_cols = row.len();
            } else if row.len() != num_cols {
                return Err("Inconsistent number of columns in matrix".into());
            }
            matrix.push(row);
        }
    }

    if !matrix.is_empty() {
        matrices.push(matrix);
    }

    if matrices.len() != 2 {
        return Err("File must contain exactly two matrices".into());
    }

    let (rows1, cols1) = (matrices[0].len(), matrices[0][0].len());
    let (rows2, cols2) = (matrices[1].len(), matrices[1][0].len());

    Ok((Array2::from_shape_vec((rows1, cols1), matrices[0].concat())?,
        Array2::from_shape_vec((rows2, cols2), matrices[1].concat())?))
}

fn multiply_matrices_serial(first_matrix: &Array2<f64>, second_matrix: &Array2<f64>) -> Array2<f64> {
    let rows_in_first_matrix = first_matrix.nrows();
    let columns_in_first_matrix = first_matrix.ncols();
    let columns_in_second_matrix = second_matrix.ncols();

    if columns_in_first_matrix != second_matrix.nrows() {
        panic!("Incompatible matrix shapes for multiplication");
    }

    let mut result = Array2::zeros((rows_in_first_matrix, columns_in_second_matrix));

    for i in 0..rows_in_first_matrix {
        for j in 0..columns_in_second_matrix {
            result[(i, j)] = (0..columns_in_first_matrix).map(|k| first_matrix[(i, k)] * second_matrix[(k, j)]).sum();
        }
    }

    result
}

fn multiply_matrices_parallel(first_matrix: &Array2<f64>, second_matrix: &Array2<f64>) -> Array2<f64> {
    let rows_in_first_matrix = first_matrix.nrows();
    let columns_in_first_matrix = first_matrix.ncols();
    let columns_in_second_matrix = second_matrix.ncols();

    if columns_in_first_matrix != second_matrix.nrows() {
        panic!("Incompatible matrix shapes for multiplication");
    }

    let mut result = Array2::zeros((rows_in_first_matrix, columns_in_second_matrix));

    result.axis_iter_mut(ndarray::Axis(0)).enumerate().for_each(|(i, mut row)| {
        for j in 0..columns_in_second_matrix {
            row[j] = (0..columns_in_first_matrix).map(|k| first_matrix[(i, k)] * second_matrix[(k, j)]).sum();
        }
    });

    result
}

fn print_matrix(matrix: &Array2<f64>) {
    for row in matrix.rows() {
        println!("{:?}", row);
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let (first_matrix, second_matrix) = read_matrices_from_file("../tcmatmul/128.txt")?;

    // println!("Matriks Pertama:");
    // print_matrix(&first_matrix);
    // println!("Matriks Kedua:");
    // print_matrix(&second_matrix);

    let start_time_serial = Instant::now();
    let result_serial = multiply_matrices_serial(&first_matrix, &second_matrix);
    let duration_serial = start_time_serial.elapsed();

    println!("Hasil perkalian matriks (Serial):");
    // print_matrix(&result_serial);
    println!("Ukuran: {} x {}",result_serial.nrows(),result_serial.ncols());
    
    println!("Durasi perhitungan (Serial): {:?}", duration_serial);

    let start_time_parallel = Instant::now();
    let result_parallel = multiply_matrices_parallel(&first_matrix, &second_matrix);
    let duration_parallel = start_time_parallel.elapsed();

    println!("Hasil perkalian matriks (Parallel):");
    // print_matrix(&result_parallel);
    println!("Ukuran: {} x {}",result_parallel.nrows(),result_parallel.ncols());

    println!("Durasi perhitungan (Parallel): {:?}", duration_parallel);

    Ok(())
}
