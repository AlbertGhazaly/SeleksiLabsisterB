#include <stdio.h>
#include <stdlib.h>

void swap_rows(double **matrix, int row1, int row2, int n) {
    for (int i = 0; i < n; i++) {
        double temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

void gauss_jordan(double **matrix, double **inverse, int n) {
    for (int i = 0; i < n; i++) {
        // Cari elemen utama
        if (matrix[i][i] == 0) {
            for (int j = i + 1; j < n; j++) {
                if (matrix[j][i] != 0) {
                    swap_rows(matrix, i, j, n);
                    swap_rows(inverse, i, j, n);
                    break;
                }
            }
        }

        // Normalisasi baris utama
        double pivot = matrix[i][i];
        for (int j = 0; j < n; j++) {
            matrix[i][j] /= pivot;
            inverse[i][j] /= pivot;
        }

        // Eliminasi baris lain
        for (int j = 0; j < n; j++) {
            if (j != i) {
                double factor = matrix[j][i];
                for (int k = 0; k < n; k++) {
                    matrix[j][k] -= factor * matrix[i][k];
                    inverse[j][k] -= factor * inverse[i][k];
                }
            }
        }
    }
}

int main() {
    FILE *inputFile = fopen("input/input.txt", "r");
    FILE *outputFile = fopen("output/c.txt", "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error membuka file!\n");
        return 1;
    }

    int n;
    fscanf(inputFile, "%d", &n);

    // Alokasi memori untuk matriks dan matriks invers
    double **matrix = (double **)malloc(n * sizeof(double *));
    double **inverse = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc(n * sizeof(double));
        inverse[i] = (double *)malloc(n * sizeof(double));
    }

    // Baca matriks dari file input dan inisialisasi matriks invers sebagai matriks identitas
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(inputFile, "%lf", &matrix[i][j]);
            inverse[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    // Hitung invers matriks menggunakan metode Gauss-Jordan
    gauss_jordan(matrix, inverse, n);

    // Tulis hasil invers ke file output
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(outputFile, "%lf ", inverse[i][j]);
        }
        fprintf(outputFile, "\n");
    }

    // Bebaskan memori
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
        free(inverse[i]);
    }
    free(matrix);
    free(inverse);

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
