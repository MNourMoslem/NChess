#include <stdio.h>
#include <time.h>

void func(int N, int num){
    typedef unsigned char u8;
    typedef unsigned long long u64;

    clock_t start_time = clock();

    u64 num64 = 0;
    for (long long i = 0; i < N; i++){
        num64 &= (u64)num;
    }

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    clock_t start_time_2 = clock();

    int c = 1;
    for (long long i = 0; i < N; i++){
        c*=num;
    }

    clock_t end_time_2 = clock();
    double time_spent_2 = (double)(end_time_2 - start_time_2) / CLOCKS_PER_SEC;

    printf("time of u64: %f\n", time_spent);
    printf("time of u8: %f\n", time_spent_2);
}

int main() {
    long long N = 2000000000;
    func(N, 10);

    return 0;
}
