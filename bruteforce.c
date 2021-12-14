#include "bruteforce.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>

int* knn(int k, int d, int n, const double *points, const double* query) {
    int *k_array = malloc(k*sizeof(int));
    int i;
    for (i = 0; i < k; i++) {
        k_array[i] = i*d;
    }

    for (i = 0; i < n; i*= d){
        insert_if_closer(k, d, points, k_array, query, i);
    }
    return k_array;
}

// int* main(const double *points, const double* query) {
//     int* myk = knn(3, 2, 10, points, query);
//     return myk
// }
