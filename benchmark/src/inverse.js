const fs = require('fs');

function swapRows(matrix, inverse, row1, row2, n) {
    for (let i = 0; i < n; i++) {
        const temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

function gaussJordan(matrix, inverse, n) {
    for (let i = 0; i < n; i++) {
        if (matrix[i][i] === 0) {
            for (let j = i + 1; j < n; j++) {
                if (matrix[j][i] !== 0) {
                    swapRows(matrix, inverse, i, j, n);
                    break;
                }
            }
        }

        const pivot = matrix[i][i];
        for (let j = 0; j < n; j++) {
            matrix[i][j] /= pivot;
            inverse[i][j] /= pivot;
        }

        for (let j = 0; j < n; j++) {
            if (j !== i) {
                const factor = matrix[j][i];
                for (let k = 0; k < n; k++) {
                    matrix[j][k] -= factor * matrix[i][k];
                    inverse[j][k] -= factor * inverse[i][k];
                }
            }
        }
    }
}

const inputFile = 'input/input.txt';
const outputFile = 'output/javascript.txt';

const input = fs.readFileSync(inputFile, 'utf8').split('\n');
const n = parseInt(input[0]);

const matrix = [];
const inverse = [];

for (let i = 1; i <= n; i++) {
    const row = input[i].split(' ').map(Number);
    matrix.push(row);
    inverse.push(new Array(n).fill(0).map((_, index) => (index === i - 1 ? 1 : 0)));
}

gaussJordan(matrix, inverse, n);

let output = '';
for (const row of inverse) {
    output += row.join(' ') + '\n';
}

fs.writeFileSync(outputFile, output);
