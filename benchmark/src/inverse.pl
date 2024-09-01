use strict;
use warnings;

sub swap_rows {
    my ($matrix, $row1, $row2, $n) = @_;
    for my $i (0 .. $n - 1) {
        my $temp = $matrix->[$row1]->[$i];
        $matrix->[$row1]->[$i] = $matrix->[$row2]->[$i];
        $matrix->[$row2]->[$i] = $temp;
    }
}

sub gauss_jordan {
    my ($matrix, $inverse, $n) = @_;
    for my $i (0 .. $n - 1) {
        if ($matrix->[$i]->[$i] == 0) {
            for my $j ($i + 1 .. $n - 1) {
                if ($matrix->[$j]->[$i] != 0) {
                    swap_rows($matrix, $i, $j, $n);
                    swap_rows($inverse, $i, $j, $n);
                    last;
                }
            }
        }
        my $pivot = $matrix->[$i]->[$i];
        for my $j (0 .. $n - 1) {
            $matrix->[$i]->[$j] /= $pivot;
            $inverse->[$i]->[$j] /= $pivot;
        }
        for my $j (0 .. $n - 1) {
            if ($j != $i) {
                my $factor = $matrix->[$j]->[$i];
                for my $k (0 .. $n - 1) {
                    $matrix->[$j]->[$k] -= $factor * $matrix->[$i]->[$k];
                    $inverse->[$j]->[$k] -= $factor * $inverse->[$i]->[$k];
                }
            }
        }
    }
}

open my $input_file, '<', 'input/input.txt' or die "Error opening file!";
open my $output_file, '>', 'output/perl.txt' or die "Error opening file!";

my $n = <$input_file>;
chomp($n);

my @matrix;
my @inverse;

for my $i (0 .. $n - 1) {
    my @row = split ' ', <$input_file>;
    push @matrix, \@row;
    push @inverse, [(map {0} 1 .. $n)];
    $inverse[$i][$i] = 1;
}

gauss_jordan(\@matrix, \@inverse, $n);

for my $i (0 .. $n - 1) {
    print $output_file join(' ', @{$inverse[$i]}), "\n";
}

close $input_file;
close $output_file;
