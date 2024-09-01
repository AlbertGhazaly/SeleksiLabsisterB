using System;
using System.IO;

class Program
{
    static void SwapRows(double[][] matrix, int row1, int row2, int n)
    {
        for (int i = 0; i < n; i++)
        {
            double temp = matrix[row1][i];
            matrix[row1][i] = matrix[row2][i];
            matrix[row2][i] = temp;
        }
    }

    static void GaussJordan(double[][] matrix, double[][] inverse, int n)
    {
        for (int i = 0; i < n; i++)
        {
            // Cari elemen utama
            if (matrix[i][i] == 0)
            {
                for (int j = i + 1; j < n; j++)
                {
                    if (matrix[j][i] != 0)
                    {
                        SwapRows(matrix, i, j, n);
                        SwapRows(inverse, i, j, n);
                        break;
                    }
                }
            }

            // Normalisasi baris utama
            double pivot = matrix[i][i];
            for (int j = 0; j < n; j++)
            {
                matrix[i][j] /= pivot;
                inverse[i][j] /= pivot;
            }

            // Eliminasi baris lain
            for (int j = 0; j < n; j++)
            {
                if (j != i)
                {
                    double factor = matrix[j][i];
                    for (int k = 0; k < n; k++)
                    {
                        matrix[j][k] -= factor * matrix[i][k];
                        inverse[j][k] -= factor * inverse[i][k];
                    }
                }
            }
        }
    }

    static void Main()
    {
        string inputPath = "input/input.txt";
        string outputPath = "output/csharp.txt";

        if (!File.Exists(inputPath))
        {
            Console.WriteLine("File input tidak ditemukan!");
            return;
        }

        string[] lines = File.ReadAllLines(inputPath);
        int n = int.Parse(lines[0]);

        // Alokasi memori untuk matriks dan matriks invers
        double[][] matrix = new double[n][];
        double[][] inverse = new double[n][];
        for (int i = 0; i < n; i++)
        {
            matrix[i] = new double[n];
            inverse[i] = new double[n];
        }

        // Baca matriks dari file input dan inisialisasi matriks invers sebagai matriks identitas
        for (int i = 0; i < n; i++)
        {
            string[] row = lines[i + 1].Split(' ');
            for (int j = 0; j < n; j++)
            {
                matrix[i][j] = double.Parse(row[j]);
                inverse[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }

        // Hitung invers matriks menggunakan metode Gauss-Jordan
        GaussJordan(matrix, inverse, n);

        // Tulis hasil invers ke file output
        using (StreamWriter outputFile = new StreamWriter(outputPath))
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    outputFile.Write(inverse[i][j] + " ");
                }
                outputFile.WriteLine();
            }
        }

    }
}
