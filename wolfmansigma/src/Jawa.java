import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Jawa {

    // Fungsi untuk membaca matriks dari file
    public static List<double[][]> readMatricesFromFile(String filePath) throws IOException {
        List<double[][]> matrices = new ArrayList<>();
        List<double[]> matrix = new ArrayList<>();
        BufferedReader reader = new BufferedReader(new FileReader(filePath));
        String line;
        int id = 0;

        while ((line = reader.readLine()) != null) {
            if (id != 0) {
                if (!line.trim().isEmpty()) {
                    String[] parts = line.trim().split("\\s+");
                    double[] row = new double[parts.length];
                    for (int i = 0; i < parts.length; i++) {
                        row[i] = Double.parseDouble(parts[i]);
                    }
                    matrix.add(row);
                } else {
                    if (!matrix.isEmpty()) {
                        double[][] arrayMatrix = matrix.toArray(new double[matrix.size()][]);
                        matrices.add(arrayMatrix);
                        matrix.clear();
                    }
                }
            }
            id++;
        }
        reader.close();

        if (!matrix.isEmpty()) {
            double[][] arrayMatrix = matrix.toArray(new double[matrix.size()][]);
            matrices.add(arrayMatrix);
        }

        return matrices;
    }

    // Fungsi untuk melakukan perkalian matriks
    public static double[][] multiplyMatrices(double[][] firstMatrix, double[][] secondMatrix) {
        int rowsInFirstMatrix = firstMatrix.length;
        int columnsInFirstMatrix = firstMatrix[0].length; // Sama dengan jumlah baris di matriks kedua
        int columnsInSecondMatrix = secondMatrix[0].length;
        double[][] result = new double[rowsInFirstMatrix][columnsInSecondMatrix];

        for (int i = 0; i < rowsInFirstMatrix; i++) {
            for (int j = 0; j < columnsInSecondMatrix; j++) {
                for (int k = 0; k < columnsInFirstMatrix; k++) {
                    result[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
                }
            }
        }

        return result;
    }
     // Fungsi untuk melakukan perkalian matriks secara parallel
     public static double[][] multiplyMatricesParallel(double[][] firstMatrix, double[][] secondMatrix) {
        int rowsInFirstMatrix = firstMatrix.length;
        int columnsInFirstMatrix = firstMatrix[0].length; // Sama dengan jumlah baris di matriks kedua
        int columnsInSecondMatrix = secondMatrix[0].length;
        double[][] result = new double[rowsInFirstMatrix][columnsInSecondMatrix];

        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new MatrixMultiplyTask(firstMatrix, secondMatrix, result, 0, rowsInFirstMatrix));
        pool.shutdown();

        return result;
    }

    // Kelas RecursiveTask untuk ForkJoinPool
    static class MatrixMultiplyTask extends RecursiveTask<Void> {
        private static final int THRESHOLD = 64; // Ukuran minimum untuk melakukan parallel
        private double[][] firstMatrix;
        private double[][] secondMatrix;
        private double[][] result;
        private int startRow;
        private int endRow;

        MatrixMultiplyTask(double[][] firstMatrix, double[][] secondMatrix, double[][] result, int startRow, int endRow) {
            this.firstMatrix = firstMatrix;
            this.secondMatrix = secondMatrix;
            this.result = result;
            this.startRow = startRow;
            this.endRow = endRow;
        }

        @Override
        protected Void compute() {
            int rowCount = endRow - startRow;
            if (rowCount <= THRESHOLD) {
                for (int i = startRow; i < endRow; i++) {
                    for (int j = 0; j < secondMatrix[0].length; j++) {
                        for (int k = 0; k < firstMatrix[0].length; k++) {
                            result[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
                        }
                    }
                }
            } else {
                int middle = (startRow + endRow) / 2;
                MatrixMultiplyTask task1 = new MatrixMultiplyTask(firstMatrix, secondMatrix, result, startRow, middle);
                MatrixMultiplyTask task2 = new MatrixMultiplyTask(firstMatrix, secondMatrix, result, middle, endRow);
                invokeAll(task1, task2);
            }
            return null;
        }
    }

    // Fungsi untuk mencetak matriks
    public static void printMatrix(double[][] matrix) {
        for (double[] row : matrix) {
            for (double value : row) {
                System.out.print(value + " ");
            }
            System.out.println();
        }
    }

    public static void main(String[] args) {
        try {
            List<double[][]> matrices = readMatricesFromFile("../tcmatmul/128.txt");
            if (matrices.size() != 2) {
                throw new IllegalArgumentException("File harus berisi tepat dua matriks");
            }
            
            double[][] firstMatrix = matrices.get(0);
            double[][] secondMatrix = matrices.get(1);
            long startTime = System.nanoTime();
            double[][] result = multiplyMatrices(firstMatrix, secondMatrix);
            long endTime = System.nanoTime();
            System.out.println("Hasil perkalian matriks:");
            // for (int i= 0;i<result.length;i++){
            //     System.out.print(result[i][0]+" ");
            // }
            System.out.println("");
            System.out.println("Shape: "+result.length+"x"+result[0].length);
            long duration = endTime - startTime;
            System.out.println("Durasi perhitungan (dalam milidetik): " + (duration / 1_000_000.0));

            startTime = System.nanoTime();
            double[][] result2 = multiplyMatricesParallel(firstMatrix, secondMatrix);
            endTime = System.nanoTime();
            System.out.println("Hasil perkalian matriks Paralel:");
            // for (int i= 0;i<result2.length;i++){
            //     System.out.print(result2[i][0]+" ");
            // }
            System.out.println("");
            System.out.println("Shape: "+result2.length+"x"+result2[0].length);
            duration = endTime - startTime;
            System.out.println("Durasi perhitungan (dalam milidetik): " + (duration / 1_000_000.0));
            
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
