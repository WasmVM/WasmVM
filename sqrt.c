// Reference: https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

void printbf(float value){
    const char *bit_rep[16] = {
        [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
        [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
        [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
        [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
    };
    printf("%f (", value);
    uint32_t ivalue = *(uint32_t*)&value;
    printf("%s", bit_rep[(ivalue >> 28) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 24) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 20) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 16) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 12) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 8) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 4) & 0xf]);
    printf("%s)\n", bit_rep[ivalue & 0xf]);
}

void printbd(double value){
    const char *bit_rep[16] = {
        [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
        [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
        [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
        [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
    };
    printf("%lf (", value);
    uint64_t ivalue = *(uint64_t*)&value;
    printf("%s", bit_rep[(ivalue >> 60) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 56) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 52) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 48) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 44) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 40) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 36) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 32) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 28) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 24) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 20) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 16) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 12) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 8) & 0xf]);
    printf("%s", bit_rep[(ivalue >> 4) & 0xf]);
    printf("%s)\n", bit_rep[ivalue & 0xf]);
}

float hacker_sqrtf(float x){
    union {
        int32_t i;
        float x;
    }u;
    // Magic number sqrt from "Hacker's Delight"
    u.x = x;
    u.i = 0x5f3759df - (u.i >> 1);
    u.x = x * u.x * (1.5f - 0.5f * x * u.x * u.x);
    // Two Babylonian Method
    u.x = 0.5 * (u.x + x / u.x);
    u.x = 0.5 * (u.x + x / u.x);
    return u.x;
}

double hacker_sqrtd(double x){
    union {
        int64_t i;
        double x;
    }u;
    // Magic number sqrt from "Hacker's Delight"
    u.x = x;
    u.i = 0x5fe6eb50c7b537a9LL - (u.i >> 1);
    u.x = x * u.x * (1.5 - 0.5f * x * u.x * u.x);
    // Three Babylonian Method
    u.x = 0.5 * (u.x + x / u.x);
    u.x = 0.5 * (u.x + x / u.x);
    u.x = 0.5 * (u.x + x / u.x);
    return u.x;
}

int32_t popcnt_32(uint32_t x){
    x = x - ((x >> 1) & 033333333333) - ((x >> 2) & 011111111111);
    return ((x + (x >> 3)) & 030707070707) % 63;
}
int64_t popcnt_64(uint64_t x){
    x -= ((x >> 1) & 0x5555555555555555LLU);
    x = (x & 0x3333333333333333LLU) + (x >> 2 & 0x3333333333333333LLU);
    return ((x + (x >> 4)) & 0xf0f0f0f0f0f0f0fLLU) * 0x101010101010101LLU >> 56;
}

int32_t clz_32(int32_t x){
    if(x == 0){
        return 32;
    }else{
        int32_t count = 0;
        if(!(x & 0xffff0000)){
            count += 16;
            x <<= 16;
        }
        if(!(x & 0xff000000)){
            count += 8;
            x <<= 8;
        }
        if(!(x & 0xf0000000)){
            count += 4;
            x <<= 4;
        }
        if(!(x & 0xc0000000)){
            count += 2;
            x <<= 2;
        }
        if(!(x & 0x80000000)){
            count += 1;
            x <<= 1;
        }
        return count;
    }
}

int32_t clz_64(int64_t x){
    if(x == 0){
        return 64;
    }else{
        int32_t count = 0;
        if(!(x & 0xffffffff00000000LLU)){
            count += 32;
            x <<= 32;
        }
        if(!(x & 0xffff000000000000LLU)){
            count += 16;
            x <<= 16;
        }
        if(!(x & 0xff00000000000000LLU)){
            count += 8;
            x <<= 8;
        }
        if(!(x & 0xf000000000000000LLU)){
            count += 4;
            x <<= 4;
        }
        if(!(x & 0xc000000000000000LLU)){
            count += 2;
            x <<= 2;
        }
        if(!(x & 0x8000000000000000LLU)){
            count += 1;
            x <<= 1;
        }
        return count;
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    unsigned epoch = atoi(argv[1]);
    uint32_t err_bits_float = 0;
    int32_t math_clock_float = 0, hack_clock_float = 0;
    int32_t min_clz_float = 32;
    printf("=== Float ===\n");
    for(unsigned i = 0; i < epoch; ++i){
        float value = (float)rand() / (float)(RAND_MAX);
        clock_t start = clock();
        float math_res = sqrtf(value);
        math_clock_float += clock() - start;
        start = clock();
        float hack_res = hacker_sqrtf(value);
        hack_clock_float += clock() - start;
        int32_t math_int = *(uint32_t*)&math_res, hack_int = *(uint32_t*)&hack_res;
        err_bits_float += abs(popcnt_32(math_int) - popcnt_32(hack_int));
        int32_t xor_clz = clz_32(abs(math_int - hack_int));
        if(xor_clz < min_clz_float){
            min_clz_float = xor_clz;
            printf("--- Epoch %d ---\n", i + 1);
            printf("Math.h sqrt: ");
            printbf(math_res);
            printf("Hacker sqrt: ");
            printbf(hack_res);
        }
    }
    uint64_t err_bits_double = 0;
    int32_t math_clock_double = 0, hack_clock_double = 0;
    int32_t min_clz_double = 64;
    printf("=== Double ===\n");
    for(unsigned i = 0; i < epoch; ++i){
        double value = (double)rand() / (double)(RAND_MAX);
        clock_t start = clock();
        double math_res = sqrt(value);
        math_clock_double += clock() - start;
        start = clock();
        double hack_res = hacker_sqrtd(value);
        hack_clock_double += clock() - start;
        int64_t math_int = *(uint64_t*)&math_res, hack_int = *(uint64_t*)&hack_res;
        err_bits_double += llabs(popcnt_64(math_int) - popcnt_64(hack_int));
        int32_t xor_clz = clz_64(llabs(math_int - hack_int));
        if(xor_clz < min_clz_double){
            min_clz_double = xor_clz;
            printf("Math.h sqrt: ");
            printbd(math_res);
            printf("Hacker sqrt: ");
            printbd(hack_res);
        }
    }
    printf("\n=== Summary ===\n");
    printf("Float accuracy: %lf\n", (1.0 - (double)err_bits_float / (double)(epoch * 32)) * 100.0);
    printf("Float performance: math: %d, hack: %d, percentage: %lf\n", math_clock_float, hack_clock_float, ((double)(math_clock_float - hack_clock_float) / (double)math_clock_float) * 100.0);
    printf("Float clz: %d\n", min_clz_float);
    printf("Double accuracy: %lf\n", (1.0 - (double)err_bits_double / (double)(epoch * 64)) * 100.0);
    printf("Double performance: math: %d, hack: %d, percentage: %lf\n", math_clock_double, hack_clock_double, ((double)(math_clock_double - hack_clock_double) / (double)math_clock_double) * 100.0);
    printf("Double clz: %d\n", min_clz_double);
    return 0;
}
