#include <iostream>
// Poin A
int addInteger(int a, int b) {
    while (b != 0) {    // iterasi sampai b kosong dan hasil assign ke a
        int temp = a & b; 
        a = a ^ b;           
        b = temp << 1;
    }
    return a;
}

// Poin B
float addFloating754(float a, float b) {
    unsigned int int_a = *(unsigned int*)&a;
    unsigned int int_b = *(unsigned int*)&b;
    unsigned int int_result = addFloat(int_a, int_b);

    float result = *(float*)&int_result;
    return result;
}
unsigned int addFloat(unsigned int a, unsigned int b) {
    // ambil nilai tanda (negatif/positif), eksponen, dan mantisa dari float1
    unsigned int sign_a = (a >> 31) & 0x1;
    unsigned int exponent_a = (a >> 23) & 0xFF;
    unsigned int mantissa_a = a & 0x7FFFFF;

    // ambil nilai tanda (negatif/positif), eksponen, dan mantisa dari float2
    unsigned int sign_b = (b >> 31) & 0x1;
    unsigned int exponent_b = (b >> 23) & 0xFF;
    unsigned int mantissa_b = b & 0x7FFFFF;

    // Tambahin implicit leading 1
    if (exponent_a != 0) mantissa_a |= 0x800000;
    if (exponent_b != 0) mantissa_b |= 0x800000;

    // Menyesuaiin eksponen dengan menggeser mantissa yang lebih kecil
    if (exponent_a > exponent_b) {
        mantissa_b >>= (exponent_a - exponent_b);
        exponent_b = exponent_a;
    } else if (exponent_b > exponent_a) {
        mantissa_a >>= (exponent_b - exponent_a);
        exponent_a = exponent_b;
    }

     // operasi penjumlahan/pengurangan mantissa
    unsigned int result_mantissa;
    unsigned int result_sign = sign_a;

    if (sign_a == sign_b) {
        result_mantissa = mantissa_a + mantissa_b;
    } else {
        if (mantissa_a >= mantissa_b) {
            result_mantissa = mantissa_a - mantissa_b;
        } else {
            result_mantissa = mantissa_b - mantissa_a;
            result_sign = sign_b;
        }
    }

    // Normalisasi hasil
    unsigned int result_exponent = exponent_a;
    while (result_mantissa > 0xFFFFFF) {
        result_mantissa >>= 1;
        result_exponent++;
    }
    while (result_mantissa > 0 && (result_mantissa & 0x800000) == 0) {
        result_mantissa <<= 1;
        result_exponent--;
    }

    // edge case handling kalau hasilnya denomralized
    if (result_exponent <= 0) {
        result_mantissa >>= (1 - result_exponent);
        result_exponent = 0;
    } else if (result_exponent >= 255) {
        result_exponent = 255;  
        result_mantissa = 0;
    }

    // hapus implicit leading 1
    if (result_exponent != 0) result_mantissa &= 0x7FFFFF;

    // gabungin jadi hasil dan return
    unsigned int result = (result_sign << 31) | (result_exponent << 23) | result_mantissa;

    return result;
}


// implementasi
int main() {
    float num1 = 1.5f;
    float num2 = 2.5f;
    float result = addFloating754(num1, num2);

    int in1 = 2;
    int in2 = 4;
    int result2 = addInteger(in1,in2);
    std::cout<<"The result of adding integer "<< in1 <<" and "<< in2<<" is "<< result2 << std::endl; // hasil: 6
    std::cout << "The result of adding float " << num1 << " and " << num2 << " is " << result << std::endl; //hasil: 4
    return 0;
}


