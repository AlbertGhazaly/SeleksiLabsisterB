<?php
function swap_rows(&$matrix, $row1, $row2, $n) {
    for ($i = 0; $i < $n; $i++) {
        $temp = $matrix[$row1][$i];
        $matrix[$row1][$i] = $matrix[$row2][$i];
        $matrix[$row2][$i] = $temp;
    }
}

function gauss_jordan(&$matrix, &$inverse, $n) {
    for ($i = 0; $i < $n; $i++) {
        if ($matrix[$i][$i] == 0) {
            for ($j = $i + 1; $j < $n; $j++) {
                if ($matrix[$j][$i] != 0) {
                    swap_rows($matrix, $i, $j, $n);
                    swap_rows($inverse, $i, $j, $n);
                    break;
                }
            }
        }
        $pivot = $matrix[$i][$i];
        for ($j = 0; $j < $n; $j++) {
            $matrix[$i][$j] /= $pivot;
            $inverse[$i][$j] /= $pivot;
        }
        for ($j = 0; $j < $n; $j++) {
            if ($j != $i) {
                $factor = $matrix[$j][$i];
                for ($k = 0; $k < $n; $k++) {
                    $matrix[$j][$k] -= $factor * $matrix[$i][$k];
                    $inverse[$j][$k] -= $factor * $inverse[$i][$k];
                }
            }
        }
    }
}

$inputFile = fopen("input/input.txt", "r");
$outputFile = fopen("output/php.txt", "w");

$n = intval(fgets($inputFile));

$matrix = array();
$inverse = array();
for ($i = 0; $i < $n; $i++) {
    $matrix[$i] = array_map('floatval', explode(" ", trim(fgets($inputFile))));
    $inverse[$i] = array_fill(0, $n, 0);
    $inverse[$i][$i] = 1.0;
}

gauss_jordan($matrix, $inverse, $n);

for ($i = 0; $i < $n; $i++) {
    fwrite($outputFile, implode(" ", $inverse[$i]) . "\n");
}

fclose($inputFile);
fclose($outputFile);
?>
