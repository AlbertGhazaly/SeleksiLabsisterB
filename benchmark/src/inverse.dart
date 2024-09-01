import 'dart:io';

void swapRows(List<List<double>> matrix, List<List<double>> inverse, int row1, int row2, int n) {
  for (int i = 0; i < n; i++) {
    double temp = matrix[row1][i];
    matrix[row1][i] = matrix[row2][i];
    matrix[row2][i] = temp;
  }
}

void gaussJordan(List<List<double>> matrix, List<List<double>> inverse, int n) {
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

void main() {
  File inputFile = File('input/input.txt');
  File outputFile = File('output/dart.txt');

  List<String> lines = inputFile.readAsLinesSync();
  int n = int.parse(lines[0]);

  List<List<double>> matrix = List.generate(n, (_) => List<double>.filled(n, 0.0));
  List<List<double>> inverse = List.generate(n, (_) => List<double>.filled(n, 0.0));

  for (int i = 0; i < n; i++) {
    List<String> values = lines[i + 1].split(' ');
    for (int j = 0; j < n; j++) {
      matrix[i][j] = double.parse(values[j]);
      inverse[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  gaussJordan(matrix, inverse, n);

  outputFile.writeAsStringSync('');
  for (int i = 0; i < n; i++) {
    outputFile.writeAsStringSync(inverse[i].join(' ') + '\n', mode: FileMode.append);
  }
}
