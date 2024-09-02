use strict;
use warnings;


sub add {
    my ($a, $b) = @_;
    
    my $int_part_a = int($a);
    my $frac_part_a = $a - $int_part_a;
    
    my $int_part_b = int($b);
    my $frac_part_b = $b - $int_part_b;

    while ($int_part_b) {
        my $carry = $int_part_a & $int_part_b;  
        $int_part_a ^= $int_part_b;              
        $int_part_b = $carry << 1;                
    }

    my $final_frac = $frac_part_a + $frac_part_b;

    if ($final_frac >= 1) {
        $int_part_a = subtract($int_part_a,-1);       
        $final_frac  = subtract($final_frac,1);    
    }

    return subtract($int_part_a , -$final_frac); 
}


sub subtract {
    my ($a, $b) = @_;
    my $is_negative = 0;

   
    while ($b) {
        my $borrow = (~$a) & $b;  
        $a ^= $b;                
        $b = $borrow << 1;       
    }


    if ($a & (1 << 31)) { 
        $is_negative = 1;

        $a = add(~($a) , 1); 
    }

    return ($is_negative ? -$a : $a); 
}

sub multiply {
    my ($a, $b) = @_;
    my $result = 0;
    while ($b) {
        if ($b & 1) { 
            $result = add($result, $a); 
        }
        $a <<= 1; 
        $b >>= 1; 
    }
    return $result;
}


sub power {
    my ($base, $exponent) = @_;
    my $result = 1;
    while ($exponent > 0) {
        $result = multiply($result, $base);
        $exponent = subtract($exponent, 1);
    }
    return $result;
}

sub float_divide {
    my ($a, $b) = @_;
    return 0 if $b == 0; 
    my $result = 0.0;

    while ($a >= $b) {
        $result = add($result,1);
        $a = subtract($a, $b);
    }

    my $fractional_part = 0.0;
    my $divisor = $b;
    my $i = 0;
    while ($i < 6){
        $a = multiply($a,10);
        my $fraction = 0;
        while ($a >= $divisor) {
            $fraction = add($fraction,1);
            $a = subtract($a,$divisor);
        }
        $fractional_part += $fraction / (10 ** ($i + 1));
        $i = add($i,1);
    }

    return $result+  $fractional_part; 
}


sub sqrt_custom {
    my ($x) = @_;
    return 0 if $x < 0; 
    return 0 if $x == 0; 
    return 1 if $x == 1; 

    my $guess = float_divide($x, 2.0);
    my $prev_guess = 0.0;

    while (abs($guess - $prev_guess) > 0.001) { 
        # printf "Guess %.3f adding val %.6f float div %.6f\n",$guess, add($guess, float_divide($x, $guess)),float_divide($x, $guess);
       
    
        $prev_guess = $guess;
        $guess = float_divide(add($guess, float_divide($x, $guess)), 2.0); 
        # printf "Guess %.3f adding val %.6f float div %.6f\n",$guess, add($guess, float_divide($x, $guess)),float_divide($x, $guess);
       
    }

    return sprintf("%.6f", $guess); 
}



sub evaluate_expression {
    my ($expression) = @_;
    $expression =~ s/R\((\d+)\)/sqrt_custom($1)/ge; 
    my @tokens = split /\s+/, $expression;

    my $result = 0;
    my $operation = "add";
    
    foreach my $token (@tokens) {
        if ($token =~ /^\d+$/ || $token =~ /^\d+\.\d+$/) { 
            if ($operation eq "add") {
                $result = add($result, $token);
            } elsif ($operation eq "subtract") {
                $result = subtract($result, $token);
            } elsif ($operation eq "multiply") {
                $result = multiply($result, $token);
            } elsif ($operation eq "divide") {
                $result = float_divide($result, $token);
            } elsif ($operation eq "**") {
                $result = power($result, $token);
            }
        } elsif ($token eq "+") {
            $operation = "add";
        } elsif ($token eq "-") {
            $operation = "subtract";
        } elsif ($token eq "*") {
            $operation = "multiply";
        } elsif ($token eq "/") {
            $operation = "divide";
        } elsif ($token eq "**") {
            $operation = "**"; 
        } else {
            return "Error: Invalid operator '$token'";
        }
    }
    return $result;
}

while (1) {
    print "Masukkan ekspresi ada spasinya \n(misalnya: 2 + 3 - 1 * 4 / R(16) atau 2 ** 3) atau ketik 'exit' untuk keluar\noperasi: ";
    my $input = <STDIN>;
    chomp($input);

    last if $input eq 'exit'; 

    my $output = evaluate_expression($input);
    print "Hasil: $output\n";
}

