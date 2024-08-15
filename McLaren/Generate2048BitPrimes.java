import java.math.BigInteger;
import java.security.SecureRandom;

public class Generate2048BitPrimes {

    private static final int BIT_LENGTH = 2048; // Bit length for the prime numbers

    public static void main(String[] args) {
        // Generate two large 2048-bit primes
        BigInteger p = generatePrime(BIT_LENGTH);
        BigInteger q = generatePrime(BIT_LENGTH);

        System.out.println("Prime p: " + p);
        System.out.println("Prime q: " + q);
    }

    // Generate a large prime number with specified bit length
    private static BigInteger generatePrime(int bitLength) {
        SecureRandom random = new SecureRandom();
        return BigInteger.probablePrime(bitLength, random);
    }
}

