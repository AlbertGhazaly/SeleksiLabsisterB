import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.Scanner;

public class inverse {
    static void swapRows(double[][] matrix, double[][] inverse, int row1, int row2, int n) {
        for (int i = 0; i < n; i++) {
            double temp = matrix[row1][i];
            matrix[row1][i] = matrix[row2][i];
            matrix[row2][i] = temp;
            temp = inverse[row1][i];
            inverse[row1][i] = inverse[row2][i];
            inverse[row2][i] = temp;
        }
    }

    static void gaussJordan(double[][] matrix, double[][] inverse, int n) {
        for (int i = 0; i < n; i++) {
            if (matrix[i][i] == 0) {
                for (int j = i + 1; j < n; j++) {
                    if (matrix[j][i] != 0) {
                        swapRows(matrix, inverse, i, j, n);
                        break;
                    }
                }
            }
            double pivot = matrix[i][i];
            for (int j = 0; j < n; j++) {
                matrix[i][j] /= pivot;
                inverse[i][j] /= pivot;
            }
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

    public static void main(String[] args) {
        try {
            Scanner inputFile = new Scanner(new File("input/input.txt"));
            PrintWriter outputFile = new PrintWriter("output/java.txt");

            int n = inputFile.nextInt();
            double[][] matrix = new double[n][n];
            double[][] inverse = new double[n][n];

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    matrix[i][j] = inputFile.nextDouble();
                    inverse[i][j] = (i == j) ? 1.0 : 0.0;
                }
            }

            gaussJordan(matrix, inverse, n);

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    outputFile.print(inverse[i][j] + " ");
                }
                outputFile.println();
            }

            inputFile.close();
            outputFile.close();
        } catch (FileNotFoundException e) {
            System.out.println("Error opening file!");
        }
    }
}
