#include <stdio.h>

int compute(int a, int b) {
    int temp1 = (a << 3) + 25;
    int temp2 = temp1 >> 31;
    int result1 = (temp2 + temp1) >> 1;

    int mul_result = result1 * b;
    int add_result = b + result1;
    int shift_result = add_result << (mul_result & 3);

    int or_result = shift_result | add_result;
    int final_result = or_result ^ mul_result;

    printf("%d %d\n", shift_result, add_result);
    return final_result;
}

int main() {
    int result1 = compute(42, 69);
    float float1 = (float)result1;

    int result2 = compute(69, 42);
    float float2 = (float)result2;

    float sum = float1 + float2;
    double final_result = (double)sum;

    printf("%f %f\n", (double)float1, final_result);
    
    return 0;
}
