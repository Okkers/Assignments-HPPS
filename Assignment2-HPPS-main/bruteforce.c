#include "bruteforce.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#include<stdio.h>



int* knn(int k, int d, int n, const double *points, const double* query) {
    int *k_array = malloc(k*sizeof(int));
    for (int i = 0; i < k; i++) {
        k_array[i] = -1;
    }

    for (int i = 0; i < n; i++){
        // printf("Interation: %d\n", i);
        insert_if_closer(k, d, points, k_array, query, i);
    }
    // free(k_array);
    return k_array;
    free(k_array);
}


