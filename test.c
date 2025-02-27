#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long uint64;z

int main(){

    uint64 arr1[2][2] = {{1, 2}, {30, 4}};
    uint64 arr2[2][2];

    memcpy(arr2, arr1, sizeof(arr1));

    printf("%llu ", arr2[0][0]);
    printf("%llu ", arr2[0][1]);
    printf("%llu ", arr2[1][0]);
    printf("%llu ", arr2[1][1]);

    return 0;
}